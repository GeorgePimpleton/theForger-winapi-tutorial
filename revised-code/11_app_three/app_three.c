#include <windows.h>
#include <commctrl.h>
#include "resource.h"

#pragma comment (lib, "comctl32.lib")

const WCHAR g_szClassName[ ] = L"myWindowClass";

#define IDC_MAIN_EDIT	101
#define IDC_MAIN_TOOL	102
#define IDC_MAIN_STATUS	103

BOOL LoadTextFileToEdit(HWND hEdit, PCWSTR pszFileName)
{
   HANDLE hFile;
   BOOL   bSuccess = FALSE;

   hFile = CreateFileW(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
                       OPEN_EXISTING, 0, NULL);
   if (hFile != INVALID_HANDLE_VALUE)
   {
      DWORD dwFileSize;

      dwFileSize = GetFileSize(hFile, NULL);
      if (dwFileSize != 0xFFFFFFFF)
      {
         PSTR pszFileText;

         pszFileText = (PSTR) GlobalAlloc(GPTR, dwFileSize + 1);
         if (pszFileText != NULL)
         {
            DWORD dwRead;

            if (ReadFile(hFile, pszFileText, dwFileSize, &dwRead, NULL))
            {
               pszFileText[dwFileSize] = 0; // Add null terminator
               if (SetWindowTextA(hEdit, pszFileText))
                  bSuccess = TRUE; // It worked!
            }
            GlobalFree(pszFileText);
         }
      }
      CloseHandle(hFile);
   }
   return bSuccess;
}

BOOL SaveTextFileFromEdit(HWND hEdit, PCWSTR pszFileName)
{
   HANDLE hFile;
   BOOL   bSuccess = FALSE;

   hFile = CreateFileW(pszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
   if (hFile != INVALID_HANDLE_VALUE)
   {
      DWORD dwTextLength;

      dwTextLength = GetWindowTextLengthW(hEdit);
      // No need to bother if there's no text.
      if (dwTextLength > 0)
      {
         PSTR  pszText;
         DWORD dwBufferSize = dwTextLength + 1;

         pszText = (PSTR) GlobalAlloc(GPTR, dwBufferSize);
         if (pszText != NULL)
         {
            if (GetWindowTextA(hEdit, pszText, dwBufferSize))
            {
               DWORD dwWritten;

               if (WriteFile(hFile, pszText, dwTextLength, &dwWritten, NULL))
                  bSuccess = TRUE;
            }
            GlobalFree(pszText);
         }
      }
      CloseHandle(hFile);
   }
   return bSuccess;
}

void DoFileOpen(HWND hwnd)
{
   OPENFILENAMEW ofn;
   WCHAR         szFileName[MAX_PATH] = L"";

   ZeroMemory(&ofn, sizeof(ofn));

   ofn.lStructSize = sizeof(ofn);
   ofn.hwndOwner   = hwnd;
   ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
   ofn.lpstrFile   = szFileName;
   ofn.nMaxFile    = MAX_PATH;
   ofn.Flags       = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
   ofn.lpstrDefExt = L"txt";

   if (GetOpenFileNameW(&ofn))
   {
      HWND hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
      if (LoadTextFileToEdit(hEdit, szFileName))
      {
         SendDlgItemMessageW(hwnd, IDC_MAIN_STATUS, SB_SETTEXT, 0, (LPARAM) L"Opened...");
         SendDlgItemMessageW(hwnd, IDC_MAIN_STATUS, SB_SETTEXT, 1, (LPARAM) szFileName);
      }
   }
}

void DoFileSave(HWND hwnd)
{
   OPENFILENAMEW ofn;
   WCHAR         szFileName[MAX_PATH] = L"";

   ZeroMemory(&ofn, sizeof(ofn));

   ofn.lStructSize = sizeof(ofn);
   ofn.hwndOwner   = hwnd;
   ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
   ofn.lpstrFile   = szFileName;
   ofn.nMaxFile    = MAX_PATH;
   ofn.lpstrDefExt = L"txt";
   ofn.Flags       = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

   if (GetSaveFileNameW(&ofn))
   {
      HWND hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
      if (SaveTextFileFromEdit(hEdit, szFileName))
      {
         SendDlgItemMessageW(hwnd, IDC_MAIN_STATUS, SB_SETTEXT, 0, (LPARAM) L"Saved...");
         SendDlgItemMessageA(hwnd, IDC_MAIN_STATUS, SB_SETTEXT, 1, (LPARAM) szFileName);
      }
   }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch (msg)
   {
   case WM_CREATE:
   {
      HFONT hfDefault;
      HWND  hEdit;

      HWND        hTool;
      TBBUTTON    tbb[3];
      TBADDBITMAP tbab;

      HWND hStatus;
      int  statwidths[ ] = { 100, -1 };

      // Create Edit Control
      hEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
                              WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
                              0, 0, 100, 100, hwnd, (HMENU) IDC_MAIN_EDIT, GetModuleHandleW(NULL), NULL);
      if (hEdit == NULL)
         MessageBoxW(hwnd, L"Could not create edit box.", L"Error", MB_OK | MB_ICONERROR);

      hfDefault = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
   #pragma warning (disable : 6387)
      SendMessageW(hEdit, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM(FALSE, 0));

      // Create Toolbar
      hTool = CreateWindowExW(0, TOOLBARCLASSNAMEW, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0,
                              hwnd, (HMENU) IDC_MAIN_TOOL, GetModuleHandleW(NULL), NULL);
      if (hTool == NULL)
         MessageBoxW(hwnd, L"Could not create tool bar.", L"Error", MB_OK | MB_ICONERROR);

      // Send the TB_BUTTONSTRUCTSIZE message, which is required for
      // backward compatibility.
      SendMessageW(hTool, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0);

      tbab.hInst = HINST_COMMCTRL;
      tbab.nID   = IDB_STD_SMALL_COLOR;
      SendMessageW(hTool, TB_ADDBITMAP, 0, (LPARAM) &tbab);

      ZeroMemory(tbb, sizeof(tbb));
      tbb[0].iBitmap   = STD_FILENEW;
      tbb[0].fsState   = TBSTATE_ENABLED;
      tbb[0].fsStyle   = TBSTYLE_BUTTON;
      tbb[0].idCommand = ID_FILE_NEW;

      tbb[1].iBitmap   = STD_FILEOPEN;
      tbb[1].fsState   = TBSTATE_ENABLED;
      tbb[1].fsStyle   = TBSTYLE_BUTTON;
      tbb[1].idCommand = ID_FILE_OPEN;

      tbb[2].iBitmap   = STD_FILESAVE;
      tbb[2].fsState   = TBSTATE_ENABLED;
      tbb[2].fsStyle   = TBSTYLE_BUTTON;
      tbb[2].idCommand = ID_FILE_SAVEAS;

      SendMessageW(hTool, TB_ADDBUTTONS, sizeof(tbb) / sizeof(TBBUTTON), (LPARAM) &tbb);

      // Create Status bar
      hStatus = CreateWindowExW(0, STATUSCLASSNAMEW, NULL,
                                WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0,
                                hwnd, (HMENU) IDC_MAIN_STATUS, GetModuleHandleW(NULL), NULL);

      SendMessageW(hStatus, SB_SETPARTS, sizeof(statwidths) / sizeof(int), (LPARAM) statwidths);
      SendMessageW(hStatus, SB_SETTEXT, 0, (LPARAM) L"Hi there :)");
   }
   break;

   case WM_SIZE:
   {
      HWND hTool;
      RECT rcTool;
      int  iToolHeight;

      HWND hStatus;
      RECT rcStatus;
      int  iStatusHeight;

      HWND hEdit;
      int  iEditHeight;
      RECT rcClient;

      // Size toolbar and get height
      hTool = GetDlgItem(hwnd, IDC_MAIN_TOOL);
      SendMessageW(hTool, TB_AUTOSIZE, 0, 0);

      GetWindowRect(hTool, &rcTool);
      iToolHeight = rcTool.bottom - rcTool.top;

      // Size status bar and get height
      hStatus = GetDlgItem(hwnd, IDC_MAIN_STATUS);
      SendMessageW(hStatus, WM_SIZE, 0, 0);

      GetWindowRect(hStatus, &rcStatus);
      iStatusHeight = rcStatus.bottom - rcStatus.top;

      // Calculate remaining height and size edit
      GetClientRect(hwnd, &rcClient);

      iEditHeight = rcClient.bottom - iToolHeight - iStatusHeight;

      hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
      SetWindowPos(hEdit, NULL, 0, iToolHeight, rcClient.right, iEditHeight, SWP_NOZORDER);
   }
   break;

   case WM_CLOSE:
      DestroyWindow(hwnd);
      break;

   case WM_DESTROY:
      PostQuitMessage(0);
      break;

   case WM_COMMAND:
      switch (LOWORD(wParam))
      {
      case ID_FILE_EXIT:
         PostMessageW(hwnd, WM_CLOSE, 0, 0);
         break;

      case ID_FILE_NEW:
         SetDlgItemTextA(hwnd, IDC_MAIN_EDIT, "");
         break;

      case ID_FILE_OPEN:
         DoFileOpen(hwnd);
         break;

      case ID_FILE_SAVEAS:
         DoFileSave(hwnd);
         break;
      }
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

   InitCommonControls();

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
   wc.lpszMenuName  = MAKEINTRESOURCEW(IDR_MAINMENU);
   wc.lpszClassName = g_szClassName;

   if (!RegisterClassExW(&wc))
   {
      MessageBoxW(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
      return 0;
   }

   hwnd = CreateWindowExW(0,
                          g_szClassName,
                          L"theForger's Tutorial Application",
                          WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                          CW_USEDEFAULT, CW_USEDEFAULT, 480, 320,
                          NULL, NULL, hInstance, NULL);

   if (hwnd == NULL)
   {
      MessageBoxW(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
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
