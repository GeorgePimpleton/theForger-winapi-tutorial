# theForger's Win32 API tutorial
[![Language](https://img.shields.io/badge/Language%20-C-blue.svg)](https://github.com/GeorgePimpleton/theForger-winapi-tutorial/)
[![Platform](https://img.shields.io/badge/Platform%20-Win32-blue.svg)](https://github.com/GeorgePimpleton/theForger-winapi-tutorial/)

An unofficial repo of theForger's Win32 API tutorial as well as an archive of [theForger's Win32 API Programming Tutorial](http://www.winprog.org/tutorial/) site.

### Background
Win9X/Me is officially dead!  Windows API ANSI encoding is dead!  Windows XP and later now use Unicode!

Windows and C/C++ have changed since Win9X/Me was Windows.  Programmers need to adapt.

### Why this repository exists
theForger's Win32 API tutorial is one of the better non-MS affiliated free online resources for learning the basics of Win32 API programming, but the code is IMO hopelessly out-dated.  Written when Win9X was the main OS the Windows API has changed since then.  Windows is no longer a hybrid 16/32 bit hybrid operating system.  The core of the operating system is all 32-bit or 64-bit.  64-bit CPUs are now common and Microsoft has kept up by releasing x64 Windows with a resulting change to the Windows API.

Multi-core CPUs are also common, but the lessons won't be using more than one core.

Rewriting the code to work with modern Windows systems is the goal.  The goal of having theForger's Win32 API sources compile and work on a modern system whether it is x86 or x64.

### Notes
I changed the directory structure to better reflect the tutorial's flow.

I used Visual Studio 2022 Community edition to rework and modernize the code as made sense to me.  The changes aren't the only way to go.

For most of the examples there are two different sets of code.  The first is the bare minimum needed to modify the code to work using the TEXT macro and character data type of TCHAR.  This approach allows flexibility when compiling Unicode or ANSI (wide character set).

The changes for the second examples are (not a complete list):

1. Explictly used the Unicode version of WinAPI functions, objects and data types.  wWinMain vs. WinMain, for example.  WCHAR vs. TCHAR.  And so on.
2. Add/changed some casts so the code would work with both x86 & x64 Windows.
3. Used SAL on wWinMain to stop Visual Studio whinging about "inconsistent annotations".

Compare theForger's orginal code and my versions to see what I consider to be necessary or required modifications.  It isn't the only way to modify the code.

Why don't I simply repo the entire solution instead of just the individual source files?  My layout might not work for you.  I also like being able to compile and test any code for x86 & x64.

The source files don't require using Visual Studio.  As long as you use a Windows compliant compiler the sources should build runnable executables.

### Post-script
The code as written by theForger ***is*** compileable as written as long as the code isn't compiled using the Unicode/wide character set. If someone wanted to compile using the Multi-byte character set the code needs zero modification.  There will be at least two warnings compiling the unmodified code this way, there should be no errors.
