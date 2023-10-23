#include <windows.h>

#define ID_FILE_EXIT 9001
#define ID_STUFF_GO  9002

const WCHAR g_szClassName[ ] = L"myWindowClass";

LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch (msg)
   {
   case WM_CREATE:
   {
      HMENU menu;
      HMENU subMenu;
      HICON icon;
      HICON iconSm;

      menu = CreateMenu();

      subMenu = CreatePopupMenu();
      AppendMenuW(subMenu, MF_STRING, ID_FILE_EXIT, L"E&xit");
      AppendMenuW(menu, MF_STRING | MF_POPUP, (UINT_PTR) subMenu, L"&File");

      subMenu = CreatePopupMenu();
      AppendMenuW(subMenu, MF_STRING, ID_STUFF_GO, L"&Go");
      AppendMenuW(menu, MF_STRING | MF_POPUP, (UINT_PTR) subMenu, L"&Stuff");

      SetMenu(wnd, menu);

      icon = (HICON) LoadImageW(NULL, L"menu_two.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
      if (icon)
      {
         SendMessageW(wnd, WM_SETICON, ICON_BIG, (LPARAM) icon);
      }
      else
      {
         MessageBoxW(wnd, L"Could not load large icon! Is it in the current working directory?", L"Error", MB_OK | MB_ICONERROR);
      }

      iconSm = (HICON) LoadImageW(NULL, L"menu_two.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);

      if (iconSm)
      {
         SendMessageW(wnd, WM_SETICON, ICON_SMALL, (LPARAM) iconSm);
      }
      else
      {
         MessageBoxW(wnd, L"Could not load small icon! Is it in the current working directory?", L"Error", MB_OK | MB_ICONERROR);
      }
   }
   break;

   case WM_COMMAND:
      switch (LOWORD(wParam))
      {
      case ID_FILE_EXIT:
         PostMessageW(wnd, WM_CLOSE, 0, 0);
         break;

      case ID_STUFF_GO:
         MessageBoxW(wnd, L"You clicked Go!", L"Woo!", MB_OK);
         break;
      }
      break;

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

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                    _In_ PWSTR lpCmdLine, _In_ int nCmdShow)
{
   WNDCLASSEXW wc = { 0 };
   HWND        wnd;
   MSG         msg;

   wc.cbSize        = sizeof(WNDCLASSEXW);
   wc.style         = 0;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = hInstance;
   wc.hIcon         = NULL;
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = g_szClassName;
   wc.hIconSm       = NULL;

   if (!RegisterClassExW(&wc))
   {
      MessageBoxW(NULL, L"Window Registration Failed!", L"Error!",
                  MB_ICONEXCLAMATION | MB_OK);
      return 0;
   }

   wnd = CreateWindowExW(WS_EX_CLIENTEDGE,
                          g_szClassName,
                          L"A Menu #2",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
                          NULL, NULL, hInstance, NULL);

   if (wnd == NULL)
   {
      MessageBoxW(NULL, L"Window Creation Failed!", L"Error!",
                  MB_ICONEXCLAMATION | MB_OK);
      return 0;
   }

   ShowWindow(wnd, nCmdShow);
   UpdateWindow(wnd);

   while (GetMessageW(&msg, NULL, 0, 0) > 0)
   {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
   }
   return (int) msg.wParam;
}