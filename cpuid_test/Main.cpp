#include "Targetver.h"
#include "../cpuid/cpuid.h"
#include <iostream>

bool SupportLongMode()
{
	return cpuid(0x80000001, 3, 29); // Function id 0x80000001 contains bitset with flags for extended processor info and features, and in fourth register (EDX) bit 29 indicates Long Mode (64-bit x86-64/AMD64 processor).
}
bool SupportSSE()
{
	return cpuid(1, 3, 25); // Function id 1 contains bitset with flags for main features, and in fourth register (EDX) bit 25 indicates SSE.
}
bool SupportSSE2()
{
	return cpuid(1, 3, 26); // Function id 1 contains bitset with flags for main features, and in fourth register (EDX) bit 26 indicates SSE2.
}
bool SupportSSE3()
{
	return cpuid(1, 2, 0); // Function id 1 contains bitset with flags for main features, and in third register (ECX) bit 0 indicates SSE3.
}
bool SupportSSSE3()
{
	return cpuid(1, 2, 9); // Function id 1 contains bitset with flags for main features, and in third register (ECX) bit 9 indicates SSSE3.
}
bool SupportSSE41()
{
	return cpuid(1, 2, 19); // Function id 1 contains bitset with flags for main features, and in third register (ECX) bit 19 indicates SSE4.1.
}
bool SupportSSE42()
{
	return cpuid(1, 2, 20); // Function id 1 contains bitset with flags for main features, and in third register (ECX) bit 20 indicates SSE4.2.
}
bool SupportAVX()
{
	return cpuid(1, 2, 28); // Function id 1 contains bitset with flags for main features, and in third register (ECX) bit 28 indicates AVX.
}
bool SupportAVX2()
{
	return cpuid(7, 1, 5); // Function id 7 contains bitset with flags for extended features, and in second register (EBX) bit 5 indicates AVX2.
}
bool SupportAVX512()
{
	return cpuid(7, 1, 16); // Function id 7 contains bitset with flags for extended features, and in second register (EBX) bit 16 indicates AVX-512 Foundation - the core extension required by all imiplementations of AVX-512.
}
bool SupportAES()
{
	return cpuid(1, 2, 25); // Function id 1 contains bitset with flags for main features, and in third register (ECX) bit 25 indicates AES.
}

int wmain(int argc, wchar_t *argv[], wchar_t *envp[])
{
	std::wcout << L"LongMode (64-bit) " << (SupportLongMode() ? L"supported" : L"not supported") << std::endl;
	std::wcout << L"SSE " << (SupportSSE() ? L"supported" : L"not supported") << std::endl;
	std::wcout << L"SSE2 " << (SupportSSE2() ? L"supported" : L"not supported") << std::endl;
	std::wcout << L"SSE3 " << (SupportSSE3() ? L"supported" : L"not supported") << std::endl;
	std::wcout << L"SSSE3 " << (SupportSSSE3() ? L"supported" : L"not supported") << std::endl;
	std::wcout << L"SSE41 " << (SupportSSE41() ? L"supported" : L"not supported") << std::endl;
	std::wcout << L"SSE42 " << (SupportSSE42() ? L"supported" : L"not supported") << std::endl;
	std::wcout << L"AVX " << (SupportAVX() ? L"supported" : L"not supported") << std::endl;
	std::wcout << L"AVX2 " << (SupportAVX2() ? L"supported" : L"not supported") << std::endl;
	std::wcout << L"AVX512 " << (SupportAVX512() ? L"supported" : L"not supported") << std::endl;
	std::wcout << L"AES " << (SupportAES() ? L"supported" : L"not supported") << std::endl;
}