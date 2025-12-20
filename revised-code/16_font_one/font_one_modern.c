#include <windows.h>

#include "resource.h"

HFONT    g_font          = NULL;
BOOL     g_opaque        = TRUE;
COLORREF g_rgbText       = RGB(0, 0, 0);
COLORREF g_rgbBackground = RGB(55, 55, 55);

COLORREF g_rgbCustom[16] = { 0 };

void DoSelectFont(HWND wnd)
{
   CHOOSEFONTW cf = { sizeof(CHOOSEFONTW) };
   LOGFONTW    lf;

   GetObjectW(g_font, sizeof(LOGFONTW), &lf);

   cf.Flags     = CF_EFFECTS | CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS;
   cf.hwndOwner = wnd;
   cf.lpLogFont = &lf;
   cf.rgbColors = g_rgbText;

   if ( ChooseFontW(&cf) )
   {
      HFONT font = CreateFontIndirectW(&lf);

      if ( font )
      {
         g_font = font;
      }
      else
      {
         MessageBoxW(wnd, L"Font creation failed!", L"Error", MB_OK | MB_ICONEXCLAMATION);
      }

      g_rgbText = cf.rgbColors;
   }
}

void DoSelectColour(HWND wnd)
{
   CHOOSECOLORW cc = { sizeof(CHOOSECOLORW) };

   cc.Flags        = CC_RGBINIT | CC_FULLOPEN | CC_ANYCOLOR;
   cc.hwndOwner    = wnd;
   cc.rgbResult    = g_rgbBackground;
   cc.lpCustColors = g_rgbCustom;

   if ( ChooseColorW(&cc) )
   {
      g_rgbBackground = cc.rgbResult;
   }
}

void DrawClientSize(HDC dc, RECT* rect, HFONT font)
{
   WCHAR size[100];
   WCHAR title[ ] = L"These are the dimensions of your client area:";
   HFONT oldFont  = (HFONT) SelectObject(dc, font);

   SetBkColor(dc, g_rgbBackground);
   SetTextColor(dc, g_rgbText);

   if ( g_opaque )
   {
      SetBkMode(dc, OPAQUE);
   }
   else
   {
      SetBkMode(dc, TRANSPARENT);
   }

   DrawTextW(dc, title, -1, rect, DT_WORDBREAK);

   wsprintfW(size, L"{%d, %d, %d, %d}", rect->left, rect->top, rect->right, rect->bottom);

   DrawTextW(dc, size, -1, rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

   SelectObject(dc, oldFont);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch ( msg )
   {
   case WM_CREATE:
      g_font = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
      break;

   case WM_CLOSE:
      DestroyWindow(hwnd);
      break;

   case WM_COMMAND:
      switch ( LOWORD(wParam) )
      {
      case ID_FILE_EXIT:
         PostMessageW(hwnd, WM_CLOSE, 0, 0);
         break;

      case ID_FORMAT_FONT:
         DoSelectFont(hwnd);

         InvalidateRect(hwnd, NULL, TRUE);
         UpdateWindow(hwnd);
         break;

      case ID_FORMAT_DEFAULTGUIFONT:
         DeleteObject(g_font);
         g_font = (HFONT) GetStockObject(DEFAULT_GUI_FONT);

         InvalidateRect(hwnd, NULL, TRUE);
         UpdateWindow(hwnd);
         break;

      case ID_FORMAT_TEST:
      {
         HFONT font;
         HDC   dc;
         long  fontHeight;

         dc         = GetDC(NULL);
         fontHeight = -MulDiv(12, GetDeviceCaps(dc, LOGPIXELSY), 72);
         ReleaseDC(NULL, dc);

         font = CreateFontW(fontHeight, 0, 0, 0, 0, TRUE, 0, 0, 0, 0, 0, 0, 0, L"Times New Roman");

         if ( font )
         {
            DeleteObject(g_font);
            g_font = font;
         }
         else
         {
            MessageBoxW(hwnd, L"Font creation failed!", L"Error", MB_OK | MB_ICONEXCLAMATION);
         }
         InvalidateRect(hwnd, NULL, TRUE);
         UpdateWindow(hwnd);
      }
      break;

      case ID_FORMAT_BACKGROUNDCOLOUR:
         DoSelectColour(hwnd);

         InvalidateRect(hwnd, NULL, TRUE);
         UpdateWindow(hwnd);
         break;

      case ID_FORMAT_OPAQUE:
         g_opaque = !g_opaque;

         InvalidateRect(hwnd, NULL, TRUE);
         UpdateWindow(hwnd);
         break;
      }
      break;

   case WM_INITMENUPOPUP:
      CheckMenuItem((HMENU) wParam, ID_FORMAT_OPAQUE, MF_BYCOMMAND | (g_opaque ? MF_CHECKED : MF_UNCHECKED));
      break;

   case WM_PAINT:
   {
      RECT        client;
      PAINTSTRUCT ps;
      HDC         dc = BeginPaint(hwnd, &ps);

      GetClientRect(hwnd, &client);

      DrawClientSize(dc, &client, g_font);

      EndPaint(hwnd, &ps);
   }
   break;

   case WM_DESTROY:
      DeleteObject(g_font);

      PostQuitMessage(0);
      break;

   default:
      return DefWindowProcW(hwnd, msg, wParam, lParam);
   }
   return 0;
}

int WINAPI wWinMain(_In_ HINSTANCE inst, _In_opt_ HINSTANCE prevInst,
                    _In_ PWSTR cmdLine, _In_ int cmdShow)
{
   UNREFERENCED_PARAMETER(prevInst);
   UNREFERENCED_PARAMETER(cmdLine);

   WNDCLASSEXW wc        = { 0 };
   HWND        wnd;
   MSG         msg;
   PCWSTR      className = L"myWindowClass";

   wc.cbSize        = sizeof(WNDCLASSEXW);
   wc.style         = CS_VREDRAW | CS_HREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = inst;
   wc.hIcon         = (HICON)   LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hIconSm       = (HICON)   LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH)  (COLOR_WINDOW + 1);
   wc.lpszMenuName  = MAKEINTRESOURCEW(IDR_MENU);
   wc.lpszClassName = className;

   if ( !RegisterClassExW(&wc) )
   {
      MessageBoxW(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
      return 0;
   }

   wnd = CreateWindowExW(WS_EX_CLIENTEDGE,
                         className,
                         L"A Font Program",
                         WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
                         NULL, NULL, inst, NULL);

   if ( wnd == NULL )
   {
      MessageBoxW(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
      return 0;
   }

   ShowWindow(wnd, cmdShow);
   UpdateWindow(wnd);

   while ( GetMessageW(&msg, NULL, 0, 0) > 0 )
   {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
   }
   return (int) msg.wParam;
}
