#include <windows.h>

#include "resource.h"

const WCHAR g_szClassName[ ] = L"myWindowClass";

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
   switch (Message)
   {
   case WM_COMMAND:
      switch (LOWORD(wParam))
      {
      case ID_FILE_EXIT:
         PostMessageW(hwnd, WM_CLOSE, 0, 0);
         break;

      case ID_STUFF_GO:
         MessageBoxW(hwnd, L"You clicked Go!", L"Woo!", MB_OK);
         break;
      }
      break;

   case WM_CLOSE:
      DestroyWindow(hwnd);
      break;

   case WM_DESTROY:
      PostQuitMessage(0);
      break;
   default:
      return DefWindowProcW(hwnd, Message, wParam, lParam);
   }
   return 0;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                    _In_ PWSTR lpCmdLine, _In_ int nCmdShow)
{
   WNDCLASSEXW wc;
   HWND hwnd;
   MSG Msg;

   wc.cbSize        = sizeof(WNDCLASSEXW);
   wc.style         = 0;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = hInstance;
   wc.hIcon         = (HICON)   LoadImageW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(IDI_MYICON), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hIconSm       = (HICON)   LoadImageW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(IDI_MYICON), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
   wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MYMENU);
   wc.lpszClassName = g_szClassName;

   if (!RegisterClassExW(&wc))
   {
      MessageBoxW(NULL, L"Window Registration Failed!", L"Error!",
                 MB_ICONEXCLAMATION | MB_OK);
      return 0;
   }

   hwnd = CreateWindowExW(WS_EX_CLIENTEDGE,
                          g_szClassName,
                          L"A Menu",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
                          NULL, NULL, hInstance, NULL);

   if (hwnd == NULL)
   {
      MessageBoxW(NULL, L"Window Creation Failed!", L"Error!",
                 MB_ICONEXCLAMATION | MB_OK);
      return 0;
   }

   ShowWindow(hwnd, nCmdShow);
   UpdateWindow(hwnd);

   while (GetMessageW(&Msg, NULL, 0, 0) > 0)
   {
      TranslateMessage(&Msg);
      DispatchMessageW(&Msg);
   }
   return (int) Msg.wParam;
}
