// when using MFC stops Visual Studio from whinging the windows version isn't set
#include <sdkddkver.h>

// the MFC equivalent for windows.h
#include <afxwin.h>

int result = AfxMessageBox(L"Hello MFC!", MB_OK | MB_ICONASTERISK);
