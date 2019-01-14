/*

Windows installer custom action library for checking CPU features (extended instruction set support) of the executing processor.
Contains individual methods each checking a single specific CPU feature.

Functions supported:

   SupportSSE
   SupportSSE2
   SupportSSE3
   SupportSSSE3
   SupportSSE41
   SupportSSE42
   SupportAVX
   SupportAVX2
   SupportAVX512
   SupportAES
   SupportRDRND

Example:

Build this project, put the release version of desired platform (e.g. CPUFeaturesCustomAction64.dll) into the
WiX project directory. Add the following element to include the library into the installer:

  <Binary Id="CPUFeaturesCustomActionDll" SourceFile=".\CPUFeaturesCustomAction64.dll" />

Then add custom actions such as the following, to just abort install if return value indicates feature is missing:

  <CustomAction Id="CustomActionSupportSSE3" BinaryKey="CPUFeaturesCustomActionDll" DllEntry="SupportSSE3"/>
  <InstallExecuteSequence>
    <Custom Action="CustomActionSupportSSE3" After="AppSearch" />
  </InstallExecuteSequence>

If your installer has a user interface you probably also want it in the UI sequence to fail early:

  <InstallUISequence>
    <Custom Action="CustomActionSupportSSE3" After="AppSearch" />
  </InstallUISequence>

Alternatively, if you want to provide a custom error message, instead of just aborting the install with some error code
buried in the MSI log, you can instead specify the return value from the custom action to be ignored and then instead
check the properties set by the custom action using a condition element. Note: Remember to schedule the custom actions
before the LaunchConditions action, where Condition elements under the Product element will be scheduled, for example
before or after AppSearch (which is normally the action executed just before LaunchConditions). Remember also to
schedule it in the UI sequence (not only execute sequence) since the condition checking the property will be evaluated
as part of the LaunchConditions action and then it will always fail if our custom action that are supposed to set the
property value has not been executed yet.

  <CustomAction Id="CustomActionSupportAVX" BinaryKey="CPUFeaturesCustomActionDll" DllEntry="SupportAVX" Return="ignore"/>
  <InstallExecuteSequence>
    <Custom Action="CustomActionSupportAVX" After="AppSearch" />
  </InstallExecuteSequence>
  <InstallUISequence>
    <Custom Action="CustomActionSupportAVX" After="AppSearch" />
  </InstallUISequence>

  <CustomAction Id="CustomActionSupportSSE3" BinaryKey="CPUFeaturesCustomActionDll" DllEntry="SupportSSE3" Return="ignore"/>
  <InstallExecuteSequence>
    <Custom Action="CustomActionSupportSSE3" Before="CustomActionSupportAVX" />
  </InstallExecuteSequence>
  <InstallUISequence>
    <Custom Action="CustomActionSupportSSE3" Before="CustomActionSupportAVX" />
  </InstallUISequence>

  <Condition Message="This CPU does not support SSE3">CPUFEATURE_SSE3</Condition>
  <Condition Message="This CPU does not support AVX">CPUFEATURE_AVX</Condition>

*/


#include "Targetver.h"
#define STRICT // Enable STRICT Type Checking in Windows headers
#define WIN32_LEAN_AND_MEAN // To speed the build process exclude rarely-used services from Windows headers
#define NOMINMAX // Exclude min/max macros from Windows header
#include <Windows.h>
#include <Msi.h>
#include <Msiquery.h>
#include <intrin.h>

static bool CheckFeature(int functionId, unsigned char registerNumber, unsigned char bitNumber)
{
	bool support = false;
	int cpu_info[4]; // Value of the four registers EAX, EBX, ECX, and EDX, each 32-bit integers
	if (functionId > 0) { // Regular function id
		__cpuid(cpu_info, 0x0); // Request value at id 0 to get the highest valid function ID
		if (functionId <= cpu_info[0]) {
			__cpuid(cpu_info, functionId);
			support = (cpu_info[registerNumber] & 1 << bitNumber) != 0; // Check specified bit in specified register
		}
	}
	else if (functionId < 0) { // Extended function id have unsigned value above INT32_MAX (2147483647), so are represented by a negative signed integers
		__cpuid(cpu_info, 0x80000000); // Request value at id 0x80000000 to get the highest valid extended function ID
		if (functionId <= cpu_info[0]) {
			__cpuid(cpu_info, functionId);
			support = (cpu_info[registerNumber] & 1 << bitNumber) != 0; // Check specified bit in specified register
		}
	}
	return support;
}

UINT __stdcall SupportSSE(MSIHANDLE hInstall)
{
	if (CheckFeature(1, 3, 25)) { // Function id 1 contains bitset with flags for main features, and in fourth register (EDX) bit 25 indicates SSE.
		MsiSetProperty(hInstall, L"CPUFEATURE_SSE", L"1");
		return ERROR_SUCCESS;
	} else return ERROR_INSTALL_FAILURE;
}
UINT __stdcall SupportSSE2(MSIHANDLE hInstall)
{
	if (CheckFeature(1, 3, 26)) { // Function id 1 contains bitset with flags for main features, and in fourth register (EDX) bit 26 indicates SSE2.
		MsiSetProperty(hInstall, L"CPUFEATURE_SSE2", L"1");
		return ERROR_SUCCESS;
	} else return ERROR_INSTALL_FAILURE;
}
UINT __stdcall SupportSSE3(MSIHANDLE hInstall)
{
	if (CheckFeature(1, 2, 0)) { // Function id 1 contains bitset with flags for main features, and in third register (ECX) bit 0 indicates SSE3.
		MsiSetProperty(hInstall, L"CPUFEATURE_SSE3", L"1");
		return ERROR_SUCCESS;
	} else return ERROR_INSTALL_FAILURE;
}
UINT __stdcall SupportSSSE3(MSIHANDLE hInstall)
{
	if (CheckFeature(1, 2, 9)) { // Function id 1 contains bitset with flags for main features, and in third register (ECX) bit 9 indicates SSSE3.
		MsiSetProperty(hInstall, L"CPUFEATURE_SSSE3", L"1");
		return ERROR_SUCCESS;
	} else return ERROR_INSTALL_FAILURE;
}
UINT __stdcall SupportSSE41(MSIHANDLE hInstall)
{
	if (CheckFeature(1, 2, 19)) { // Function id 1 contains bitset with flags for main features, and in third register (ECX) bit 19 indicates SSE4.1.
		MsiSetProperty(hInstall, L"CPUFEATURE_SSE41", L"1");
		return ERROR_SUCCESS;
	} else return ERROR_INSTALL_FAILURE;
}
UINT __stdcall SupportSSE42(MSIHANDLE hInstall)
{
	if (CheckFeature(1, 2, 20)) { // Function id 1 contains bitset with flags for main features, and in third register (ECX) bit 20 indicates SSE4.2.
		MsiSetProperty(hInstall, L"CPUFEATURE_SSE42", L"1");
		return ERROR_SUCCESS;
	} else return ERROR_INSTALL_FAILURE;
}
UINT __stdcall SupportAVX(MSIHANDLE hInstall)
{
	if (CheckFeature(1, 2, 28)) { // Function id 1 contains bitset with flags for main features, and in third register (ECX) bit 28 indicates AVX.
		MsiSetProperty(hInstall, L"CPUFEATURE_AVX", L"1");
		return ERROR_SUCCESS;
	} else return ERROR_INSTALL_FAILURE;
}
UINT __stdcall SupportAVX2(MSIHANDLE hInstall)
{
	if (CheckFeature(7, 1, 5)) { // Function id 7 contains bitset with flags for extended features, and in second register (EBX) bit 5 indicates AVX2.
		MsiSetProperty(hInstall, L"CPUFEATURE_AVX2", L"1");
		return ERROR_SUCCESS;
	} else return ERROR_INSTALL_FAILURE;
}
UINT __stdcall SupportAVX512(MSIHANDLE hInstall)
{
	if (CheckFeature(7, 1, 16)) { // Function id 7 contains bitset with flags for extended features, and in second register (EBX) bit 16 indicates AVX-512 Foundation - the core extension required by all imiplementations of AVX-512.
		MsiSetProperty(hInstall, L"CPUFEATURE_AVX512", L"1");
		return ERROR_SUCCESS;
	} else return ERROR_INSTALL_FAILURE;
}
UINT __stdcall SupportAES(MSIHANDLE hInstall)
{
	if (CheckFeature(1, 2, 25)) { // Function id 1 contains bitset with flags for main features, and in third register (ECX) bit 25 indicates AES.
		MsiSetProperty(hInstall, L"CPUFEATURE_AES", L"1");
		return ERROR_SUCCESS;
	} else return ERROR_INSTALL_FAILURE;
}
UINT __stdcall SupportRDRND(MSIHANDLE hInstall)
{
	if (CheckFeature(1, 2, 30)) { // Function id 1 contains bitset with flags for main features, and in third register (ECX) bit 30 indicates RDRAND.
		MsiSetProperty(hInstall, L"CPUFEATURE_RDRND", L"1");
		return ERROR_SUCCESS;
	} else return ERROR_INSTALL_FAILURE;
}