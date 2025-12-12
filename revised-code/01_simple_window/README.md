# theForger's Win32 API tutorial, revised and updated code
[![Language](https://img.shields.io/badge/Language%20-C-blue.svg)](https://github.com/GeorgePimpleton/theForger-winapi-tutorial/)
[![Platform](https://img.shields.io/badge/Platform%20-Win32-blue.svg)](https://github.com/GeorgePimpleton/theForger-winapi-tutorial/)

### A simple window using MFC
Creating a custom sized window using MFC is more complicated than using the WinAPI.  ~~The code would have to call the CWnd::PreCreateWindow() function and specify the size of the window.  Setting the location of the window when creating the window in the WinAPI doesn't look to be possible.  So....even more code needed for a simple "how to program" example.  **\*ouch!\***~~

Well, there are several ways to create a sized window with MFC, sadly they bloat up the code.  One approach, the one shown here doesn't use the PreCreateWindow() method, reveals how MFC encapsulates the underlying WinAPI.

To be honest I don't know where to call the PreCreateWindow() function for this small sized window example.  Apparently that is the preferred method vs. the method I used.  I don't know all that much about MFC.
