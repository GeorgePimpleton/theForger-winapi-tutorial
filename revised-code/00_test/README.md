# theForger's Win32 API tutorial, revised and updated code
[![Language](https://img.shields.io/badge/Language%20-C-blue.svg)](https://github.com/GeorgePimpleton/theForger-winapi-tutorial/)
[![Platform](https://img.shields.io/badge/Platform%20-Win32-blue.svg)](https://github.com/GeorgePimpleton/theForger-winapi-tutorial/)

### The initial test application created with MFC
I wondered if a simple app that displays just a message box was possible using MFC.  Well, it is.  test_MFC.cpp shows how.

This example shows that MFC is not as feature rich as the Windows Desktop WinAPI in some aspects when creating minimal apps, using Message boxes, for example.  Being unable to easily specify a window title is not good, doing so requires more code.

This minimal MFC example also causes an app crash on exit that goes unnoticed executing the app without debugging.  \*\****OUCH***\*\*

The rest of the code examples (except for the next one) will NOT be redone using MFC.  Doing it yourself would be a learning experience after learning C++ and the WinAPI.
