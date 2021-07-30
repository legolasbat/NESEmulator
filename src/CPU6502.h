#pragma once

#include <string>
#include <vector>

#include "Utils.h"

#define CBit 0x01
#define ZBit 0x02
#define IBit 0x04
#define DBit 0x08
#define BBit 0x10
#define UBit 0x20
#define VBit 0x40
#define NBit 0x80

class NES;

class CPU {
public:
	CPU();
	void ConnectMemory(NES* mem) { memory = mem; }
	void Clock();
	void Reset();
	void IRQ();
	void NMI();

private:
	// Registers
	unWord pc = 0x0000;
	unByte a = 0, x = 0, y = 0, sp = 0xfd, p = 0x24;

	void Push(unByte b);
	void PushW(unWord w);
	unByte Pop();
	unWord PopW();

	void Write(unWord dir, unByte b);
	unByte Read(unWord dir);
	void WriteW(unWord dir, unWord w);
	unWord ReadW(unWord dir);

	void Fetch();

	void PSetBit(int pos);
	void PResetBit(int pos);
	bool PGetBit(int pos);

	void CheckNeg(unByte n);
	void CheckZero(unByte n);

	int cycles = 7;
	int tCycles = 0;

	NES *memory = nullptr;
	
	unWord dir = 0x0000;
	unWord relDir = 0x0000;
	unByte uTemp = 0x00;
	unByte op = 0x00;

	struct OP {
		std::string name;
		int(CPU::* operate)(void) = nullptr;
		int(CPU::* addrmode)(void) = nullptr;
		int cycles = 0;
	};

	std::vector<OP> optable;

	// Addressing Modes
	int IMP();	int IMM();
	int ZP0();	int ZPX();
	int ZPY();	int REL();
	int ABS();	int ABX();
	int ABY();	int IND();
	int IZX();	int IZY();

	// Opcodes
	int ADC();	int AND();	int ASL();	int BCC();
	int BCS();	int BEQ();	int BIT();	int BMI();
	int BNE();	int BPL();	int BRK();	int BVC();
	int BVS();	int CLC();	int CLD();	int CLI();
	int CLV();	int CMP();	int CPX();	int CPY();
	int DEC();	int DEX();	int DEY();	int EOR();
	int INC();	int INX();	int INY();	int JMP();
	int JSR();	int LDA();	int LDX();	int LDY();
	int LSR();	int NOP();	int ORA();	int PHA();
	int PHP();	int PLA();	int PLP();	int ROL();
	int ROR();	int RTI();	int RTS();	int SBC();
	int SEC();	int SED();	int SEI();	int STA();
	int STX();	int STY();	int TAX();	int TAY();
	int TSX();	int TXA();	int TXS();	int TYA();

	int XXX();
};