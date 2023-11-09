# theForger's Win32 API tutorial, revised and updated code
[![Language](https://img.shields.io/badge/Language%20-C-blue.svg)](https://github.com/GeorgePimpleton/theForger-winapi-tutorial/)
[![Platform](https://img.shields.io/badge/Platform%20-Win32-blue.svg)](https://github.com/GeorgePimpleton/theForger-winapi-tutorial/)

### A simple window using MFC
Creating a custom sized window using MFC is more complicated than one using the WinAPI.  The code would have to call the CWnd::PreCreateWindow() function and specify the size of the window.  Setting the location of the window when creating the window in the WinAPI doesn't look to be possible.  So....even more code needed for a simple "how to program" example.  ***\*ouch!\****

MFC may be less code than the WinAPI when dealing with most of the features of the API, but not as simple when creating simple pared-down examples.

To be honest I don't know where to call the PreCreateWindow function in this example code.  I don't know all that much about MFC.
