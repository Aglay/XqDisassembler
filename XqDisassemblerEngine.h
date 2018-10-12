#pragma once
#include<Windows.h>
#include<iostream>
#include<vector>
#include<map>
#include<string>
#include<sstream>
#include <iomanip>
#include<algorithm>
using namespace std;

typedef struct _MODR_M
{
	union
	{
		struct
		{
			BYTE R_M : 3;	
			BYTE Reg : 3;
			BYTE Mod : 2;
		};
		BYTE B_MOD;
	};
}_MODR_M;

typedef enum TYPE
{
	T_NONE = 0x0,
	DIRECT = 0x100,
	SIM	= 0x200,
	DISP8 = 0x400,
	DISP32 = 0x800,
	DISP16 = 0x1000,
	SIM_DIP8 = (SIM | DISP8),
	SIM_DISP32 = (SIM | DISP32)
};

typedef enum ADDR_MODE
{
	ADDR_NONE = 0,
	ADDR_BYTE = 1,
	ADDR_WORD = 2,
	ADDR_DWORD = 4
};

typedef struct Analy_SIB
{
	TYPE type;
	const char* pInfo;
}Analy_SIB;

typedef struct Analy_Opcode
{
	DWORD Operand[3];
	string Description;
};

typedef struct
{
	TYPE type;
	const char* pInfo[5];
}Analy_Modrm;

typedef struct Instruction
{
	BYTE	RepeatPrefix;	//�ظ�ָ��ǰ׺Repe..
	BYTE	SegmentPrefix;	//��ǰ׺FS,SS..
	BYTE	OperandPrefix;	//��������Сǰ׺0x66��16->32��32->16
	BYTE	AddressPrefix;	//��ַ��Сǰ׺0x67��16->32��32->16

	BYTE	Opcode[3];		//���3��Opcode

	BYTE	ModR_M;			//ModR/M
	BYTE	SIB;			//SIB

	Analy_Opcode GroupOpcode;
	string  instruct;
	string  operand[3];

	string	DisassemblerStr;			//�����ָ����ı���ʽ
	string	OpcodeStr;					//�����ָ���OPCODE��ʽ
	DWORD  Address;
	DWORD	DisassemblerLen;			//�����OPCODE�ĳ���
}Instruction, *PInstruction;


class XqDisassemblerEngine
{
private:
	vector<Instruction> m_VecInstr;
	DWORD m_OpIndex;
	PBYTE m_pOpcode;
	DWORD m_StartIndex;

public:
	DWORD XqDisassembler_Start(IN BYTE *pOpcode,IN const DWORD DisLen,IN const DWORD ImageBase = 0);
	VOID ShowDissembler_Str();

private:
	VOID DecodeOneByte(IN DWORD Mark, OUT PInstruction pInstr);
	VOID DecodeImm8(IN DWORD Mark,OUT PInstruction pInstr);
	VOID DecodeImm16(IN DWORD Mark, OUT PInstruction pInstr);
	VOID DecodeImm66(IN DWORD Mark, OUT PInstruction pInstr);
	VOID DecodeFPU(IN DWORD Mark, IN BYTE mod, OUT PInstruction pInstr);
	VOID DecodeModRM(IN DWORD Mark,IN BYTE MorRM,OUT PInstruction pInstr);
	VOID DecodeSIB(IN BYTE mod,IN DWORD i,OUT PInstruction pInstr);
	VOID DecodeDISPx(IN _MODR_M m,IN Analy_Modrm ModAanlyE, IN int i, OUT PInstruction pInstr,BYTE size);
	VOID DecodeGroup(IN DWORD &Mark,IN DWORD opcode,IN _MODR_M m,OUT PInstruction pInstr);
	VOID DecodeAddr67(IN DWORD Mark, OUT PInstruction pInstr);
	VOID DecodeRegX(PInstruction pInstr, int i, DWORD op);
	VOID Decode2Bytes(IN DWORD &Mark,OUT PInstruction pInstr);
	string GetPrefixString(PInstruction pInstr);
	VOID ComposeInstr(OUT PInstruction pInstr);
};

enum
{
	ES=1,
	CS,
	SS,
	DS,
	FS,
	GS,
	OperandSize66,
	AddressSize67,
	LOCK,
	REPNE,
	REPE
};


#define MODRM	0x80000000
#define IMM8	0x40000000
#define IMM66	0x20000000
#define PREFIX	0x10000000
#define IMM16	0x8000000
#define ONEBYTE	0x4000000
#define ADDR67	0x2000000
#define GROUP	0x1000000
#define MXX		0x800000		//��ModRM��M����
#define IMMX	(IMM8|IMM16|IMM66)
#define StringInstruction	0x400000		//ָ���ظ�ǰ׺0xF2 0xF3(REPNE REP/REPE)��Opcode1����ֻ��������7���ַ���ָ����ϣ�
											// 		0xA4: 0xA5:		MOVS
											// 		0xA6: 0xA7:		CMPS
											// 		0xAE: 0xAF:		SCAS
											// 		0xAC: 0xAD:		LODS
											// 		0xAA: 0xAB:		STOS
											// 		0x6C: 0x6D:		INS
											// 		0x6E: 0x6F:		OUTS
#define FPU		0x200000		//D8��ʼ��FPU_CODE

#define TWOBYTE 0x100000
#define RESERVED			0x00000400		//����
#define MustHave66			0x00000800		//������0x66ǰ׺,ֻ��opcode38/3A������������ָ��
#define MustHaveF2			0x00001000		//Ŀǰֻ��һ��ָ���Ǳ�����0xF2ǰ׺�ģ�0FF0
#define MustHavePrefix		0x00002000		//������ǰ׺
#define MustNo66			0x00004000		//����û��0x66ǰ׺,��ɨ��ָ��ʱ����66��ȡ��־ָ��Ƿ��д˱�־������ֱ�ӷ���1��˵����66ǰ׺�Ƕ����
#define MustNoF2			0x00008000		//����ͬ��
#define MustNoF3			0x00010000		//����ͬ��
#define ThreeOpCode0F38		0x00000080		//0x0F38��3��opcode
#define ThreeOpCode0F3A		0x00000100		//0x0F3A��3��opcode
#define Uxx					0x00040000		//rm����ѰXMM��ֻ����mod==11ʱ�ſ��Խ��룬���ܵ�opcode: 66 0F 50/C5/D7/F7	F2 OF D6
#define Nxx					0x00080000		//rm����ѰMMX��ֻ����mod==11ʱ�ſ��Խ��룬���ܵ�opcode: OF C5/D7/F7			F3 OF D6


#define Fw		(E+FPU+S_W)
#define Fd		(E+FPU+S_D)
#define Fq		(E+FPU+S_Q)
#define Ft		(E+FPU+S_T)
#define F28		(E+FPU+S_E)			//m14/m28
#define F98		(E+FPU)
#define Freg	(FPU+REG)			//st(x)

//��ҪModR/M����
#define	NONE	0x0

#define S		0x100 
#define Sw		(S+S_W)

#define OX		0x200
#define Ob		(OX+S_B)
#define Ov		(OX+S_V)

#define E		0x0400 
#define Ev		(E+S_V)
#define Eb		(E+S_B)
#define Ew		(E+S_W)

#define G		0x0800 
#define Gv		(G+S_V)
#define Gb		(G+S_B)
#define Gw		(G+S_W)

#define Ib		(IMM8+S_B) 
#define Iw		(IMM16+S_W) 
#define Iz		(IMM66+S_D) 

#define Jxx		0x1000	
#define Jz		(Iz + Jxx)
#define Jb		(Ib + Jxx)

//�Ĵ�������
#define EXX		0x80
#define XX		0x40
#define XL		0x20
#define REG		(EXX+XX+XL)

#define EAX		(EXX+R_A)	
#define EBX		(EXX+R_B)
#define ECX		(EXX+R_C)
#define	EDX		(EXX+R_D)
#define ESP		(EXX+R_SP)
#define EBP		(EXX+R_BP)
#define ESI		(EXX+R_SI)
#define EDI		(EXX+R_DI)

#define AX		(XX+R_A)
#define BX		(XX+R_B)
#define CX		(XX+R_C)
#define	DX		(XX+R_D)
#define SP		(XX+R_SP)
#define BP		(XX+R_BP)
#define SI		(XX+R_SI)
#define DI		(XX+R_DI)

#define AL		(XL+R_A)
#define BL		(XL+R_B)
#define CL		(XL+R_C)
#define	DL		(XL+R_D)

#define THREEBYTE 0x04

enum OP_SIZE
{
	S_B,	//BYTE
	S_W,	//WORD
	S_D,	//DWORD
	S_Q,	//QWORD
	S_T,	//TWORD
	S_E,	//16 BYTES
	S_V		//���Ƿ��и�д��������С��opcode->66��67ǰ׺
};

enum REGISTER
{
	R_A,
	R_B,
	R_C,
	R_D,
	R_SP,
	R_BP,
	R_SI,
	R_DI
};








