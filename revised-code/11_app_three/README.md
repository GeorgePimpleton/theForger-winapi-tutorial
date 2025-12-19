# theForger's Win32 API tutorial, revised and updated code
[![Language](https://img.shields.io/badge/Language%20-C-blue.svg)](https://github.com/GeorgePimpleton/theForger-winapi-tutorial/)
[![Platform](https://img.shields.io/badge/Platform%20-Win32-blue.svg)](https://github.com/GeorgePimpleton/theForger-winapi-tutorial/)

### 11_app_three
Text files in Windows can be a tricky thing, I know of no reasonable way to make a generic app that can agnostically edit windows and load/save text as ANSI or Unicode.  You have to specify using the ANSI or Unicode versions of the Desktop WinAPI functions.  ~~For this reason there is no "minimal" rewrite version of this particular app, only the "modern" rewrite.~~

####Notes

The Common Controls to the rescue!   Using them instead of the older controls works!

A caveate, though.  The minimal example must not be compiled using Unicode, it must use Multibyte.  Otherwise any text imported looks quite odd.
