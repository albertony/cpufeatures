//
// Checking details of Advanced Vector Extensions (AVX) support of the executing processor.
// Using the Microsoft-specific __cpuid intrinsic, which generates the cpuid instruction,
// available on x86 and x64, that queries the processor for information. The intrinsic
// stores the supported features and CPU information returned by the cpuid instruction
// in an array of four 32-bit integers that is filled with the values of the the
// EAX, EBX, ECX, and EDX registers (in that order). The information returned has a
// different meaning depending on the value passed as the function_id parameter. The
// information returned with various values of function_id is processor-dependent.
//
// This implementation is detecting all AVX-related features, from Intel and AMD processors,
// but can only be compiled by Microsoft (Visual C++) compiler.
//
// Based on source code from the Microsoft Docs article about the __cpuid/__cpuidex
// intrinsic, with information about newer AVX features from Wikipedia article about CPUID.
// See: https://docs.microsoft.com/en-us/cpp/intrinsics/cpuid-cpuidex
// See also: https://en.wikipedia.org/wiki/CPUID
// See also: https://software.intel.com/sites/default/files/managed/c5/15/architecture-instruction-set-extensions-programming-reference.pdf
//
//
// Advanced Vector Extensions (AVX) 
//   AVX is the original extension, introduced in the Sandy Bridge architecture in 2008.
//   AVX2 expands AVX and introduces Fused Multiply-Accumulate (FMA) operations. Introduced in the Haswell architecture in 2013.
//   AVX-512 expands to 512-bit support utilizing a new EVEX prefix encoding proposed by Intel in July 2013. Introduced with the Knights Landing architecture in 2015.
//
// AVX-512 consists of multiple extensions not all meant to be supported by all processors implementing them.
// The instruction set consists of the following:
//   Part 1 (Introduced in Knights Landing):
//     AVX-512 Foundation (F) – adds several new instructions and expands most 32-bit and 64-bit floating point SSE-SSE4.1 and AVX/AVX2 instructions with EVEX coding scheme to support the 512-bit registers, operation masks, parameter broadcasting, and embedded rounding and exception control.
//     AVX-512 Conflict Detection Instructions (CD) – efficient conflict detection to allow more loops to be vectorized, supported by Knights Landing.
//     AVX-512 Exponential and Reciprocal Instructions (ER) – exponential and reciprocal operations designed to help implement transcendental operations, supported by Knights Landing.
//     AVX-512 Prefetch Instructions (PF) – new prefetch capabilities, supported by Knights Landing.
//   Part 2 (Introduced in Skylake X):
//     AVX-512 Vector Length Extensions (VL) – extends most AVX-512 operations to also operate on XMM (128-bit) and YMM (256-bit) registers (including XMM16-XMM31 and YMM16-YMM31 in x86-64 mode).
//     AVX-512 Byte and Word Instructions (BW) – extends AVX-512 to cover 8-bit and 16-bit integer operations.
//     AVX-512 Doubleword and Quadword Instructions (DQ) – enhanced 32-bit and 64-bit integer operations.
//  Part 3 (Introduced in Cannonlake):
//     AVX-512 Integer Fused Multiply Add (IFMA) - fused multiply add for 512-bit integers.
//     AVX-512 Vector Byte Manipulation Instructions (VBMI) adds vector byte permutation instructions which are not present in AVX-512BW.
// Part 4 (Introduced with Ice Lake):
//     AVX-512 Vector Neural Network Instructions Word variable precision (VNNI) - vector instructions for deep learning.
//     AVX-512 Vector Bit Manipulation Instructions 2 (VBMI2)
//     AVX-512 Vector Population Count D/Q (POPCNTDQ)
//     AVX-512 BITALG (BITALG)
// Part 5 (Introduced in Knights Mill):
//     AVX-512 Vector Neural Network Instructions Word variable precision (4VNNIW) - vector instructions for deep learning, enhanced word, variable precision.
//     AVX-512 Fused Multiply Accumulation Packed Single precision (4FMAPS) - vector instructions for deep learning, floating point, single precision.
// Only the core extension AVX-512F (AVX-512 Foundation) is required by all implementations.
// For example desktop processors will additionally support CD, VL, and BW/DQ, while computing coprocessors will support CD, ER and PF.
//
#include "Targetver.h"
#include <iostream>
#include <intrin.h>

struct AVXFeatures {
	bool avx=false, avx2=false, avx512f=false, avx512pf=false, avx512er=false, avx512cd=false,
		 avx512bw=false, avx512dq=false, avx512vl=false,
		 avx512ifma=false, avx512vbmi=false,
		 avx512vnni=false, avx512vbmi2=false, avx512popcntdq=false, avx512bitalg=false,
		 avx5124vnniw=false, avx5124fmaps=false;
};

static AVXFeatures _get_avx_features()
{
	AVXFeatures features;
	int cpu_info[4]; // Value of the four registers EAX, EBX, ECX, and EDX, each 32-bit integers
	__cpuid(cpu_info, 0x0); // First request function id 0 to get the number of the highest valid function ID
	const int nIds = cpu_info[0];
	if (nIds >= 1) {
		__cpuid(cpu_info, 0x1); // Request function id 1 which contains bitset with flags for main features
		// Detect AVX (introduced in Sandy Bridge)
		features.avx = (cpu_info[2] & 1<<28) != 0; // Bit 28 of the ECX register function indicates AVX
		if (nIds >= 7) {
			__cpuid(cpu_info, 0x7); // Request function id 7 which contains bitset with flags for some extended features
			// Detect AVX-2 (introduced in Haswell)
			features.avx2 = (cpu_info[1] & 1<<5) != 0; // Bit 5 of the EBX register indicates AVX-2
			// Detect AVX-512
			features.avx512f = (cpu_info[1] & 1<<16) != 0; // Bit 16 of the EBX register indicates AVX-512-F (AVX-512 Foundation, the core extension required by all imiplementations of AVX-512)
			// Detect AVX-512 feature extension set 1 (Introduced in Knights Landing, together with the main foundation extension)
			features.avx512pf = (cpu_info[1] & 1<<26) != 0; // Bit 26 of the EBX register indicates AVX-512-PF (AVX-512 Prefetch)
			features.avx512er = (cpu_info[1] & 1<<27) != 0; // Bit 27 of the EBX register indicates AVX-512-ER (AVX-512 Exponential and Reciprocal)
			features.avx512cd = (cpu_info[1] & 1<<28) != 0; // Bit 28 of the EBX register indicates AVX-512-CD (AVX-512 Conflict Detection)
			// Detect AVX-512 feature extension set 2 (Introduced in Skylake X)
			features.avx512bw = (cpu_info[1] & 1<<30) != 0; // Bit 30 of the EBX register indicates AVX-512-BW (AVX-512 Byte and Word)
			features.avx512dq = (cpu_info[1] & 1<<17) != 0; // Bit 17 of the EBX register indicates AVX-512-DQ (AVX-512 Doubleword and Quadword)
			features.avx512vl = (cpu_info[1] & 1<<31) != 0; // Bit 31 of the EBX register indicates AVX-512-VL (AVX-512 Vector Length)
			// Detect AVX-512 feature extension set 3 (Introduced with Cannonlake)
			features.avx512ifma = (cpu_info[1] & 1<<21) != 0; // Bit 21 of the EBX register indicates AVX-512-IFMA (AVX-512 Integer Fused Multiply-Add)
			features.avx512vbmi = (cpu_info[2] & 1<<1) != 0; // Bit 1 of the ECX register indicates AVX-512-VBMI (AVX-512 Vector Byte Manipulation)
			// Detect AVX-512 feature extension set 4 (Introduced with Ice Lake)
			features.avx512vnni = (cpu_info[2] & 1<<11) != 0; // Bit 11 of the ECX register indicates AVX-512-VNNI (AVX-512 Vector Neural Network Instructions)
			features.avx512vbmi2 = (cpu_info[2] & 1<<6) != 0; // Bit 6 of the ECX register indicates AVX-512-VBMI2 (AVX-512 Vector Bit Manipulation Instructions 2)
			features.avx512popcntdq = (cpu_info[2] & 1<<14) != 0; // Bit 14 of the ECX register indicates AVX-512-POPCNTDQ (AVX-512 Vector Population Count D/Q)
			features.avx512bitalg = (cpu_info[2] & 1<<12) != 0; // Bit 12 of the ECX register indicates AVX-512-BITALG (AVX-512 BITALG)
			// Detect AVX-512 feature extension set 5 (Introduced with Knights Mill)
			features.avx5124vnniw = (cpu_info[3] & 1<<2) != 0; // Bit 2 of the EDX register indicates AVX-512-4VNNIW (AVX-512 4-register Vector Neural Network Instructions Word variable precision)
			features.avx5124fmaps = (cpu_info[3] & 1<<3) != 0; // Bit 3 of the EDX register indicates AVX-512-4FMAPS (AVX-512 4-register Fused Multiply Accumulation Packed Single precision)
		}
	}
	return features;
}

static void _print_feature_support(std::wostream& stream, const wchar_t* feature_name, bool is_supported, bool print_if_supported, bool print_if_unsupported, bool print_xml) {
    if ((is_supported && print_if_supported) || (!is_supported && print_if_unsupported)) {
        if (print_xml) {
            stream << L"<feature name=\"" << feature_name << L"\" supported=\"" << (is_supported ? "true" : "false") << "\"/>" << std::endl;
        } else {
            stream << feature_name << (is_supported ? " supported" : " not supported") << std::endl;
        }
    }
};

static void _print_avx_features(std::wostream& stream, const AVXFeatures& features, bool print_supported, bool print_unsupported, bool print_xml)
{
	_print_feature_support(std::wcout, L"AVX", features.avx, print_supported, print_unsupported, print_xml);
	_print_feature_support(std::wcout, L"AVX2", features.avx2, print_supported, print_unsupported, print_xml);
	_print_feature_support(std::wcout, L"AVX-512 (F)", features.avx512f, print_supported, print_unsupported, print_xml);
	_print_feature_support(std::wcout, L"AVX-512 CD", features.avx512cd, print_supported, print_unsupported, print_xml);
	_print_feature_support(std::wcout, L"AVX-512 ER", features.avx512er, print_supported, print_unsupported, print_xml);
	_print_feature_support(std::wcout, L"AVX-512 PF", features.avx512pf, print_supported, print_unsupported, print_xml);
	_print_feature_support(std::wcout, L"AVX-512 VL", features.avx512vl, print_supported, print_unsupported, print_xml);
	_print_feature_support(std::wcout, L"AVX-512 BW", features.avx512bw, print_supported, print_unsupported, print_xml);
	_print_feature_support(std::wcout, L"AVX-512 DQ", features.avx512dq, print_supported, print_unsupported, print_xml);
	_print_feature_support(std::wcout, L"AVX-512 IFMA", features.avx512ifma, print_supported, print_unsupported, print_xml);
	_print_feature_support(std::wcout, L"AVX-512 VBMI", features.avx512vbmi, print_supported, print_unsupported, print_xml);
	_print_feature_support(std::wcout, L"AVX-512 VNNI", features.avx512vnni, print_supported, print_unsupported, print_xml);
	_print_feature_support(std::wcout, L"AVX-512 VBMI2", features.avx512vbmi2, print_supported, print_unsupported, print_xml);
	_print_feature_support(std::wcout, L"AVX-512 POPCNTDQ", features.avx512popcntdq, print_supported, print_unsupported, print_xml);
	_print_feature_support(std::wcout, L"AVX-512 BITALG", features.avx512bitalg, print_supported, print_unsupported, print_xml);
	_print_feature_support(std::wcout, L"AVX-512 4VNNIW", features.avx5124vnniw, print_supported, print_unsupported, print_xml);
	_print_feature_support(std::wcout, L"AVX-512 4FMAPS", features.avx5124fmaps, print_supported, print_unsupported, print_xml);
}

void print_avx_features(std::wostream& stream, bool print_supported, bool print_unsupported, bool print_xml)
{
	AVXFeatures features = _get_avx_features();
	if (print_xml) {
		stream << L"<cpu>" << std::endl;
		stream << L"<features>" << std::endl;
	}
	_print_avx_features(std::wcout, features, print_supported, print_unsupported, print_xml);
	if (print_xml) {
		stream << L"</features>" << std::endl;
		stream << L"</cpu>" << std::endl;
	}

}