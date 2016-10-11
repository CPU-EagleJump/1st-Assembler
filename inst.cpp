#include <string>
#include <vector>
#include <cstdlib>

using namespace std;

#include "common.h"

string reg_to_bin(string reg)
{
	return num_to_bin(stoul(reg.substr(1)), 5);
}

string num_to_imm(string num)
{
	return num_to_bin(static_cast<uint32_t>(stoi(num)), 12);
}

int32_t calc_offset(string label)
{
	return static_cast<int32_t>(label_to_addr(label)) - static_cast<int32_t>(cur_addr);
}

string gen_R_type(string op, vector<string> args)
{
	string opcode, funct3, funct7, rd, rs1, rs2;

	if (op == "add" || op == "sub")
		opcode = "0110011";
	if (op == "add" || op == "sub")
		funct3 = "000";
	if (op == "add")
		funct7 = "0000000";
	else if (op == "sub")
		funct7 = "0100000";
	rd = reg_to_bin(args[0]);
	rs1 = reg_to_bin(args[1]);
	rs2 = reg_to_bin(args[2]);

	return funct7 + rs2 + rs1 + funct3 + rd + opcode; // reversed
}

string gen_I_type(string op, vector<string> args)
{
	string opcode, funct3, imm, rd, rs1;

	if (op == "addi")
		opcode = "0010011";
	else if (op == "jalr")
		opcode = "1100111";
	else if (op == "lw")
		opcode = "0000011";
	if (op == "addi" || op == "jalr")
		funct3 = "000";
	else if (op == "lw")
		funct3 = "010";
	imm = num_to_bin(stoi(args[2]), 12);
	rd = reg_to_bin(args[0]);
	rs1 = reg_to_bin(args[1]);

	return imm + rs1 + funct3 + rd + opcode;
}

string gen_S_type(string op, vector<string> args)
{
	string opcode, funct3, imm, rs1, rs2;

	if (op == "sw")
		opcode = "0100011";
	if (op == "sw")
		funct3 = "010";
	imm = num_to_bin(stoi(args[2]), 12);
	rs2 = reg_to_bin(args[0]);
	rs1 = reg_to_bin(args[1]);

	return imm.substr(0, 7) + rs2 + rs1 + funct3 + imm.substr(7) + opcode;
}

string gen_SB_type(string op, vector<string> args)
{
	string opcode, funct3, imm, rs1, rs2;

	opcode = "1100011";
	if (op == "beq")
		funct3 = "000";
	else if (op == "bne")
		funct3 = "001";
	else if (op == "blt")
		funct3 = "100";
	else if (op == "bge")
		funct3 = "101";
	int32_t offset = calc_offset(args[2]);
	if (abs(offset) > (1 << 13)) {
		report_error("too far branch");
		report_cur_line();
		close_asm_and_exit();
	}
	imm = num_to_bin(offset, 13);
	rs1 = reg_to_bin(args[0]);
	rs2 = reg_to_bin(args[1]);

	return imm.substr(0, 1) + imm.substr(2, 6) + rs2 + rs1 + funct3 + imm.substr(8, 4) + imm.substr(1, 1) + opcode;
}

string gen_UJ_type(string op, vector<string> args)
{
	string opcode, imm, rd;

	opcode = "1101111";
	int32_t offset = calc_offset(args[1]);
	if (abs(offset) > (1 << 21)) {
		report_error("too far jump");
		report_cur_line();
		close_asm_and_exit();
	}
	imm = num_to_bin(offset, 21);
	rd = reg_to_bin(args[0]);

	return imm.substr(0, 1) + imm.substr(10, 10) + imm.substr(9, 1) + imm.substr(1, 8) + rd + opcode;
}

void process_instruction(vector<string> elems)
{
    string op = elems[0];
    vector<string> args(elems.begin() + 1, elems.end());

	if (op == "lw" || op == "sw") {
		string b = args.back();
		args.pop_back();
		vector<string> ps = split_string(b, "()");
		args.push_back(ps[1]); // reg comes first
		args.push_back(ps[0]);
	}

    string inst;
    if (op == "add" || op == "sub")
        inst = gen_R_type(op, args);
    else if (op == "addi" || op == "lw" || op == "jalr")
        inst = gen_I_type(op, args);
    else if (op == "sw")
        inst = gen_S_type(op, args);
    else if (op == "beq" || op == "bne" || op == "blt" || op == "bge")
        inst = gen_SB_type(op, args);
    else if (op == "jal")
        inst = gen_UJ_type(op, args);
    else if (op == "halt")
        inst = num_to_bin(0, 25) + "0000000";
    else {
		report_error("invalid instruction");
		report_cur_line();
		close_asm_and_exit();
	}

    words.push_back(bin_to_word(inst));
}

