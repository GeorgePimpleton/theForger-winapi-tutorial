# theForger's Win32 API tutorial
--------------------------------

[![Language](https://img.shields.io/badge/Language%20-C-blue.svg)](https://github.com/GeorgePimpleton/theForger-winapi-tutorial/)
[![Platform](https://img.shields.io/badge/Platform%20-Win32-blue.svg)](https://github.com/GeorgePimpleton/theForger-winapi-tutorial/)

An unofficial repo of theForger's Win32 API tutorial as well as an archive of [theForger's Win32 API Programming Tutorial](http://www.winprog.org/tutorial/) site.

### Background

Win9X/Me is officially dead!  Windows API ANSI encoding is dead!  Windows XP and later now use Unicode!

Windows and C/C++ have changed since 2003/2004.  Programmers need to adapt.

### Why this repository exists

theForger's Win32 API tutorial is one of the best resources for learning the basics of Win32 API programming, but the code is out-dated.  Written when Win9X was the main OS for people since 1998 the Windows API has changed.  Windows© is no longer a hybrid 16/32 bit hybrid operating system.  The core of the operating system is all 32-bit.  64-bit CPUs are now common and Microsoft has kept up by releasing a x64 Windows© version.

Rewriting the code to work with modern Windows© systems is the goal.  The goal of having theForger's Win32 API sources compile and work on a modern system whether it is x86 or x64.

### Notes

I used Visual Studio 2022 Community edition.

Why don't I simply repo the entire solution instead of just the individual source files?  My layout might not work for you.  I also like being able to compile and test any code for x86 & x64.

The source files don't require using Visual Studio.  As long as you use a Windows© compliant compiler the sources should build runnable executables.
