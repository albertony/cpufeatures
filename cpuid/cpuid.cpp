/*

Minimalistic library for exposing the __cpuid/__cpuidex intrinsics, which can be used to check
CPU features (extended instruction set support) of the executing processor.

This library exports two functions, cpuid and cpuidex, which executes the corresponding
intrinsic function. The implementations handle the raw input and output of the intrinsic functions
to support a bit higher level easier to use interface: For instance instead of returning the raw
bit sequences of the four registers, it takes a register number and bit number as arguments and
checks if the bit is set. Return value is a nonzero signed integer if the bit is set, otherwise it is zero.

Exported functions:
  int cpuid(unsigned char function_id, unsigned char register_number, unsigned char bit_number)
  int cpuidex(unsigned char function_id, unsigned char subfunction_id, unsigned char register_number, unsigned char bit_number)

The interface are tried to be as generally simple to use as possible, for instance for
loading the library into a managed environment such as C# and PowerShell using DllImport:
- Exporting the functions using module-definition file instead of dllexport to avoid any kind of
  name mangling.
- Uses stdcall convention, which is the same as Windows API functions use (and exposed as macros
  WINAPI, APIENTRY etc), and
  the default of the DllImport attribute in .NET.
- Uses signed integer as the return type, same as Windows API type BOOL, instead of native bool
  since that is more conformant. For example in .NET the native bool type is 1 byte while in C++ it
  is normally 4 byte (int), which may lead lead to problems if not specifying the marshalling accurately.
- Specifies input arguments, which are all low unsigned integers, as type unsigned char, which is the
  same as Windows API type BYTE and also matches the .NET type byte.

PowerShell example:

	# Make sure DllImport will search for the library in the same directory as the current location in PowerShell
	[IO.Directory]::SetCurrentDirectory((Convert-Path (Get-Location -PSProvider FileSystem)))

	# Import the native functions from the library into the managed environment.
	Add-Type –MemberDefinition @"
	[DllImport("cpuid64.dll")]
	[return: MarshalAs(UnmanagedType.Bool)]
	public static extern bool cpuid(byte function_id, byte register_number, byte bit_number);
	[DllImport("cpuid64.dll")]
	[return: MarshalAs(UnmanagedType.Bool)]
	public static extern bool cpuidex(byte function_id, byte subfunction_id, byte register_number, byte bit_number);
	"@ -Name "cpuidlib" -Namespace ""

	# PowerShell wrapper functions for looking up some of the most relevant CPU features.
	function SupportSSE()
	{
		[cpuidlib]::cpuid(1, 3, 25) # Function id 1 contains bitset with flags for main features, and in fourth register (EDX) bit 25 indicates SSE.
	}
	function SupportSSE2()
	{
		[cpuidlib]::cpuid(1, 3, 26) # Function id 1 contains bitset with flags for main features, and in fourth register (EDX) bit 26 indicates SSE2.
	}
	function SupportSSE3()
	{
		[cpuidlib]::cpuid(1, 2, 0) # Function id 1 contains bitset with flags for main features, and in third register (ECX) bit 0 indicates SSE3.
	}
	function SupportSSSE3()
	{
		[cpuidlib]::cpuid(1, 2, 9) # Function id 1 contains bitset with flags for main features, and in third register (ECX) bit 9 indicates SSSE3.
	}
	function SupportSSE41()
	{
		[cpuidlib]::cpuid(1, 2, 19) # Function id 1 contains bitset with flags for main features, and in third register (ECX) bit 19 indicates SSE4.1.
	}
	function SupportSSE42()
	{
		[cpuidlib]::cpuid(1, 2, 20) # Function id 1 contains bitset with flags for main features, and in third register (ECX) bit 20 indicates SSE4.2.
	}
	function SupportAVX()
	{
		[cpuidlib]::cpuid(1, 2, 28) # Function id 1 contains bitset with flags for main features, and in third register (ECX) bit 28 indicates AVX.
	}
	function SupportAVX2()
	{
		[cpuidlib]::cpuid(7, 1, 5) # Function id 7 contains bitset with flags for extended features, and in second register (EBX) bit 5 indicates AVX2.
	}
	function SupportAVX512()
	{
		[cpuidlib]::cpuid(7, 1, 16) # Function id 7 contains bitset with flags for extended features, and in second register (EBX) bit 16 indicates AVX-512 Foundation - the core extension required by all imiplementations of AVX-512.
	}
	function SupportAES()
	{
		[cpuidlib]::cpuid(1, 2, 25) # Function id 1 contains bitset with flags for main features, and in third register (ECX) bit 25 indicates AES.
	}
*/

#include "Targetver.h"
#include "cpuid.h"
#define STRICT // Enable STRICT Type Checking in Windows headers
#define WIN32_LEAN_AND_MEAN // To speed the build process exclude rarely-used services from Windows headers
#define NOMINMAX // Exclude min/max macros from Windows header
#include <Windows.h>
#include <intrin.h>

static int max_function_id; // The number of the highest valid regular function ID for current CPU
static int max_extended_function_id; // The number of the highest valid extended function ID for the current CPU

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		int cpu_info[4]; // Value of the four registers EAX, EBX, ECX, and EDX, each 32-bit integers
		__cpuid(cpu_info, 0x0); // Request function id 0 to get the number of the highest valid function ID
		max_function_id = cpu_info[0];
		__cpuid(cpu_info, 0x80000000); // Request value at id 0x80000000 to get the highest valid extended function ID
		max_extended_function_id = cpu_info[0];
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
int __stdcall cpuid(int function_id, unsigned char register_number, unsigned char bit_number)
{
	int support = 0;
	if ((function_id > 0 && function_id <= max_function_id) || (function_id < 0 && function_id <= max_extended_function_id)) { // Regular function ids are positive, extended function id have value above INT32_MAX (2147483647) and are therefore represented negative signed integers
		if (register_number < 4) { // Four registers (EAX, EBX, ECX, and EDX), numbered 0,1,2,3.
			int cpu_info[4]; // Value of the four registers EAX, EBX, ECX, and EDX, each 32-bit integers
			__cpuid(cpu_info, function_id);
			support = (cpu_info[register_number] & 1 << bit_number); // Check specified bit in specified register
		}
	}
	return support;
}
int __stdcall cpuidex(int function_id, int subfunction_id, unsigned char register_number, unsigned char bit_number)
{
	int support = 0;
	if ((function_id > 0 && function_id <= max_function_id) || (function_id < 0 && function_id <= max_extended_function_id)) { // Regular function ids are positive, extended function id have value above INT32_MAX (2147483647) and are therefore represented negative signed integers
		if (register_number < 4) { // Four registers (EAX, EBX, ECX, and EDX), numbered 0,1,2,3.
			int cpu_info[4]; // Value of the four registers EAX, EBX, ECX, and EDX, each 32-bit integers
			__cpuidex(cpu_info, function_id, subfunction_id);
			support = (cpu_info[register_number] & 1 << bit_number); // Check specified bit in specified register
		}
	}
	return support;
}
