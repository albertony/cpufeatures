#pragma once

// Including SDKDDKVer.h defines the highest available Windows platform.
// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.
//#include <SDKDDKVer.h>

// Currently building with Windows 8.1 SDK (and also with the standard platform toolset,
// not the Windows XP compatible variant) which means Windows Vista / Server 2008 is
// minimum possible supported Windows version. Setting _WIN32_WINNT to 0x0600 ensuring
// this (Vista/Server 2008) is our actual minimum.
#include <winsdkver.h>
#define _WIN32_WINNT 0x0600
#include <SDKDDKVer.h>

// For Custom Action project we should also set minimum required MSI version
#ifndef _WIN32_MSI              // Specifies that the minimum required MSI version is MSI 3.1
#define _WIN32_MSI 310          // Change this to the appropriate value to target other versions of MSI.
#endif