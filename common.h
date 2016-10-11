#ifndef _COMMON_H_
#define _COMMON_H_

#include <vector>
#include <string>
#include <cstdint>
#include <iostream>

using namespace std;


// main.cpp
extern vector<uint32_t> words;
extern uint32_t cur_addr;
extern string cur_line;
extern int cur_line_at;

void close_asm_and_exit();
void inc_addr();
uint32_t label_to_addr(string label);

// inst.cpp
void process_instruction(vector<string> elems);

// util.cpp
vector<string> split_string(const string &str, const string &delims);
uint32_t bin_to_word(string bin);
string num_to_bin(uint32_t num, int len);

// report.cpp
void report_error(string message);
void report_warning(string message);
void report_cur_line();

#endif

