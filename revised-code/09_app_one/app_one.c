#include <windows.h>

const WCHAR g_szClassName[ ] = L"myWindowClass";

#define IDC_MAIN_EDIT	101

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch (msg)
   {
   case WM_CREATE:
   {
      HFONT hfDefault;
      HWND  hEdit;

      hEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
                             WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
                             0, 0, 100, 100, hwnd, (HMENU) IDC_MAIN_EDIT, GetModuleHandle(NULL), NULL);
      if (hEdit == NULL)
         MessageBoxW(hwnd, L"Could not create edit box.", L"Error", MB_OK | MB_ICONERROR);

      hfDefault = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
   #pragma warning (disable : 6387)
      SendMessageW(hEdit, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM(FALSE, 0));
   }
   break;

   case WM_SIZE:
   {
      HWND hEdit;
      RECT rcClient;

      GetClientRect(hwnd, &rcClient);

      hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
      SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);
   }
   break;

   case WM_CLOSE:
      DestroyWindow(hwnd);
      break;

   case WM_DESTROY:
      PostQuitMessage(0);
      break;

   default:
      return DefWindowProcW(hwnd, msg, wParam, lParam);
   }
   return 0;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                    _In_ PWSTR lpCmdLine, _In_ int nCmdShow)
{
   WNDCLASSEXW wc;
   HWND        hwnd;
   MSG         Msg;

   wc.cbSize        = sizeof(WNDCLASSEXW);
   wc.style         = 0;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = hInstance;
   wc.hIcon         = (HICON)   LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hIconSm       = (HICON)   LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = g_szClassName;

   if (!RegisterClassExW(&wc))
   {
      MessageBoxW(NULL, L"Window Registration Failed!", L"Error!",
                 MB_ICONEXCLAMATION | MB_OK);
      return 0;
   }

   hwnd = CreateWindowExW(0,
                          g_szClassName,
                          L"theForger's Tutorial Application",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT, 480, 320,
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