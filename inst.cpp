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
    return static_cast<int32_t>(label_to_text_addr(label)) - static_cast<int32_t>(cur_text_addr);
}

string gen_R_type(string op, vector<string> args)
{
    string opcode, funct3, funct7, rd, rs1, rs2;

    if (op == "slli" || op == "srli" || op == "srai")
        opcode = "0010011";
    else if (op == "add" || op == "sub" || op == "sll" || op == "slt" || op == "sltu" || op == "xor"
            || op == "srl" || op == "sra" || op == "or" || op == "and")
        opcode = "0110011";
    else
        opcode = "1010011";

    if (op == "slli")
        funct3 = "001";
    else if (op == "srli" || op == "srai")
        funct3 = "101";
    else if (op == "add" || op == "sub")
        funct3 = "000";
    else if (op == "sll")
        funct3 = "001";
    else if (op == "slt")
        funct3 = "010";
    else if (op == "sltu")
        funct3 = "011";
    else if (op == "xor")
        funct3 = "100";
    else if (op == "srl" || op == "sra")
        funct3 = "101";
    else if (op == "or")
        funct3 = "110";
    else if (op == "and")
        funct3 = "111";
    else if (op == "fsgnj.s")
        funct3 = "000";
    else if (op == "fsgnjn.s")
        funct3 = "001";
    else if (op == "fsgnjx.s")
        funct3 = "010";
    else if (op == "fmin.s")
        funct3 = "000";
    else if (op == "fmax.s")
        funct3 = "001";
    else if (op == "feq.s")
        funct3 = "010";
    else if (op == "flt.s")
        funct3 = "001";
    else if (op == "fle.s")
        funct3 = "000";
    else if (op == "fmv.s.x")
        funct3 = "000";
    else
        funct3 = "000"; // rm = RNE

    if (op == "slli" || op == "srli" || op == "add" || op == "sll" || op == "slt" || op == "sltu"
            || op == "xor" || op == "srl" || op == "or" || op == "and")
        funct7 = "0000000";
    else if (op == "srai" || op == "sub" || op == "sra")
        funct7 = "0100000";
    else if (op == "fadd.s")
        funct7 = "0000000";
    else if (op == "fsub.s")
        funct7 = "0000100";
    else if (op == "fmul.s")
        funct7 = "0001000";
    else if (op == "fdiv.s")
        funct7 = "0001100";
    else if (op == "fsqrt.s")
        funct7 = "0101100";
    else if (op == "fsgnj.s" || op == "fsgnjn.s" || op == "fsgnjx.s")
        funct7 = "0010000";
    else if (op == "fmin.s" || op == "fmax.s")
        funct7 = "0010100";
    else if (op == "fcvt.w.s" || op == "fcvt.wu.s")
        funct7 = "1100000";
    else if (op == "feq.s" || op == "flt.s" || op == "fle.s")
        funct7 = "1010000";
    else if (op == "fcvt.s.w" || op == "fcvt.s.wu")
        funct7 = "1101000";
    else if (op == "fmv.s.x")
        funct7 = "1111000";

    rd = reg_to_bin(args[0]);
    rs1 = reg_to_bin(args[1]);
    if (op == "slli" || op == "srli" || op == "srai") // shamt
        rs2 = num_to_bin(stoul(args[2]), 5);
    else if (op == "fsqrt.s" || op == "fcvt.w.s" || op == "fcvt.s.w" || op == "fmv.s.x")
        rs2 = "00000";
    else if (op == "fcvt.wu.s" || op == "fcvt.s.wu")
        rs2 = "00001";
    else
        rs2 = reg_to_bin(args[2]);

    return funct7 + rs2 + rs1 + funct3 + rd + opcode; // reversed
}

string gen_I_type(string op, vector<string> args)
{
    string opcode, funct3, imm, rd, rs1;

    if (op == "addi" || op == "slti" || op == "sltiu" || op == "xori" || op == "ori" || op == "andi")
        opcode = "0010011";
    else if (op == "jalr")
        opcode = "1100111";
    else if (op == "lw")
        opcode = "0000011";
    else if (op == "flw")
        opcode = "0000111";

    if (op == "addi")
        funct3 = "000";
    else if (op == "slti")
        funct3 = "010";
    else if (op == "sltiu")
        funct3 = "011";
    else if (op == "xori")
        funct3 = "100";
    else if (op == "ori")
        funct3 = "110";
    else if (op == "andi")
        funct3 = "111";
    else if (op == "jalr")
        funct3 = "000";
    else if (op == "lw" || op == "flw")
        funct3 = "010";

    imm = num_to_bin(stoi(args[2]), 12);
    rd = reg_to_bin(args[0]);
    rs1 = reg_to_bin(args[1]);

    return imm + rs1 + funct3 + rd + opcode;
}

// Special I
string gen_IS_type(string op, vector<string> args)
{
    string opcode, rd;

    if (op == "inb")
        opcode = "0000010";

    rd = reg_to_bin(args[0]);

    return num_to_bin(0, 20) + rd + opcode;
}

string gen_S_type(string op, vector<string> args)
{
    string opcode, funct3, imm, rs1, rs2;

    if (op == "sw")
        opcode = "0100011";
    else if (op == "fsw")
        opcode = "0100111";

    funct3 = "010";

    imm = num_to_bin(stoi(args[2]), 12);
    rs2 = reg_to_bin(args[0]);
    rs1 = reg_to_bin(args[1]);

    return imm.substr(0, 7) + rs2 + rs1 + funct3 + imm.substr(7) + opcode;
}

// Special S
string gen_SS_type(string op, vector<string> args)
{
    string opcode, rs2;

    if (op == "outb")
        opcode = "0000110";

    rs2 = reg_to_bin(args[0]);

    return num_to_bin(0, 7) + rs2 + num_to_bin(0, 13) + opcode;
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
    else if (op == "bltu")
        funct3 = "110";
    else if (op == "bgeu")
        funct3 = "111";

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

string gen_U_type(string op, vector<string> args)
{
    string opcode, imm, rd;

    if (op == "lui")
        opcode = "0110111";
    else if (op == "auipc")
        opcode = "0010111";

    imm = num_to_bin(stoul(args[1]), 20);
    rd = reg_to_bin(args[0]);

    return imm + rd + opcode;
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

    if (op == "lw" || op == "flw" || op == "sw" || op == "fsw") {
        string b = args.back();
        args.pop_back();
        vector<string> ps = split_string(b, "()");
        args.push_back(ps[1]); // reg comes first
        if (isdigit(ps[0][0]) || ps[0][0] == '-')
            args.push_back(ps[0]);
        else
            args.push_back(to_string(label_to_data_addr(ps[0])));
    } else if (op == "addi") {
        string b = args.back();
        if (!(isdigit(b[0]) || b[0] == '-')) {
            args.pop_back();
            args.push_back(to_string(label_to_text_addr(b)));
        }
    }

    string inst;
    if (op == "slli" || op == "srli" || op == "srai" || op == "add" || op == "sub"
            || op == "sll" || op == "slt" || op == "sltu" || op == "xor"
            || op == "srl" || op == "sra" || op == "or" || op == "and"
            || op == "fadd.s" || op == "fsub.s" || op == "fmul.s" || op == "fdiv.s"
            || op == "fsqrt.s" || op == "fsgnj.s" || op == "fsgnjn.s" || op == "fsgnjx.s"
            || op == "fmin.s" || op == "fmax.s" || op == "fcvt.w.s" || op == "fcvt.wu.s"
            || op == "feq.s" || op == "flt.s" || op == "fle.s"
            || op == "fcvt.s.w" || op == "fcvt.s.wu" || op == "fmv.s.x")
        inst = gen_R_type(op, args);
    else if (op == "addi" || op == "slti" || op == "sltiu" || op == "xori" || op == "ori" || op == "andi"
            || op == "lw" || op == "flw" || op == "jalr")
        inst = gen_I_type(op, args);
    else if (op == "sw" || op == "fsw")
        inst = gen_S_type(op, args);
    else if (op == "beq" || op == "bne" || op == "blt" || op == "bge" || op == "bltu" || op == "bgeu")
        inst = gen_SB_type(op, args);
    else if (op == "lui" || op == "auipc")
        inst = gen_U_type(op, args);
    else if (op == "jal")
        inst = gen_UJ_type(op, args);
    else if (op == "inb")
        inst = gen_IS_type(op, args);
    else if (op == "outb")
        inst = gen_SS_type(op, args);
    else if (op == "halt")
        inst = num_to_bin(0, 25) + "0000000";
    else {
        report_error("invalid instruction");
        report_cur_line();
        close_asm_and_exit();
    }

    text_words.push_back(bin_to_word(inst));
}

