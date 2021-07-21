#include "CPU6502.h"

int CPU::ADC() {
	Fetch();
	unWord t = (unWord)a + (unWord)uTemp + (PGetBit(CBit) ? 1 : 0);

	PResetBit(CBit);
	if (t > 255) {
		PSetBit(CBit);
	}

	CheckNeg((unByte)t);
	CheckZero((unByte)t);

	PResetBit(VBit);
	if ( ( ~((unWord)a ^ (unWord)uTemp) & ((unWord)a ^ (unWord)t) ) & 0x0080) {
		PSetBit(VBit);
	}

	a = t & 0x00ff;

	return 1;
}

int CPU::AND() {
	Fetch();
	a = a & uTemp;
	CheckNeg(a);
	CheckZero(a);
	return 1;
}

int CPU::ASL() {
	Fetch();
	unWord t = (unWord)(uTemp << 1);
	PResetBit(CBit);
	if (t & 0xff00) {
		PSetBit(CBit);
	}
	CheckZero((unByte)t);
	CheckNeg((unByte)t);
	if (optable[op].addrmode == &CPU::IMP)
		a = t & 0x00ff;
	else
		Write(dir, t & 0x00ff);
	return 0;
}

int CPU::BCC() {
	if (!PGetBit(CBit)) {
		cycles++;
		dir = pc + relDir;

		if ((dir & 0xff00) != (pc & 0xff00)) {
			cycles++;
		}

		pc = dir;
	}
	return 0;
}

int CPU::BCS() {
	if (PGetBit(CBit)) {
		cycles++;
		dir = pc + relDir;

		if ((dir & 0xff00) != (pc & 0xff00)) {
			cycles++;
		}

		pc = dir;
	}
	return 0;
}

int CPU::BEQ() {
	if (PGetBit(ZBit)) {
		cycles++;
		dir = pc + relDir;

		if ((dir & 0xff00) != (pc & 0xff00)) {
			cycles++;
		}

		pc = dir;
	}
	return 0;
}

int CPU::BIT() {
	Fetch();
	if (uTemp & (1 << 7)) {
		PSetBit(NBit);
	}
	else {
		PResetBit(NBit);
	}
	if (uTemp & (1 << 6)) {
		PSetBit(VBit);
	}
	else {
		PResetBit(VBit);
	}
	CheckZero(a & uTemp);
	return 0;
}

int CPU::BMI() {
	if (PGetBit(NBit)) {
		cycles++;
		dir = pc + relDir;

		if ((dir & 0xff00) != (pc & 0xff00)) {
			cycles++;
		}

		pc = dir;
	}
	return 0;
}

int CPU::BNE() {
	if (!PGetBit(ZBit)) {
		cycles++;
		dir = pc + relDir;

		if ((dir & 0xff00) != (pc & 0xff00)) {
			cycles++;
		}

		pc = dir;
	}
	return 0;
}

int CPU::BPL() {
	if (!PGetBit(NBit)) {
		cycles++;
		dir = pc + relDir;

		if ((dir & 0xff00) != (pc & 0xff00)) {
			cycles++;
		}

		pc = dir;
	}
	return 0;
}

int CPU::BRK() {
	pc++;
	PushW(pc);
	PSetBit(IBit);
	PSetBit(BBit);
	Push(p);
	PResetBit(BBit);

	pc = ReadW((unWord)0xfffe);
	return 0;
}

int CPU::BVC() {
	if (!PGetBit(VBit)) {
		cycles++;
		dir = pc + relDir;

		if ((dir & 0xff00) != (pc & 0xff00)) {
			cycles++;
		}

		pc = dir;
	}
	return 0;
}

int CPU::BVS() {
	if (PGetBit(VBit)) {
		cycles++;
		dir = pc + relDir;

		if ((dir & 0xff00) != (pc & 0xff00)) {
			cycles++;
		}

		pc = dir;
	}
	return 0;
}

int CPU::CLC() {
	PResetBit(CBit);
	return 0;
}

int CPU::CLD() {
	PResetBit(DBit);
	return 0;
}

int CPU::CLI() {
	PResetBit(IBit);
	return 0;
}

int CPU::CLV() {
	PResetBit(VBit);
	return 0;
}

int CPU::CMP() {
	Fetch();
	unWord t = (unWord)a - (unWord)uTemp;
	CheckNeg((unByte)t);
	CheckZero((unByte)t);

	PResetBit(CBit);
	if (a >= uTemp) {
		PSetBit(CBit);
	}

	return 1;
}

int CPU::CPX() {
	Fetch();
	unWord t = (unWord)x - (unWord)uTemp;
	CheckNeg((unByte)t);
	CheckZero((unByte)t);

	PResetBit(CBit);
	if (x >= uTemp) {
		PSetBit(CBit);
	}
	return 0;
}

int CPU::CPY() {
	Fetch();
	unWord t = (unWord)y - (unWord)uTemp;
	CheckNeg((unByte)t);
	CheckZero((unByte)t);

	PResetBit(CBit);
	if (y >= uTemp) {
		PSetBit(CBit);
	}
	return 0;
}

int CPU::DEC() {
	Fetch();
	unWord t = uTemp - 1;
	CheckNeg((unByte)t);
	CheckZero((unByte)t);

	Write(dir, t & 0x00ff);
	return 0;
}

int CPU::DEX() {
	x--;
	CheckNeg(x);
	CheckZero(x);
	return 0;
}

int CPU::DEY() {
	y--;
	CheckNeg(y);
	CheckZero(y);
	return 0;
}

int CPU::EOR() {
	Fetch();
	a = a ^ uTemp;
	CheckNeg(a);
	CheckZero(a);
	return 1;
}

int CPU::INC() {
	Fetch();
	unWord t = uTemp + 1;
	CheckNeg((unByte)t);
	CheckZero((unByte)t);

	Write(dir, t & 0x00ff);
	return 0;
}

int CPU::INX() {
	x++;
	CheckNeg(x);
	CheckZero(x);
	return 0;
}

int CPU::INY() {
	y++;
	CheckNeg(y);
	CheckZero(y);
	return 0;
}

int CPU::JMP() {
	pc = dir;
	return 0;
}

int CPU::JSR() {
	PushW(--pc);
	pc = dir;
	return 0;
}

int CPU::LDA() {
	Fetch();
	a = uTemp;
	CheckNeg(a);
	CheckZero(a);
	return 1;
}

int CPU::LDX() {
	Fetch();
	x = uTemp;
	CheckNeg(x);
	CheckZero(x);
	return 1;
}

int CPU::LDY() {
	Fetch();
	y = uTemp;
	CheckNeg(y);
	CheckZero(y);
	return 1;
}

int CPU::LSR() {
	Fetch();
	PResetBit(CBit);
	if (uTemp & 0x01)
		PSetBit(CBit);
	unWord t = uTemp >> 1;
	CheckNeg((unByte)t);
	CheckZero((unByte)t);
	if (optable[op].addrmode == &CPU::IMP)
		a = t & 0x00ff;
	else
		Write(dir, t & 0x00ff);
	return 0;
}

int CPU::NOP() {
	return 0;
}

int CPU::ORA() {
	Fetch();
	a = a | uTemp;
	CheckNeg(a);
	CheckZero(a);
	return 1;
}

int CPU::PHA() {
	Push(a);
	return 0;
}

int CPU::PHP() {
	Push(p | BBit | UBit);
	PResetBit(BBit);
	return 0;
}

int CPU::PLA() {
	a = Pop();
	CheckNeg(a);
	CheckZero(a);
	return 0;
}

int CPU::PLP() {
	p = Pop();
	PSetBit(UBit);
	return 0;
}

int CPU::ROL() {
	Fetch();
	unWord t = (unWord)(uTemp << 1) | ((PGetBit(CBit) ? 1 : 0));
	PResetBit(CBit);
	if (t & 0xff00) {
		PSetBit(CBit);
	}
	CheckZero((unByte)t);
	CheckNeg((unByte)t);
	if (optable[op].addrmode == &CPU::IMP)
		a = t & 0x00ff;
	else
		Write(dir, t & 0x00ff);
	return 0;
}

int CPU::ROR() {
	Fetch();
	unWord t = (unWord)((PGetBit(CBit) ? 1 : 0) << 7) | (uTemp >> 1);
	PResetBit(CBit);
	if (uTemp & 0x01) {
		PSetBit(CBit);
	}
	CheckZero((unByte)t);
	CheckNeg((unByte)t);
	if (optable[op].addrmode == &CPU::IMP)
		a = t & 0x00ff;
	else
		Write(dir, t & 0x00ff);
	return 0;
}

int CPU::RTI() {
	p = Pop();
	PResetBit(BBit);
	PSetBit(UBit);
	pc = PopW();
	return 0;
}

int CPU::RTS() {
	pc = PopW();
	pc++;
	return 0;
}

int CPU::SBC() {
	Fetch();
	unWord val = ((unWord)uTemp) ^ 0x00ff;

	unWord t = (unWord)a + val + (PGetBit(CBit) ? 1 : 0);

	PResetBit(CBit);
	if (t & 0xff00) {
		PSetBit(CBit);
	}

	CheckNeg((unByte)t);
	CheckZero((unByte)t);

	PResetBit(VBit);
	if ((t ^ (unWord)a) & (t ^ val) & 0x0080) {
		PSetBit(VBit);
	}

	a = t & 0x00ff;

	return 1;
}

int CPU::SEC() {
	PSetBit(CBit);
	return 0;
}

int CPU::SED() {
	PSetBit(DBit);
	return 0;
}

int CPU::SEI() {
	PSetBit(IBit);
	return 0;
}

int CPU::STA() {
	Write(dir, a);
	return 0;
}

int CPU::STX() {
	Write(dir, x);
	return 0;
}

int CPU::STY() {
	Write(dir, y);
	return 0;
}

int CPU::TAX() {
	x = a;
	CheckNeg(x);
	CheckZero(x);
	return 0;
}

int CPU::TAY() {
	y = a;
	CheckNeg(y);
	CheckZero(y);
	return 0;
}

int CPU::TSX() {
	x = sp;
	CheckNeg(x);
	CheckZero(x);
	return 0;
}

int CPU::TXA() {
	a = x;
	CheckNeg(a);
	CheckZero(a);
	return 0;
}

int CPU::TXS() {
	sp = x;
	return 0;
}

int CPU::TYA() {
	a = y;
	CheckNeg(a);
	CheckZero(a);
	return 0;
}

int CPU::XXX() {
	return 0;
}
