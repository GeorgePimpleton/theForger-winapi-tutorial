# theForger's Win32 API tutorial, revised and updated code
[![Language](https://img.shields.io/badge/Language%20-C-blue.svg)](https://github.com/GeorgePimpleton/theForger-winapi-tutorial/)
[![Platform](https://img.shields.io/badge/Platform%20-Win32-blue.svg)](https://github.com/GeorgePimpleton/theForger-winapi-tutorial/)

### 11_app_three
Text files in Windows can be a tricky thing, there is no reasonable way to make a generic app that can agnostically edit windows and load/save text as ANSI or Unicode.  You have to specify using the ANSI or Unicode versions of the Desktop WinAPI functions.  For this reason there is no "minimal" rewrite version of this particular app, only the "modern" rewrite.

Yes, it is possible to compile the original code as written by manually changing the Visual Studio defaults the character set used in the project's settings.  This could be done for all theForger code examples.  I personally choose to not do so.  Microsoft highly recommends writing new Windows code to be Unicode as much as possible.
