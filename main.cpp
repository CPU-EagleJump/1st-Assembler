#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <cstdlib>
#include <cstdint>
#include <iostream>

using namespace std;

#include "common.h"


ifstream asm_file;

const uint32_t WORD_SIZE = 4;

vector<uint32_t> text_words, data_words;

bool is_text_now;

uint32_t cur_text_addr, cur_data_addr;
map<string, uint32_t> text_label_map, data_label_map;

string cur_line;
int cur_line_at;
vector<uint32_t> inst_lines;

void close_asm_and_exit()
{
    asm_file.close();
    exit(1);
}

void process_label(string label)
{
    if (is_text_now) {
        if (text_label_map.find(label) != text_label_map.end()) {
            report_warning("duplicate labels; overwriting");
            report_cur_line();
        }
        text_label_map[label] = cur_text_addr;
    } else {
        if (data_label_map.find(label) != data_label_map.end()) {
            report_warning("duplicate labels; overwriting");
            report_cur_line();
        }
        data_label_map[label] = cur_data_addr;
    }
}

uint32_t label_to_text_addr(string label)
{
    auto it = text_label_map.find(label);
    if (it == text_label_map.end()) {
        report_error("no such label");
        report_cur_line();
        close_asm_and_exit();
    }

    return it->second;
}

uint32_t label_to_data_addr(string label)
{
    auto it = data_label_map.find(label);
    if (it == data_label_map.end()) {
        report_error("no such label");
        report_cur_line();
        close_asm_and_exit();
    }

    return it->second;
}

void process_directive(vector<string> elems, bool is_gen)
{
    string op = elems[0];
    vector<string> args(elems.begin() + 1, elems.end());

    if (op == ".data")
        is_text_now = false;
    else if (op == ".text")
        is_text_now = true;
    else if (op == ".long") {
        if (is_gen) {
            if (is_text_now) {
                report_cur_line();
                report_warning(".long directive is used in .text section");
            }
            uint32_t d = stoul(args[0], nullptr, 16);
            data_words.push_back(d);
        }
        cur_data_addr += WORD_SIZE;
    } else {
        report_error("invalid directive");
        report_cur_line();
        close_asm_and_exit();
    }
}

// read lines and generate words
void process_lines(bool is_gen)
{
    is_text_now = true;
    cur_text_addr = cur_data_addr = 0;
    cur_line_at = 0;

    while (!asm_file.eof()) {
        getline(asm_file, cur_line);
        cur_line_at++;

        string line = cur_line;

        // erase comment
        auto h_at = line.find('#');
        if (h_at != string::npos)
            line.erase(h_at);

        vector<string> elems = split_string(line, " \t,");

        if (elems.size() > 0) {
            if (elems[0].back() == ':') {
                if (!is_gen)
                    process_label(elems[0].substr(0, elems[0].size() - 1));
            } else if (elems[0].front() == '.') {
                process_directive(elems, is_gen);
            }
            else {
                if (is_gen) {
                    if (!is_text_now) {
                        report_error("instruction is used in .data section");
                        report_cur_line();
                        close_asm_and_exit();
                    }
                    process_instruction(elems);
                    inst_lines.push_back(cur_line_at);
                }
                cur_text_addr += WORD_SIZE;
            }
        }
    }
}

void write_word(ofstream &ofs, uint32_t w)
{
    char bs[WORD_SIZE];
    bs[0] = (char)w;
    bs[1] = (char)(w >> 8);
    bs[2] = (char)(w >> 16);
    bs[3] = (char)(w >> 24);

    ofs.write(bs, WORD_SIZE);
}

int main(int argc, char **argv)
{
    vector<string> params, options;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-')
            options.push_back(argv[i]);
        else
            params.push_back(argv[i]);
    }

    // params

    if (params.size() == 0) {
        report_error("no input file");
        exit(1);
    }

    string asm_name = params[0];
    if (asm_name.size() < 2 || asm_name.substr(asm_name.size() - 2) != ".s") {
        report_error("invalid file type");
        exit(1);
    }

    asm_file.open(asm_name, ios::in);
    if (asm_file.fail()) {
        report_error("no such file");
        exit(1);
    }

    // options

    bool is_debug = true, is_ascii = false;
    for (string opt : options) {
        if (opt == "-no-debug")
            is_debug = false;
        if (opt == "-ascii")
            is_ascii = true;
    }

    process_lines(false);

    // return to beginning
    asm_file.clear();
    asm_file.seekg(0, ios::beg);

    process_lines(true);

    asm_file.close();


    string base_name = asm_name.substr(0, asm_name.size() - 2);

    if (is_ascii) {
        string data_name = base_name + ".zoi.data", text_name = base_name + ".zoi.text";
        ofstream data_file, text_file;

        data_file.open(data_name, ios::out | ios::trunc);
        for (uint32_t w : data_words) {
            data_file << num_to_bin(w, 32) << endl;
        }
        data_file.close();

        text_file.open(text_name, ios::out | ios::trunc);
        for (uint32_t w : text_words) {
            text_file << num_to_bin(w, 32) << endl;
        }
        text_file.close();
    } else {
        string zoi_name = base_name + ".zoi";
        ofstream zoi_file;
        zoi_file.open(zoi_name, ios::out | ios::trunc | ios::binary);

        // header
        if (is_debug)
            zoi_file.write("ZOI?", 4);
        else
            zoi_file.write("ZOI!", 4);

        write_word(zoi_file, data_words.size());
        write_word(zoi_file, text_words.size());

        // data
        for (uint32_t w : data_words)
            write_word(zoi_file, w);

        // text
        for (uint32_t w : text_words)
            write_word(zoi_file, w);

        // debug info
        if (is_debug) {
            for (uint32_t l : inst_lines)
                write_word(zoi_file, l);

            // copy
            ifstream asm_bin_file;
            asm_bin_file.open(asm_name, ios::in | ios::binary);
            zoi_file << asm_bin_file.rdbuf();
        }

        zoi_file.close();
    }
}

