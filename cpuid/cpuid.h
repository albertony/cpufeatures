#pragma once

#ifdef CPUID_EXPORTS
//Exporting using module-definition file instead of dllexport, to avoid any name mangling.
//#define LIBRARY_API __declspec(dllexport)
#define LIBRARY_API
#else
#define LIBRARY_API __declspec(dllimport)
#endif

LIBRARY_API int __stdcall cpuid(int function_id, unsigned char register_number, unsigned char bit_number);
LIBRARY_API int __stdcall cpuidex(int function_id, int subfunction_id, unsigned char register_number, unsigned char bit_number);
