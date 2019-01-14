//
// Checking CPU features (extended instruction set support) of the executing processor.
//
// This implementation is detecting only the most relevant features, and from both
// Intel/AMD as well as ARM processors, and can be compiled on Microsoft, Apple and Android.
// On Microsoft it mainly uses the Microsoft-specific __cpuid intrinsic, on other platforms
// it resort to inline assembly code, generating the cpuid instruction, available on x86 and x64,
// that queries the processor for information.
//
// Most of the source code is copied from libsodium (src/libsodium/sodium/runtime.c and
// src/libsodium/include/sodium/private/common.h), just slightly modified to fit my application.
// See: https://github.com/jedisct1/libsodium
// See also: https://docs.microsoft.com/en-us/cpp/intrinsics/cpuid-cpuidex
//
#include "Targetver.h"
#include <stddef.h>
#include <stdint.h>
#include <iostream>

#ifdef HAVE_ANDROID_GETCPUFEATURES
# include <cpu-features.h>
#endif
//#include "private/common.h"
//#include "runtime.h"

#if defined(_MSC_VER) && (defined(_M_X64) || defined(_M_AMD64) || defined(_M_IX86))
# include <intrin.h>
# define HAVE_INTRIN_H    1
# define HAVE_MMINTRIN_H  1
# define HAVE_EMMINTRIN_H 1
# define HAVE_PMMINTRIN_H 1
# define HAVE_TMMINTRIN_H 1
# define HAVE_SMMINTRIN_H 1
# define HAVE_AVXINTRIN_H 1
# if _MSC_VER >= 1600
#  define HAVE_WMMINTRIN_H 1
# endif
//# if _MSC_VER >= 1700 && defined(_M_X64)
# if _MSC_VER >= 1700 // Albertony: Removed condition on _M_X64: When used as a utility for checking CPU features AVX2 features can be tested by a 32-bit build, but when used as part of an application (as in the original libsodium) it makes sense to only check for AVX from a 64-bit build!
#  define HAVE_AVX2INTRIN_H 1
#  define HAVE_RDRAND 1 // Albertony: Added 14.06.2018
# endif
#elif defined(HAVE_INTRIN_H)
# include <intrin.h>
#endif

typedef struct CPUFeatures_ {
    int has_neon; // ARM specific (Advanced SIMD extension for ARM)
    int has_sse2;
    int has_sse3;
    int has_ssse3;
    int has_sse41;
    int has_avx;
    int has_avx2;
    int has_avx512f;
    int has_pclmul;
    int has_aesni;
    int has_rdrand;
} CPUFeatures;


#define CPUID_EBX_AVX2    0x00000020
#define CPUID_EBX_AVX512F 0x00010000

#define CPUID_ECX_SSE3    0x00000001
#define CPUID_ECX_PCLMUL  0x00000002
#define CPUID_ECX_SSSE3   0x00000200
#define CPUID_ECX_SSE41   0x00080000
#define CPUID_ECX_AESNI   0x02000000
#define CPUID_ECX_XSAVE   0x04000000
#define CPUID_ECX_OSXSAVE 0x08000000
#define CPUID_ECX_AVX     0x10000000
#define CPUID_ECX_RDRAND  0x40000000

#define CPUID_EDX_SSE2    0x04000000

#define XCR0_SSE 0x00000002
#define XCR0_AVX 0x00000004

static int _arm_cpu_features(CPUFeatures * const cpu_features)
{
#ifndef __arm__
    cpu_features->has_neon = 0;
    return -1;
#else
# ifdef __APPLE__
#  ifdef __ARM_NEON__
    cpu_features->has_neon = 1;
#  else
    cpu_features->has_neon = 0;
#  endif
# elif defined(HAVE_ANDROID_GETCPUFEATURES) && \
    defined(ANDROID_CPU_ARM_FEATURE_NEON)
    cpu_features->has_neon =
        (android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_NEON) != 0x0;
# else
    cpu_features->has_neon = 0;
# endif
    return 0;
#endif
}

static void _cpuid(unsigned int cpu_info[4U], const unsigned int cpu_info_type)
{
#if defined(_MSC_VER) && \
    (defined(_M_X64) || defined(_M_AMD64) || defined(_M_IX86))
    __cpuid((int *) cpu_info, cpu_info_type);
#elif defined(HAVE_CPUID)
    cpu_info[0] = cpu_info[1] = cpu_info[2] = cpu_info[3] = 0;
# ifdef __i386__
    __asm__ __volatile__(
        "pushfl; pushfl; "
        "popl %0; "
        "movl %0, %1; xorl %2, %0; "
        "pushl %0; "
        "popfl; pushfl; popl %0; popfl"
        : "=&r"(cpu_info[0]), "=&r"(cpu_info[1])
        : "i"(0x200000));
    if (((cpu_info[0] ^ cpu_info[1]) & 0x200000) == 0x0) {
        return; /* LCOV_EXCL_LINE */
    }
# endif
# ifdef __i386__
    __asm__ __volatile__("xchgl %%ebx, %k1; cpuid; xchgl %%ebx, %k1"
                         : "=a"(cpu_info[0]), "=&r"(cpu_info[1]),
                           "=c"(cpu_info[2]), "=d"(cpu_info[3])
                         : "0"(cpu_info_type), "2"(0U));
# elif defined(__x86_64__)
    __asm__ __volatile__("xchgq %%rbx, %q1; cpuid; xchgq %%rbx, %q1"
                         : "=a"(cpu_info[0]), "=&r"(cpu_info[1]),
                           "=c"(cpu_info[2]), "=d"(cpu_info[3])
                         : "0"(cpu_info_type), "2"(0U));
# else
    __asm__ __volatile__("cpuid"
                         : "=a"(cpu_info[0]), "=b"(cpu_info[1]),
                           "=c"(cpu_info[2]), "=d"(cpu_info[3])
                         : "0"(cpu_info_type), "2"(0U));
# endif
#else
    (void) cpu_info_type;
    cpu_info[0] = cpu_info[1] = cpu_info[2] = cpu_info[3] = 0;
#endif
}

static int _intel_cpu_features(CPUFeatures * const cpu_features)
{
    unsigned int cpu_info[4];
    unsigned int id;

    _cpuid(cpu_info, 0x0);
    if ((id = cpu_info[0]) == 0U) {
        return -1; /* LCOV_EXCL_LINE */
    }
    _cpuid(cpu_info, 0x00000001);
#ifdef HAVE_EMMINTRIN_H
    cpu_features->has_sse2 = ((cpu_info[3] & CPUID_EDX_SSE2) != 0x0);
#else
    cpu_features->has_sse2   = 0;
#endif

#ifdef HAVE_PMMINTRIN_H
    cpu_features->has_sse3 = ((cpu_info[2] & CPUID_ECX_SSE3) != 0x0);
#else
    cpu_features->has_sse3   = 0;
#endif

#ifdef HAVE_TMMINTRIN_H
    cpu_features->has_ssse3 = ((cpu_info[2] & CPUID_ECX_SSSE3) != 0x0);
#else
    cpu_features->has_ssse3  = 0;
#endif

#ifdef HAVE_SMMINTRIN_H
    cpu_features->has_sse41 = ((cpu_info[2] & CPUID_ECX_SSE41) != 0x0);
#else
    cpu_features->has_sse41  = 0;
#endif

    cpu_features->has_avx = 0;
#ifdef HAVE_AVXINTRIN_H
    if ((cpu_info[2] & (CPUID_ECX_AVX | CPUID_ECX_XSAVE | CPUID_ECX_OSXSAVE)) ==
        (CPUID_ECX_AVX | CPUID_ECX_XSAVE | CPUID_ECX_OSXSAVE)) {
        uint32_t xcr0 = 0U;
# if defined(HAVE__XGETBV) || \
        (defined(_MSC_VER) && defined(_XCR_XFEATURE_ENABLED_MASK) && _MSC_FULL_VER >= 160040219)
        xcr0 = (uint32_t) _xgetbv(0);
# elif defined(_MSC_VER) && defined(_M_IX86)
        /*
         * Visual Studio documentation states that eax/ecx/edx don't need to
         * be preserved in inline assembly code. But that doesn't seem to
         * always hold true on Visual Studio 2010.
         */
        __asm {
            push eax
            push ecx
            push edx
            xor ecx, ecx
            _asm _emit 0x0f _asm _emit 0x01 _asm _emit 0xd0
            mov xcr0, eax
            pop edx
            pop ecx
            pop eax
        }
# elif defined(HAVE_AVX_ASM)
        __asm__ __volatile__(".byte 0x0f, 0x01, 0xd0" /* XGETBV */
                             : "=a"(xcr0)
                             : "c"((uint32_t) 0U)
                             : "%edx");
# endif
        if ((xcr0 & (XCR0_SSE | XCR0_AVX)) == (XCR0_SSE | XCR0_AVX)) {
            cpu_features->has_avx = 1;
        }
    }
#endif

    cpu_features->has_avx2 = 0;
#ifdef HAVE_AVX2INTRIN_H
    if (cpu_features->has_avx) {
        unsigned int cpu_info7[4];

        _cpuid(cpu_info7, 0x00000007);
        cpu_features->has_avx2 = ((cpu_info7[1] & CPUID_EBX_AVX2) != 0x0);
    }
#endif

    cpu_features->has_avx512f = 0;
#ifdef HAVE_AVX512FINTRIN_H
    if (cpu_features->has_avx2) {
        unsigned int cpu_info7[4];

        _cpuid(cpu_info7, 0x00000007);
        cpu_features->has_avx512f = ((cpu_info7[1] & CPUID_EBX_AVX512F) != 0x0);
    }
#endif

#ifdef HAVE_WMMINTRIN_H
    cpu_features->has_pclmul = ((cpu_info[2] & CPUID_ECX_PCLMUL) != 0x0);
    cpu_features->has_aesni  = ((cpu_info[2] & CPUID_ECX_AESNI) != 0x0);
#else
    cpu_features->has_pclmul = 0;
    cpu_features->has_aesni  = 0;
#endif

#ifdef HAVE_RDRAND
   cpu_features->has_rdrand = ((cpu_info[2] & CPUID_ECX_RDRAND) != 0x0);
#else
    cpu_features->has_rdrand = 0;
#endif

    return 0;
}

static int _get_cpu_features(CPUFeatures * const cpu_features)
{
    int ret = -1;
    ret &= _arm_cpu_features(cpu_features);
    ret &= _intel_cpu_features(cpu_features);

    return ret;
}

static void _print_feature_support(std::wostream& stream, const wchar_t* feature_name, bool is_supported, bool print_if_supported, bool print_if_unsupported, bool print_xml) {
    if ((is_supported && print_if_supported) || (!is_supported && print_if_unsupported)) {
        if (print_xml) {
            stream << L"<feature name=\"" << feature_name << L"\" supported=\"" << (is_supported ? "true" : "false") << "\"/>" << std::endl;
        } else {
            if (print_if_supported && print_if_unsupported) {
                stream << feature_name << (is_supported ? " supported" : " not supported") << std::endl;
            } else {
                stream << feature_name << std::endl;
            }
        }
    }
};

static void _print_cpu_features(std::wostream& stream, CPUFeatures * const cpu_features, bool print_supported, bool print_unsupported, bool print_xml)
{
    _print_feature_support(std::wcout, L"NEON", cpu_features->has_neon, print_supported, print_unsupported, print_xml);
    _print_feature_support(std::wcout, L"SSE2", cpu_features->has_sse2, print_supported, print_unsupported, print_xml);
    _print_feature_support(std::wcout, L"SSE3", cpu_features->has_sse3, print_supported, print_unsupported, print_xml);
    _print_feature_support(std::wcout, L"SSSE3", cpu_features->has_ssse3, print_supported, print_unsupported, print_xml);
    _print_feature_support(std::wcout, L"SSE4.1", cpu_features->has_sse41, print_supported, print_unsupported, print_xml);
    _print_feature_support(std::wcout, L"AVX", cpu_features->has_avx, print_supported, print_unsupported, print_xml);
    _print_feature_support(std::wcout, L"AVX2", cpu_features->has_avx2, print_supported, print_unsupported, print_xml);
    _print_feature_support(std::wcout, L"AVX512F", cpu_features->has_avx512f, print_supported, print_unsupported, print_xml);
    _print_feature_support(std::wcout, L"PCLMUL", cpu_features->has_pclmul, print_supported, print_unsupported, print_xml);
    _print_feature_support(std::wcout, L"AES-NI", cpu_features->has_aesni, print_supported, print_unsupported, print_xml);
	_print_feature_support(std::wcout, L"RDRAND", cpu_features->has_rdrand, print_supported, print_unsupported, print_xml);
}

void print_cpu_features(std::wostream& stream, bool print_supported, bool print_unsupported, bool print_xml)
{
    CPUFeatures cpu_features;
    _get_cpu_features(&cpu_features);
    if (print_xml) {
        stream << L"<cpu>" << std::endl;
        stream << L"<features>" << std::endl;
    }
    _print_cpu_features(std::wcout, &cpu_features, print_supported, print_unsupported, print_xml);
    if (print_xml) {
        stream << L"</features>" << std::endl;
        stream << L"</cpu>" << std::endl;
    }
}