//
// Library exposing simple functions for checking a single CPU feature (extended instruction set support) of the executing processor.
//
// Functions:
//    bool SupportSSE()
//    bool SupportSSE2()
//    bool SupportSSE3()
//    bool SupportSSSE3()
//    bool SupportSSE41()
//    bool SupportSSE42()
//    bool SupportAVX()
//    bool SupportAVX2()
//    bool SupportAVX512()
//    bool SupportAES()
//
#include "Targetver.h"
#include "CPUFeaturesLibrary.h"
#include <intrin.h>

static bool CheckFeature(int functionId, unsigned char registerNumber, unsigned char bitNumber)
{
	bool support = false;
	int cpu_info[4]; // Value of the four registers EAX, EBX, ECX, and EDX, each 32-bit integers
	if (functionId > 0) { // Regular function id have positive value
		__cpuid(cpu_info, 0x0); // Request value at id 0 to get the highest valid function ID
		if (functionId <= cpu_info[0]) {
			__cpuid(cpu_info, functionId);
			support = (cpu_info[registerNumber] & 1 << bitNumber) != 0; // Check specified bit in specified register
		}
	} else if (functionId < 0) { // Extended function id have unsigned value above INT32_MAX (2147483647), so are represented by a negative signed integers
		__cpuid(cpu_info, 0x80000000); // Request value at id 0x80000000 to get the highest valid extended function ID
		if (functionId <= cpu_info[0]) {
			__cpuid(cpu_info, functionId);
			support = (cpu_info[registerNumber] & 1 << bitNumber) != 0; // Check specified bit in specified register
		}
	}
	return support;
}
bool SupportLongMode()
{
	return CheckFeature(0x80000001, 3, 29); // Function id 0x80000001 contains bitset with flags for extended processor info and features, and in fourth register (EDX) bit 29 indicates Long Mode (64-bit x86-64/AMD64 processor).
}
bool SupportSSE()
{
	return CheckFeature(1, 3, 25); // Function id 1 contains bitset with flags for main features, and in fourth register (EDX) bit 25 indicates SSE.
}
bool SupportSSE2()
{
	return CheckFeature(1, 3, 26); // Function id 1 contains bitset with flags for main features, and in fourth register (EDX) bit 26 indicates SSE2.
}
bool SupportSSE3()
{
	return CheckFeature(1, 2, 0); // Function id 1 contains bitset with flags for main features, and in third register (ECX) bit 0 indicates SSE3.
}
bool SupportSSSE3()
{
	return CheckFeature(1, 2, 9); // Function id 1 contains bitset with flags for main features, and in third register (ECX) bit 9 indicates SSSE3.
}
bool SupportSSE41()
{
	return CheckFeature(1, 2, 19); // Function id 1 contains bitset with flags for main features, and in third register (ECX) bit 19 indicates SSE4.1.
}
bool SupportSSE42()
{
	return CheckFeature(1, 2, 20); // Function id 1 contains bitset with flags for main features, and in third register (ECX) bit 20 indicates SSE4.2.
}
bool SupportAVX()
{
	return CheckFeature(1, 2, 28); // Function id 1 contains bitset with flags for main features, and in third register (ECX) bit 28 indicates AVX.
}
bool SupportAVX2()
{
	return CheckFeature(7, 1, 5); // Function id 7 contains bitset with flags for extended features, and in second register (EBX) bit 5 indicates AVX2.
}
bool SupportAVX512()
{
	return CheckFeature(7, 1, 16); // Function id 7 contains bitset with flags for extended features, and in second register (EBX) bit 16 indicates AVX-512 Foundation - the core extension required by all imiplementations of AVX-512.
}
bool SupportAES()
{
	return CheckFeature(1, 2, 25); // Function id 1 contains bitset with flags for main features, and in third register (ECX) bit 25 indicates AES.
}
bool SupportRDRND()
{
	return CheckFeature(1, 2, 30); // Function id 1 contains bitset with flags for main features, and in third register (ECX) bit 30 indicates RDRAND.
}
bool SupportVMX()
{
	return CheckFeature(1, 2, 25); // Function id 1 contains bitset with flags for main features, and in third register (ECX) bit 5 indicates Virtual Machine eXtensions (Intel VT-x and AMD-V).
}