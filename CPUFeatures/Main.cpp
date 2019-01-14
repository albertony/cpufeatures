#include "Targetver.h"
#include "Version.h"
#include <iostream>

extern void print_cpu_features_microsoft(std::wostream& stream, bool print_supported, bool print_unsupported, bool print_xml);
extern void print_cpu_features(std::wostream& stream, bool print_supported, bool print_unsupported, bool print_xml);
extern void print_avx_features(std::wostream& stream, bool print_supported, bool print_unsupported, bool print_xml);

bool is_option(const wchar_t* arg)
{
	return (arg[0] == L'-' || arg[0] == L'/') && arg[1] != L'\0';
}
bool match_option_name(const wchar_t* arg, const wchar_t* option_name)
{
	return option_name && _wcsicmp(&arg[1], option_name) == 0;
}
bool match_option(const wchar_t* arg, const wchar_t* option_full = nullptr, const wchar_t* option_short = nullptr, const wchar_t* option_alternative = nullptr)
{
	return is_option(arg) && (match_option_name(arg, option_full) || match_option_name(arg, option_short) || match_option_name(arg, option_alternative));
}

int wmain(int argc, wchar_t *argv[], wchar_t *envp[])
{
	if (argc > 1 && match_option(argv[1], L"help", L"h", L"?")) {
		wchar_t* exeName = wcsrchr(argv[0], L'\\');
		if (exeName)
			++exeName;
		else
			exeName = argv[0];

		std::wcout << std::endl;
		std::wcout << exeName << L" version " << VERSION_STRING << std::endl;
		std::wcout << std::endl;
		std::wcout << L"Application for checking which CPU features (extended instruction sets) the" << std::endl;
		std::wcout << L"processor it runs on support. Can run different functions: Default is to use" << std::endl;
		std::wcout << L"a cross-platform method showing only the most important features. By specifying" << std::endl;
		std::wcout << L"command line argument -microsoft (-m or -ms) it will use a Microsoft-specific" << std::endl;
		std::wcout << L"variant instead, which reports a more complete set of features. Then there is" << std::endl;
		std::wcout << L"a special variant for showing complete set of AVX features (but nothing else)," << std::endl;
		std::wcout << L"triggered with argument -avx (-a). This is also a Microsoft-specific variant." << std::endl;
		std::wcout << L"By default all known features are listed and marked as supported or unsupported" << std::endl;
		std::wcout << L"but can instead list only the supported or unsupported by specifying either" << std::endl;
		std::wcout << L"argument -supported (-s) or -unsupported (-u). Optionally the result can be" << std::endl;
		std::wcout << L"presented as XML instead of the default human readable format, by specifying" << std::endl;
		std::wcout << L"argument -xml (-x). This can be useful when called from for example PowerShell." << std::endl;
		std::wcout << L"has suffix 32 or 64 according to platform architecture, and debug builds have" << std::endl;
		std::wcout << L"additional suffix d." << std::endl;
		std::wcout << std::endl;
		std::wcout << L"Usage:" << std::endl;
		std::wcout << L"  CPUFeatures[32|64][d] [-help|-h|-?]" << std::endl;
		std::wcout << L"  CPUFeatures[32|64][d] [[-microsoft|-ms|-m]|[-avx|-a]] [[-supported|-s]|[-unsupported|-u]] [-xml|-x]" << std::endl;
		return EXIT_SUCCESS;
	}
	enum Method {
		Default, Microsoft, AVX
	};
	Method method = Default;
	bool print_supported = false;
	bool print_unsupported = false;
	bool print_xml = false;
	if (argc > 1) {
		int argi = 1;
		if (match_option(argv[argi], L"microsoft", L"m", L"ms")) {
			method = Microsoft;
			++argi;
		}
		else if (match_option(argv[argi], L"avx", L"a")) {
			method = AVX;
			++argi;
		}
		if (argc > argi && match_option(argv[argi], L"supported", L"s")) {
			print_supported = true;
			++argi;
		}
		if (argc > argi && match_option(argv[argi], L"unsupported", L"u")) {
			print_unsupported = true;
			++argi;
		}
		if (argc > argi && match_option(argv[argi], L"xml", L"x")) {
			print_xml = true;
			++argi;
		}
	}
	if (!print_supported && !print_unsupported)
		print_supported = print_unsupported = true;
	switch (method) {
	case Microsoft:
		print_cpu_features_microsoft(std::wcout, print_supported, print_unsupported, print_xml);
		break;
	case AVX:
		print_avx_features(std::wcout, print_supported, print_unsupported, print_xml);
		break;
	default:
		print_cpu_features(std::wcout, print_supported, print_unsupported, print_xml);
	}
	return EXIT_SUCCESS;
}
