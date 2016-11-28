#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <cstdlib>
#include <cstdint>
#include <iostream>

using namespace std;

#include "common.h"


string asm_name, zoi_name;
ifstream asm_file;
ofstream zoi_file;

bool is_debug = false;

const uint32_t WORD_SIZE = 4;

vector<uint32_t> words;

uint32_t cur_addr;
map<string, uint32_t> label_map;

string cur_line;
int cur_line_at;
vector<uint32_t> inst_lines;

void close_asm_and_exit()
{
	asm_file.close();
	exit(1);
}

void init_addr()
{
    cur_addr = 0;
}

void inc_addr()
{
    cur_addr += WORD_SIZE;
}

void process_label(string label)
{
    if (label_map.find(label) != label_map.end()) {
        report_warning("duplicate labels; overwriting");
        report_cur_line();
    }
    label_map[label] = cur_addr;
}

uint32_t label_to_addr(string label)
{
    auto it = label_map.find(label);
    if (it == label_map.end()) {
        report_error("no such label");
		report_cur_line();
		close_asm_and_exit();
    }

	return it->second;
}

void process_directive(vector<string> elems)
{
}

// read lines and generate words
void process_lines(bool is_gen)
{
    init_addr();
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
				if (is_gen)
					process_directive(elems);
			}
            else {
                if (is_gen) {
                    process_instruction(elems);
                    inst_lines.push_back(cur_line_at);
                }
				inc_addr();
            }
        }
    }
}

void write_word(uint32_t w)
{
    char bs[WORD_SIZE];
    bs[0] = (char)w;
    bs[1] = (char)(w >> 8);
    bs[2] = (char)(w >> 16);
    bs[3] = (char)(w >> 24);

    zoi_file.write(bs, WORD_SIZE);
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

    if (params.size() == 0) {
        report_error("no input file");
        exit(1);
    }

    asm_name = params[0];
    if (asm_name.size() < 2 || asm_name.substr(asm_name.size() - 2) != ".s") {
        report_error("invalid file type");
        exit(1);
    }
    zoi_name = asm_name.substr(0, asm_name.size() - 2) + ".zoi";

    asm_file.open(asm_name, ios::in);
    if (asm_file.fail()) {
        report_error("no such file");
        exit(1);
    }

    for (string opt : options)
        if (opt == "-g")
            is_debug = true;

	process_lines(false);

	// return to beginning
	asm_file.clear();
	asm_file.seekg(0, ios::beg);

    process_lines(true);

    asm_file.close();

    zoi_file.open(zoi_name, ios::out | ios::trunc | ios::binary);

    // header
    if (is_debug)
        zoi_file.write("ZOI?", 4);
    else
        zoi_file.write("ZOI!", 4);

    write_word(words.size());

    // text
    for (uint32_t w : words)
        write_word(w);

    // debug info
    if (is_debug) {
        for (uint32_t l : inst_lines)
            write_word(l);

        // copy
        ifstream asm_bin_file;
        asm_bin_file.open(asm_name, ios::in | ios::binary);
        zoi_file << asm_bin_file.rdbuf();
    }

	zoi_file.close();
}

