#include "Targetver.h"
#include "../CPUFeaturesLibrary/CPUFeaturesLibrary.h"
#include <iostream>

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
	std::wcout << L"RDRND " << (SupportRDRND() ? L"supported" : L"not supported") << std::endl;
}