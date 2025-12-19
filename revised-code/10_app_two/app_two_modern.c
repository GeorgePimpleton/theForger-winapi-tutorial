#include <windows.h>
#include "resource.h"

BOOL    LoadTextFileToEdit( HWND, PCSTR );
BOOL    SaveTextFileFromEdit( HWND, PCSTR );
void    DoFileOpen( HWND );
void    DoFileSave( HWND );
LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

int WINAPI wWinMain( _In_     HINSTANCE inst,
                     _In_opt_ HINSTANCE prevInst,
                     _In_     PWSTR     cmdLine,
                     _In_     int       cmdShow )
{
   UNREFERENCED_PARAMETER( prevInst );
   UNREFERENCED_PARAMETER( cmdLine );

   WNDCLASSEXW wc        = { 0 };
   HWND        wnd;
   MSG         msg;
   PCWSTR      className = L"myWindowClass";

   wc.cbSize        = sizeof( WNDCLASSEXW );
   wc.style         = 0;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = inst;
   wc.hIcon         = ( HICON ) LoadImageW( NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR );
   wc.hIconSm       = ( HICON ) LoadImageW( NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR );
   wc.hCursor       = ( HCURSOR ) LoadImageW( NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED );
   wc.hbrBackground = ( HBRUSH ) ( COLOR_WINDOW + 1 );
   wc.lpszMenuName  = MAKEINTRESOURCEW( IDR_MAINMENU );
   wc.lpszClassName = className;

   if ( !RegisterClassExW( &wc ) )
   {
      MessageBoxW( NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK );
      return 0;
   }

   wnd = CreateWindowExW( 0,
                          className,
                          L"theForger's Tutorial Application",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT, 480, 320,
                          NULL, NULL, inst, NULL );

   if ( wnd == NULL )
   {
      MessageBoxW( NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK );
      return 0;
   }

   ShowWindow( wnd, cmdShow );
   UpdateWindow( wnd );

   while ( GetMessageW( &msg, NULL, 0, 0 ) > 0 )
   {
      TranslateMessage( &msg );
      DispatchMessageW( &msg );
   }
   return ( int ) msg.wParam;
}
BOOL LoadTextFileToEdit( HWND edit, PCSTR fileName )
{
   HANDLE file;
   BOOL   success = FALSE;

   file = CreateFileA( fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );

   if ( file != INVALID_HANDLE_VALUE )
   {
      DWORD fileSize;

      fileSize = GetFileSize( file, NULL );

      if ( fileSize != 0xFFFFFFFF )
      {
         PSTR fileText;

         fileText = ( PSTR ) GlobalAlloc( GPTR, fileSize + 1 );

         if ( fileText != NULL )
         {
            DWORD read;

            if ( ReadFile( file, fileText, fileSize, &read, NULL ) )
            {
               fileText[fileSize] = 0; // Add null terminator

               if ( SetWindowTextA( edit, fileText ) )
               {
                  success = TRUE; // It worked!
               }
            }
            GlobalFree( fileText );
         }
      }
      CloseHandle( file );
   }
   return success;
}

BOOL SaveTextFileFromEdit( HWND edit, PCSTR fileName )
{
   HANDLE file;
   BOOL   success = FALSE;

   file = CreateFileA( fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

   if ( file != INVALID_HANDLE_VALUE )
   {
      DWORD textLength;

      textLength = GetWindowTextLengthA( edit );

      // No need to bother if there's no text.
      if ( textLength > 0 )
      {
         PSTR  text;
         DWORD bufferSize = textLength + 1;

         text = ( PSTR ) GlobalAlloc( GPTR, bufferSize );

         if ( text != NULL )
         {
            if ( GetWindowTextA( edit, text, bufferSize ) )
            {
               DWORD written;

               if ( WriteFile( file, text, textLength, &written, NULL ) )
               {
                  success = TRUE;
               }
            }
            GlobalFree( text );
         }
      }
      CloseHandle( file );
   }
   return success;
}

void DoFileOpen( HWND wnd )
{
   OPENFILENAMEA ofn;
   CHAR          fileName[MAX_PATH] = "";

   ZeroMemory( &ofn, sizeof( ofn ) );

   ofn.lStructSize = sizeof( OPENFILENAME );
   ofn.hwndOwner   = wnd;
   ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
   ofn.lpstrFile   = fileName;
   ofn.nMaxFile    = MAX_PATH;
   ofn.Flags       = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
   ofn.lpstrDefExt = "txt";

   if ( GetOpenFileNameA( &ofn ) )
   {
      HWND edit = GetDlgItem( wnd, IDC_MAIN_EDIT );
      LoadTextFileToEdit( edit, fileName );
   }
}

void DoFileSave( HWND wnd )
{
   OPENFILENAMEA ofn;
   CHAR          fileName[MAX_PATH] = "";

   ZeroMemory( &ofn, sizeof( ofn ) );

   ofn.lStructSize = sizeof( OPENFILENAME );
   ofn.hwndOwner   = wnd;
   ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
   ofn.lpstrFile   = fileName;
   ofn.nMaxFile    = MAX_PATH;
   ofn.lpstrDefExt = "txt";
   ofn.Flags       = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

   if ( GetSaveFileNameA( &ofn ) )
   {
      HWND edit = GetDlgItem( wnd, IDC_MAIN_EDIT );
      SaveTextFileFromEdit( edit, fileName );
   }
}

LRESULT CALLBACK WndProc( HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
   switch ( msg )
   {
   case WM_CREATE:
   {
      HFONT hfDefault;
      HWND hEdit;

      hEdit = CreateWindowExW( WS_EX_CLIENTEDGE, L"EDIT", L"",
                               WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
                               0, 0, 100, 100, wnd, ( HMENU ) IDC_MAIN_EDIT, GetModuleHandleW( NULL ), NULL );

      if ( hEdit == NULL )
      {
         MessageBoxW( wnd, L"Could not create edit box.", L"Error", MB_OK | MB_ICONERROR );
      }

      hfDefault = ( HFONT ) GetStockObject( DEFAULT_GUI_FONT );
   #pragma warning (disable: 6387)
      SendMessageW( hEdit, WM_SETFONT, ( WPARAM ) hfDefault, MAKELPARAM( FALSE, 0 ) );
   }
   break;

   case WM_SIZE:
   {
      HWND edit;
      RECT client;

      GetClientRect( wnd, &client );

      edit = GetDlgItem( wnd, IDC_MAIN_EDIT );
      SetWindowPos( edit, NULL, 0, 0, client.right, client.bottom, SWP_NOZORDER );
   }
   break;

   case WM_CLOSE:
      DestroyWindow( wnd );
      break;

   case WM_DESTROY:
      PostQuitMessage( 0 );
      break;

   case WM_COMMAND:
      switch ( LOWORD( wParam ) )
      {
      case ID_FILE_EXIT:
         PostMessageW( wnd, WM_CLOSE, 0, 0 );
         break;

      case ID_FILE_NEW:
         SetDlgItemTextW( wnd, IDC_MAIN_EDIT, L"" );
         break;

      case ID_FILE_OPEN:
         DoFileOpen( wnd );
         break;

      case ID_FILE_SAVEAS:
         DoFileSave( wnd );
         break;
      }
      break;
   }

   return DefWindowProcW( wnd, msg, wParam, lParam );
}
