#include <windows.h>

// WinAPI has several pointer to string data type
// this is a constant pointer
PCWSTR g_className = L"myWindowClass";

// Step 4: the Window Procedure
LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch ( msg )
   {
   case WM_CLOSE:
      DestroyWindow(wnd);
      break;

   case WM_DESTROY:
      PostQuitMessage(0);
      break;

   default:
      return DefWindowProcW(wnd, msg, wParam, lParam);
   }

   return 0;
}

// Step 0: Windows desktop WinAPI app entry point
int WINAPI wWinMain(_In_     HINSTANCE inst,
                    _In_opt_ HINSTANCE prevInst,
                    _In_     PWSTR     cmdLine,
                    _In_     int       cmdShow)
{
   UNREFERENCED_PARAMETER(prevInst);
   UNREFERENCED_PARAMETER(cmdLine);

   WNDCLASSEXW wc = { 0 };
   HWND        wnd;
   MSG         msg;

   // Step 1: Registering the Window Class
   wc.cbSize        = sizeof(WNDCLASSEXW);
   wc.style         = 0;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = inst;
   wc.hIcon         = (HICON)   LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH)  (COLOR_WINDOW + 1);
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = g_className;
   wc.hIconSm       = (HICON) LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);

   if ( !RegisterClassExW(&wc) )
   {
      MessageBoxW(NULL, L"Window Registration Failed!", L"Error!",
                  MB_ICONEXCLAMATION | MB_OK);
      return 0;
   }

   // Step 2: Creating (and showing) the Window
   wnd = CreateWindowExW(WS_EX_CLIENTEDGE,
                         g_className,
                         L"The title of my window",
                         WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
                         NULL, NULL, inst, NULL);

   if ( wnd == NULL )
   {
      MessageBoxW(NULL, L"Window Creation Failed!", L"Error!",
                  MB_ICONEXCLAMATION | MB_OK);
      return 0;
   }

   ShowWindow(wnd, cmdShow);
   UpdateWindow(wnd);

   // Step 3: The Message Loop
   while ( GetMessageW(&msg, NULL, 0, 0) > 0 )
   {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
   }
   return (int) msg.wParam;  // needed to stop VS from whinging about possible loss of data from a conversion
}
