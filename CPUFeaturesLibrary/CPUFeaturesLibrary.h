#pragma once

#ifdef CPUFEATURESLIBRARY_EXPORTS
	//Exporting using module-definition file instead of dllexport, to avoid any name mangling.
	//#define LIBRARY_API __declspec(dllexport)
	#define LIBRARY_API
#else
	#define LIBRARY_API __declspec(dllimport)
#endif

LIBRARY_API bool SupportLongMode();
LIBRARY_API bool SupportSSE();
LIBRARY_API bool SupportSSE2();
LIBRARY_API bool SupportSSE3();
LIBRARY_API bool SupportSSSE3();
LIBRARY_API bool SupportSSE41();
LIBRARY_API bool SupportSSE42();
LIBRARY_API bool SupportAVX();
LIBRARY_API bool SupportAVX2();
LIBRARY_API bool SupportAVX512();
LIBRARY_API bool SupportAES();
LIBRARY_API bool SupportRDRND();
