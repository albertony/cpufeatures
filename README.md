# CPUFeatures

A set of utilities, libraries and code examples for exploring CPU features (extended instruction sets)
supported by the processor it runs on.

The project includes the following sub-projects
* CPUFeatures
* CPUFeaturesCustomAction
* CPUFeaturesLibrary
* cpuid

See also my project [InstructionSets](https://github.com/albertony/instructionsets),
where I have combined the knowledge from this project with an dissasembler, so that
one can list the instruction sets used by a specified executable. It also supports
a complete CPU features mode like in this project.

## CPUFeatures

Command line application for checking which CPU features (extended instruction sets) the
processor it runs on support. In case of AVX features, when using the [default mode](#default-mode),
it also includes additional check for required operating support.

Can run different functions:
Default is to use a cross-platform method showing only the most important features.
By specifying command line argument -microsoft (-m or -ms) it will use a Microsoft-specific
variant instead, which reports a more complete set of features. Then there is
a special variant for showing complete set of AVX features (but nothing else),
triggered with argument -avx (-a). This is also a Microsoft-specific variant.

By default all known features are listed and marked as supported or unsupported
but can instead list only the supported or unsupported by specifying either
argument -supported (-s) or -unsupported (-u). Optionally the result can be
presented as XML instead of the default human readable format, by specifying
argument -xml (-x). This can be useful when called from for example PowerShell.

The executable has name suffix 32 or 64 according to platform architecture,
and debug builds have additional suffix d.

### Usage

```
CPUFeatures[32|64][d] [-help|-h|-?]
CPUFeatures[32|64][d] [[-microsoft|-ms|-m]|[-avx|-a]] [[-supported|-s]|[-unsupported|-u]] [-xml|-x]
```

### Default mode

The default mode is detecting only the most relevant features, and from both
Intel/AMD as well as ARM processors, and can (in theory) be compiled on Microsoft, Apple and Android.
On Microsoft it mainly uses the Microsoft-specific __cpuid intrinsic, on other platforms
it resort to inline assembly code, generating the cpuid instruction, available on x86 and x64,
that queries the processor for information.

**Note:** The returned status of AVX features in this mode does not only indicate that it is supported
in hardware by the processor; it tells if it is supported by the processor _and_ supported by the
operating system, _and_ that it is currently enabled in the operating system! It basically tells
you if AVX instructions actually can be used or not, on the current system.
See the following [Stack Overflow answer](https://stackoverflow.com/questions/44144763/avx-feature-detection-using-sigill-versus-cpu-probing/44157138#44157138) for a description of the
algorithm used.
This is different from the [Microsoft mode](#microsoft-mode), where only processor hardware
support is reported.

Most of the source code is copied from libsodium (src/libsodium/sodium/runtime.c and
src/libsodium/include/sodium/private/common.h), just slightly modified to fit my
application.

See:
* https://github.com/jedisct1/libsodium

See also:
* https://docs.microsoft.com/en-us/cpp/intrinsics/cpuid-cpuidex

### Microsoft mode

Using the Microsoft-specific __cpuid intrinsic, which generates the cpuid instruction,
available on x86 and x64, that queries the processor for information. The intrinsic
stores the supported features and CPU information returned by the cpuid instruction
in an array of four 32-bit integers that is filled with the values of the the
EAX, EBX, ECX, and EDX registers (in that order). The information returned has a
different meaning depending on the value passed as the function_id parameter. The
information returned with various values of function_id is processor-dependent.

This implementation is detecting most features, from Intel and AMD processors,
but can only be compiled by Microsoft (Visual C++) compiler.

Most of the source code is copied from the Microsoft Docs article about the __cpuid/__cpuidex
intrinsic, just slightly modified to fit my application.

See:
* https://docs.microsoft.com/en-us/cpp/intrinsics/cpuid-cpuidex
* https://en.wikipedia.org/wiki/CPUID
* https://software.intel.com/sites/default/files/managed/c5/15/architecture-instruction-set-extensions-programming-reference.pdf


### AVX mode

Checking details of Advanced Vector Extensions (AVX) support of the executing processor.
Using the Microsoft-specific __cpuid intrinsic, which generates the cpuid instruction,
available on x86 and x64, that queries the processor for information. The intrinsic
stores the supported features and CPU information returned by the cpuid instruction
in an array of four 32-bit integers that is filled with the values of the the
EAX, EBX, ECX, and EDX registers (in that order). The information returned has a
different meaning depending on the value passed as the function_id parameter. The
information returned with various values of function_id is processor-dependent.

This implementation is detecting all AVX-related features, from Intel and AMD processors,
but can only be compiled by Microsoft (Visual C++) compiler.

Based on source code from the Microsoft Docs article about the __cpuid/__cpuidex
intrinsic, with information about newer AVX features from Wikipedia article about CPUID.

See:
* https://docs.microsoft.com/en-us/cpp/intrinsics/cpuid-cpuidex
* https://en.wikipedia.org/wiki/CPUID
* https://software.intel.com/sites/default/files/managed/c5/15/architecture-instruction-set-extensions-programming-reference.pdf

About AVX:

```
Advanced Vector Extensions (AVX)
  AVX is the original extension, introduced in the Sandy Bridge architecture in 2008.
  AVX2 expands AVX and introduces Fused Multiply-Accumulate (FMA) operations. Introduced in the Haswell architecture in 2013.
  AVX-512 expands to 512-bit support utilizing a new EVEX prefix encoding proposed by Intel in July 2013. Introduced with the Knights Landing architecture in 2015.

AVX-512 consists of multiple extensions not all meant to be supported by all processors implementing them.
The instruction set consists of the following:
  Part 1 (Introduced in Knights Landing):
    AVX-512 Foundation (F) – adds several new instructions and expands most 32-bit and 64-bit floating point SSE-SSE4.1 and AVX/AVX2 instructions with EVEX coding scheme to support the 512-bit registers, operation masks, parameter broadcasting, and embedded rounding and exception control.
    AVX-512 Conflict Detection Instructions (CD) – efficient conflict detection to allow more loops to be vectorized, supported by Knights Landing.
    AVX-512 Exponential and Reciprocal Instructions (ER) – exponential and reciprocal operations designed to help implement transcendental operations, supported by Knights Landing.
    AVX-512 Prefetch Instructions (PF) – new prefetch capabilities, supported by Knights Landing.
  Part 2 (Introduced in Skylake X):
    AVX-512 Vector Length Extensions (VL) – extends most AVX-512 operations to also operate on XMM (128-bit) and YMM (256-bit) registers (including XMM16-XMM31 and YMM16-YMM31 in x86-64 mode).
    AVX-512 Byte and Word Instructions (BW) – extends AVX-512 to cover 8-bit and 16-bit integer operations.
    AVX-512 Doubleword and Quadword Instructions (DQ) – enhanced 32-bit and 64-bit integer operations.
 Part 3 (Introduced in Cannonlake):
    AVX-512 Integer Fused Multiply Add (IFMA) - fused multiply add for 512-bit integers.
    AVX-512 Vector Byte Manipulation Instructions (VBMI) adds vector byte permutation instructions which are not present in AVX-512BW.
Part 4 (Introduced with Ice Lake):
    AVX-512 Vector Neural Network Instructions Word variable precision (VNNI) - vector instructions for deep learning.
    AVX-512 Vector Bit Manipulation Instructions 2 (VBMI2)
    AVX-512 Vector Population Count D/Q (POPCNTDQ)
    AVX-512 BITALG (BITALG)
Part 5 (Introduced in Knights Mill):
    AVX-512 Vector Neural Network Instructions Word variable precision (4VNNIW) - vector instructions for deep learning, enhanced word, variable precision.
    AVX-512 Fused Multiply Accumulation Packed Single precision (4FMAPS) - vector instructions for deep learning, floating point, single precision.
Only the core extension AVX-512F (AVX-512 Foundation) is required by all implementations.
For example desktop processors will additionally support CD, VL, and BW/DQ, while computing coprocessors will support CD, ER and PF.

```

## CPUFeaturesCustomAction

Windows installer custom action library for checking CPU features (extended instruction
set support) of the executing processor. Based on the [Microsoft mode](#microsoft-mode)
described above. Contains individual methods each checking a single specific CPU feature.

The following are the exposed functions:

```
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
```

Example usage:

Build this project, put the release version of desired platform (e.g. CPUFeaturesCustomAction64.dll) into the
WiX project directory. Add the following element to include the library into the installer:

```
<Binary Id="CPUFeaturesCustomActionDll" SourceFile=".\CPUFeaturesCustomAction64.dll" />
```

Then add custom actions such as the following, to just abort install if return value indicates feature is missing:

```
<CustomAction Id="CustomActionSupportSSE3" BinaryKey="CPUFeaturesCustomActionDll" DllEntry="SupportSSE3"/>
<InstallExecuteSequence>
    <Custom Action="CustomActionSupportSSE3" After="AppSearch" />
</InstallExecuteSequence>
```

If your installer has a user interface you probably also want it in the UI sequence to fail early:

```
<InstallUISequence>
    <Custom Action="CustomActionSupportSSE3" After="AppSearch" />
</InstallUISequence>
```

Alternatively, if you want to provide a custom error message, instead of just aborting the install with some error code
buried in the MSI log, you can instead specify the return value from the custom action to be ignored and then instead
check the properties set by the custom action using a condition element. Note: Remember to schedule the custom actions
before the LaunchConditions action, where Condition elements under the Product element will be scheduled, for example
before or after AppSearch (which is normally the action executed just before LaunchConditions). Remember also to
schedule it in the UI sequence (not only execute sequence) since the condition checking the property will be evaluated
as part of the LaunchConditions action and then it will always fail if our custom action that are supposed to set the
property value has not been executed yet.

```
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
```

## cpuid

Minimalistic library for exposing the __cpuid/__cpuidex intrinsics, which can be used to
check CPU features (extended instruction set support) of the executing processor.
Based on the [Microsoft mode](#microsoft-mode) described above.

This library exports two functions, cpuid and cpuidex, which executes the corresponding
intrinsic function. The implementations handle the raw input and output of the intrinsic
functions to support a bit higher level easier to use interface: For instance instead of
returning the raw bit sequences of the four registers, it takes a register number and bit
number as arguments and checks if the bit is set. Return value is a nonzero signed integer
if the bit is set, otherwise it is zero.

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

**Example usage from PowerShell:**

```
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
```