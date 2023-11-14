#include <windows.h>
#include <commctrl.h>
#include "resource.h"

#pragma comment (lib, "comctl32.lib")

#define IDC_MAIN_EDIT	101
#define IDC_MAIN_TOOL	102
#define IDC_MAIN_STATUS	103

BOOL LoadTextFileToEdit(HWND edit, PCWSTR fileName)
{
   HANDLE file;
   BOOL   success = FALSE;

   file = CreateFileW(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

   if ( file != INVALID_HANDLE_VALUE )
   {
      DWORD fileSize;

      fileSize = GetFileSize(file, NULL);

      if ( fileSize != 0xFFFFFFFF )
      {
         PSTR fileText;

         fileText = (PSTR) GlobalAlloc(GPTR, fileSize + 1);

         if ( fileText != NULL )
         {
            DWORD read;

            if ( ReadFile(file, fileText, fileSize, &read, NULL) )
            {
               fileText[fileSize] = 0; // Add null terminator

               if ( SetWindowTextA(edit, fileText) )
               {
                  success = TRUE; // It worked!
               }
            }
            GlobalFree(fileText);
         }
      }
      CloseHandle(file);
   }
   return success;
}

BOOL SaveTextFileFromEdit(HWND edit, PCWSTR fileName)
{
   HANDLE file;
   BOOL   success = FALSE;

   file = CreateFileW(fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

   if ( file != INVALID_HANDLE_VALUE )
   {
      DWORD textLength;

      textLength = GetWindowTextLengthW(edit);

      // No need to bother if there's no text.
      if ( textLength > 0 )
      {
         PSTR  text;
         DWORD bufferSize = textLength + 1;

         text = (PSTR) GlobalAlloc(GPTR, bufferSize);

         if ( text != NULL )
         {
            if ( GetWindowTextA(edit, text, bufferSize) )
            {
               DWORD written;

               if ( WriteFile(file, text, textLength, &written, NULL) )
               {
                  success = TRUE;
               }
            }
            GlobalFree(text);
         }
      }
      CloseHandle(file);
   }
   return success;
}

void DoFileOpen(HWND wnd)
{
   OPENFILENAMEW ofn;
   WCHAR         fileName[MAX_PATH] = L"";

   ZeroMemory(&ofn, sizeof(ofn));

   ofn.lStructSize = sizeof(ofn);
   ofn.hwndOwner   = wnd;
   ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
   ofn.lpstrFile   = fileName;
   ofn.nMaxFile    = MAX_PATH;
   ofn.Flags       = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
   ofn.lpstrDefExt = L"txt";

   if ( GetOpenFileNameW(&ofn) )
   {
      HWND edit = GetDlgItem(wnd, IDC_MAIN_EDIT);

      if ( LoadTextFileToEdit(edit, fileName) )
      {
         SendDlgItemMessageW(wnd, IDC_MAIN_STATUS, SB_SETTEXT, 0, (LPARAM) L"Opened...");
         SendDlgItemMessageW(wnd, IDC_MAIN_STATUS, SB_SETTEXT, 1, (LPARAM) fileName);
      }
   }
}

void DoFileSave(HWND wnd)
{
   OPENFILENAMEW ofn;
   WCHAR         fileName[MAX_PATH] = L"";

   ZeroMemory(&ofn, sizeof(ofn));

   ofn.lStructSize = sizeof(ofn);
   ofn.hwndOwner   = wnd;
   ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
   ofn.lpstrFile   = fileName;
   ofn.nMaxFile    = MAX_PATH;
   ofn.lpstrDefExt = L"txt";
   ofn.Flags       = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

   if ( GetSaveFileNameW(&ofn) )
   {
      HWND edit = GetDlgItem(wnd, IDC_MAIN_EDIT);

      if ( SaveTextFileFromEdit(edit, fileName) )
      {
         SendDlgItemMessageW(wnd, IDC_MAIN_STATUS, SB_SETTEXT, 0, (LPARAM) L"Saved...");
         SendDlgItemMessageA(wnd, IDC_MAIN_STATUS, SB_SETTEXT, 1, (LPARAM) fileName);
      }
   }
}

LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch ( msg )
   {
   case WM_CREATE:
   {
      HFONT fontDefault;
      HWND  edit;

      HWND        tool;
      TBBUTTON    tbb[3];
      TBADDBITMAP tbab;

      HWND status;
      int  statwidths[ ] = { 100, -1 };

      // Create Edit Control
      edit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
                              WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
                              0, 0, 100, 100, wnd, (HMENU) IDC_MAIN_EDIT, GetModuleHandleW(NULL), NULL);
      if ( edit == NULL )
      {
         MessageBoxW(wnd, L"Could not create edit box.", L"Error", MB_OK | MB_ICONERROR);
      }

      fontDefault = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
   #pragma warning (disable : 6387)
      SendMessageW(edit, WM_SETFONT, (WPARAM) fontDefault, MAKELPARAM(FALSE, 0));

      // Create Toolbar
      tool = CreateWindowExW(0, TOOLBARCLASSNAMEW, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0,
                              wnd, (HMENU) IDC_MAIN_TOOL, GetModuleHandleW(NULL), NULL);

      if ( tool == NULL )
      {
         MessageBoxW(wnd, L"Could not create tool bar.", L"Error", MB_OK | MB_ICONERROR);
      }

      // Send the TB_BUTTONSTRUCTSIZE message, which is required for
      // backward compatibility.
      SendMessageW(tool, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0);

      tbab.hInst         = HINST_COMMCTRL;
      tbab.nID           = IDB_STD_SMALL_COLOR;
      SendMessageW(tool, TB_ADDBITMAP, 0, (LPARAM) &tbab);

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

      SendMessageW(tool, TB_ADDBUTTONS, sizeof(tbb) / sizeof(TBBUTTON), (LPARAM) &tbb);

      // Create Status bar
      status = CreateWindowExW(0, STATUSCLASSNAMEW, NULL,
                                WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0,
                                wnd, (HMENU) IDC_MAIN_STATUS, GetModuleHandleW(NULL), NULL);

      SendMessageW(status, SB_SETPARTS, sizeof(statwidths) / sizeof(int), (LPARAM) statwidths);
      SendMessageW(status, SB_SETTEXT, 0, (LPARAM) L"Hi there :)");
   }
   break;

   case WM_SIZE:
   {
      HWND tool;
      RECT toolRect;
      int  toolHeight;

      HWND status;
      RECT statusRect;
      int  statusHeight;

      HWND edit;
      int  editHeight;
      RECT clientRect;

      // Size toolbar and get height
      tool = GetDlgItem(wnd, IDC_MAIN_TOOL);
      SendMessageW(tool, TB_AUTOSIZE, 0, 0);

      GetWindowRect(tool, &toolRect);
      toolHeight = toolRect.bottom - toolRect.top;

      // Size status bar and get height
      status = GetDlgItem(wnd, IDC_MAIN_STATUS);
      SendMessageW(status, WM_SIZE, 0, 0);

      GetWindowRect(status, &statusRect);
      statusHeight = statusRect.bottom - statusRect.top;

      // Calculate remaining height and size edit
      GetClientRect(wnd, &clientRect);

      editHeight = clientRect.bottom - toolHeight - statusHeight;

      edit = GetDlgItem(wnd, IDC_MAIN_EDIT);
      SetWindowPos(edit, NULL, 0, toolHeight, clientRect.right, editHeight, SWP_NOZORDER);
   }
   break;

   case WM_CLOSE:
      DestroyWindow(wnd);
      break;

   case WM_DESTROY:
      PostQuitMessage(0);
      break;

   case WM_COMMAND:
      switch ( LOWORD(wParam) )
      {
      case ID_FILE_EXIT:
         PostMessageW(wnd, WM_CLOSE, 0, 0);
         break;

      case ID_FILE_NEW:
         SetDlgItemTextA(wnd, IDC_MAIN_EDIT, "");
         break;

      case ID_FILE_OPEN:
         DoFileOpen(wnd);
         break;

      case ID_FILE_SAVEAS:
         DoFileSave(wnd);
         break;
      }
      break;

   default:
      return DefWindowProcW(wnd, msg, wParam, lParam);
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

   InitCommonControls( );

   wc.cbSize        = sizeof(WNDCLASSEXW);
   wc.style         = 0;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = inst;
   wc.hIcon         = (HICON)   LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hIconSm       = (HICON)   LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH)  (COLOR_WINDOW + 1);
   wc.lpszMenuName  = MAKEINTRESOURCEW(IDR_MAINMENU);
   wc.lpszClassName = className;

   if ( !RegisterClassExW(&wc) )
   {
      MessageBoxW(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
      return 0;
   }

   wnd = CreateWindowExW(0,
                         className,
                         L"theForger's Tutorial Application",
                         WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                         CW_USEDEFAULT, CW_USEDEFAULT, 480, 320,
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
