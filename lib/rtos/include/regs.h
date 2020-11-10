//******************************************************************************
// Copyright (c) 2018, The Regents of the University of California (Regents).
// All Rights Reserved. See LICENSE for license details.
//------------------------------------------------------------------------------

#ifndef _REGS_H_
#define _REGS_H_
#include <stdint.h>

struct regs {
	uintptr_t pc; // use this slot as pc
	uintptr_t ra;
	uintptr_t sp; 
	uintptr_t t0;
	uintptr_t t1;
	uintptr_t t2;
	uintptr_t s0;
	uintptr_t s1;
	uintptr_t a0;
	uintptr_t a1;
	uintptr_t a2;
	uintptr_t a3;
	uintptr_t a4;
	uintptr_t a5;
	uintptr_t a6;
	uintptr_t a7;
	uintptr_t s2;
	uintptr_t s3;
	uintptr_t s4;
	uintptr_t s5;
	uintptr_t s6;
	uintptr_t s7;
	uintptr_t s8;
	uintptr_t s9;
	uintptr_t s10;
	uintptr_t s11;
	uintptr_t t3;
	uintptr_t t4;
	uintptr_t t5;
	uintptr_t t6;
};

#endif /* _REGS_H_ */