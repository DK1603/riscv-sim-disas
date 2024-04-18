#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <iomanip>

using namespace std;


// Function to sign-extend a word
int32_t signExtend(uint32_t val, unsigned bits) {
    int32_t signBit = 1 << (bits - 1);
    val &= ((1 << bits) - 1);
    return (val ^ signBit) - signBit;
}

string disassembleInstruction(uint32_t instruction) {
    
    uint32_t opcode = instruction & 0x7F; // Opcode is the last 7 bits
    uint32_t rd, rs1, rs2, funct3, funct7; // Parts of the opcode
    int32_t imm;

    // Extract common fields
    rd = (instruction >> 7) & 0x1F;
    funct3 = (instruction >> 12) & 0x07;
    rs1 = (instruction >> 15) & 0x1F;
    rs2 = (instruction >> 20) & 0x1F;

    // I-type for immediate instructions
    imm = signExtend(instruction >> 20, 12);

    // R-type for register operations
    funct7 = instruction >> 25;

    string result;

// Decode based on opcode
switch (opcode) {
    case 0x33: // R-Type
            switch (funct3) {
                case 0x0:
                    if (funct7 == 0x00) {
                        result = "add ";
                    } else if (funct7 == 0x20) {
                        result = "sub ";
                    }
                    break;
                case 0x1:
                    result = "sll ";
                    break;
                case 0x2:
                    result = "slt ";
                    break;
                case 0x3:
                    result = "sltu ";
                    break;
                case 0x4:
                    result = "xor ";
                    break;
                case 0x5:
                    if (funct7 == 0x00) {
                        result = "srl ";
                    } else if (funct7 == 0x20) {
                        result = "sra ";
                    }
                    break;
                case 0x6:
                    result = "or ";
                    break;
                case 0x7:
                    result = "and ";
                    break;
            }
            result += "x" + to_string(rd) + ", x" + to_string(rs1) + ", x" + to_string(rs2);
            break;

        case 0x03: // I-Type load for opcode
            switch (funct3) {
                case 0x0:
                    result = "lb ";
                    break;
                case 0x1:
                    result = "lh ";
                    break;
                case 0x2:
                    result = "lw ";
                    break;
                case 0x4:
                    result = "lbu ";
                    break;
                case 0x5:
                    result = "lhu ";
                    break;
            }
            result += "x" + to_string(rd) + ", " + to_string(imm) + "(x" + to_string(rs1) + ")";
            break;

        case 0x13: // I-Type(immediate) for opcode
            switch (funct3) {
                case 0x0:
                    result = "addi ";
                    break;
                case 0x2:
                    result = "slti ";
                    break;
                case 0x3:
                    result = "sltiu ";
                    break;
                case 0x4:
                    result = "xori ";
                    break;
                case 0x6:
                    result = "ori ";
                    break;
                case 0x7:
                    result = "andi ";
                    break;
                case 0x1:
                    result = "slli ";
                    break;
                case 0x5:
                    if (((instruction >> 25) & 0x7F) == 0x00) {
                        result = "srli ";
                    } else {
                        result = "srai ";
                        imm = imm & 0x1F; // extracting the lower 5 bits for the shift amount
                    }
                    break;
            }
            result += "x" + to_string(rd) + ", x" + to_string(rs1) + ", " + to_string(imm);
            break;
    
    case 0x63: // B-Type instructions (branches) for opcode
            imm = signExtend(((instruction >> 7) & 0x1E) | 
                             ((instruction >> 25) & 0x3F) << 5 | 
                             ((instruction >> 7) & 0x1) << 11 | 
                             ((instruction >> 31) & 0x1) << 12, 13);
            
            //beq, bne, blt, bge, bltu, bgeu
            switch (funct3) {  
                case 0x0:
                    result = "beq x" + to_string(rs1) + ", x" + to_string(rs2) + ", " + to_string(imm);
                    break;
                case 0x1:
                    result = "bne x" + to_string(rs1) + ", x" + to_string(rs2) + ", " + to_string(imm);
                    break;
                case 0x4:
                    result = "blt x" + to_string(rs1) + ", x" + to_string(rs2) + ", " + to_string(imm);
                    break;
                case 0x5:
                    result = "bge x" + to_string(rs1) + ", x" + to_string(rs2) + ", " + to_string(imm);
                    break;
                case 0x6:
                    result = "bltu x" + to_string(rs1) + ", x" + to_string(rs2) + ", " + to_string(imm);
                    break;
                case 0x7:
                    result = "bgeu x" + to_string(rs1) + ", x" + to_string(rs2) + ", " + to_string(imm);
                    break;

            }
            break;


    case 0x23: // S-Type instructions (store) for opcode
            imm = signExtend((instruction >> 25) << 5 | (instruction >> 7) & 0x1F, 12);
            switch (funct3) {
                case 0x0:
                    result = "sb x" + to_string(rs2) + ", " + to_string(imm) + "(x" + to_string(rs1) + ")";
                    break;
                case 0x1:
                    result = "sh x" + to_string(rs2) + ", " + to_string(imm) + "(x" + to_string(rs1) + ")";
                    break;
                case 0x2:
                    result = "sw x" + to_string(rs2) + ", " + to_string(imm) + "(x" + to_string(rs1) + ")";
                    break;
                
            }
            break;

    // Four remaining cases: lui, auipc, jal and jalr
    case 0x37: // lui
        imm = (instruction >> 12); // Immediate is in the upper 20 bits
        result = "lui x" + to_string(rd) + ", " + to_string(imm << 12);
        break;

    case 0x17: // auipc
        imm = instruction >> 12; // Immediate is in the upper 20 bits
        result = "auipc x" + to_string(rd) + ", " + to_string(imm << 12);
        break;

    case 0x6F: // jal
        imm = signExtend(((instruction >> 12) & 0xFF) << 12 | // bits 19:12
                         ((instruction >> 20) & 0x1) << 11 | // bit 20
                         ((instruction >> 21) & 0x3FF) << 1 | // bits 30:21
                         ((instruction >> 31) & 0x1) << 20, 21);// bit 31
    
        result = "jal x" + to_string(rd) + ", " + to_string(imm);
        break;

    case 0x67: // jalr
        imm = signExtend(instruction >> 20, 12); // Immediate is the same as I type
        result = "jalr x" + to_string(rd) + ", " + to_string(imm) + "(x" + to_string(rs1) + ")";
        break;

    
    default:
        result = "unknown instruction";
}

    return result;
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <binary file path>" <<endl;
        return 1;
    }

    ifstream file(argv[1], ios::binary);
    if (!file.is_open()) {
        cerr << "Error opening file: " << argv[1] << endl;
        return 1;
    }

    vector<uint8_t> buffer((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

    // Assume each instruction is 4 bytes and file size is multiple of 4 bytes
    if (buffer.size() % 4 !=0) {
        cerr << "File size is not a multiple of 4 bytes." << endl;
        return 1;
    }

    // Process each instruction
    for (size_t i = 0; i < buffer.size(); i += 4) {
        
        uint32_t instruction = (buffer[i + 3] << 24) | (buffer[i + 2] << 16) | (buffer[i + 1] << 8) | (buffer[i]); // 4 bytes into 32 bits

        // Disassemble the instruction
        string disassembled = disassembleInstruction(instruction);

        int instruction_cnt = i / 4; // Number of instructions assuming multiples of 4 bytes
        if (instruction_cnt < 10) {

            // Print the disassembled instruction with hex number (if cnt <= 10)
            cout << "inst " << setfill('0') << setw(1) << hex << instruction_cnt << ": " << setw(8) << instruction << " " << disassembled << endl;
        } else {

            // Print the disassembled instruction with decimal number (if cnt > 10)
            cout << "inst " << dec << instruction_cnt << ": " << setfill('0') << setw(8) << hex << instruction << " " << disassembled << endl;
        }
    }

    return 0;
}
