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

const uint32_t WORD_SIZE = 4;

vector<uint32_t> words;

uint32_t cur_addr;
map<string, uint32_t> label_map;

string cur_line;
int cur_line_at;

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

// place magic number "ZOI!"
void place_magic_number()
{
    words.push_back((uint32_t)'Z' | (uint32_t)'O' << 8 | (uint32_t)'I' << 16 | (uint32_t)'!' << 24);
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
	return label_map[label];
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
                if (is_gen)
                    process_instruction(elems);
				inc_addr();
            }
        }
    }
}

// write words to file
void write_words()
{
    int bytes_size = words.size() * 4;
    char *bytes = new char[bytes_size];
    for (size_t at = 0; at < words.size(); at++) {
        uint32_t w = words[at];
        bytes[at * 4] = (char)w;
        bytes[at * 4 + 1] = (char)(w >> 8);
        bytes[at * 4 + 2] = (char)(w >> 16);
        bytes[at * 4 + 3] = (char)(w >> 24);
    }

    zoi_file.write(bytes, bytes_size);
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        report_error("no input file");
        exit(1);
    }

    asm_name = argv[1];
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

	process_lines(false);

	// return to beginning
	asm_file.clear();
	asm_file.seekg(0, ios::beg);

    // place_magic_number();
    process_lines(true);

    asm_file.close();

    zoi_file.open(zoi_name, ios::out | ios::trunc | ios::binary);
    write_words();
	zoi_file.close();
}

