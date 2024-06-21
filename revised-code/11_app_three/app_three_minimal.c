#include <windows.h>
#include <commctrl.h>
#include "resource.h"

// tell Visual Studio to add the common controls library
// without manually adjusting the project settings
#pragma comment (lib, "comctl32.lib")

const TCHAR g_szClassName[ ] = TEXT( "myWindowClass" );

#define IDC_MAIN_EDIT	101
#define IDC_MAIN_TOOL	102
#define IDC_MAIN_STATUS	103

BOOL LoadTextFileToEdit( HWND hEdit, PCTSTR pszFileName )
{
   HANDLE hFile;
   BOOL bSuccess = FALSE;

   hFile = CreateFile( pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
                       OPEN_EXISTING, 0, NULL );
   if ( hFile != INVALID_HANDLE_VALUE )
   {
      DWORD dwFileSize;

      dwFileSize = GetFileSize( hFile, NULL );
      if ( dwFileSize != 0xFFFFFFFF )
      {
         PTSTR pszFileText;

         pszFileText = ( PTSTR ) GlobalAlloc( GPTR, dwFileSize + 1 );
         if ( pszFileText != NULL )
         {
            DWORD dwRead;

            if ( ReadFile( hFile, pszFileText, dwFileSize, &dwRead, NULL ) )
            {
               pszFileText[dwFileSize] = 0; // Add null terminator
               if ( SetWindowText( hEdit, pszFileText ) )
                  bSuccess = TRUE; // It worked!
            }
            GlobalFree( pszFileText );
         }
      }
      CloseHandle( hFile );
   }
   return bSuccess;
}

BOOL SaveTextFileFromEdit( HWND hEdit, LPCTSTR pszFileName )
{
   HANDLE hFile;
   BOOL   bSuccess = FALSE;

   hFile = CreateFile( pszFileName, GENERIC_WRITE, 0, NULL,
                       CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
   if ( hFile != INVALID_HANDLE_VALUE )
   {
      DWORD dwTextLength;

      dwTextLength = GetWindowTextLength( hEdit );
      // No need to bother if there's no text.
      if ( dwTextLength > 0 )
      {
         PTSTR pszText;
         DWORD dwBufferSize = dwTextLength + 1;

         pszText = ( PTSTR ) GlobalAlloc( GPTR, dwBufferSize );
         if ( pszText != NULL )
         {
            if ( GetWindowText( hEdit, pszText, dwBufferSize ) )
            {
               DWORD dwWritten;

               if ( WriteFile( hFile, pszText, dwTextLength, &dwWritten, NULL ) )
                  bSuccess = TRUE;
            }
            GlobalFree( pszText );
         }
      }
      CloseHandle( hFile );
   }
   return bSuccess;
}

void DoFileOpen( HWND hwnd )
{
   OPENFILENAME ofn;
   TCHAR        szFileName[MAX_PATH] = TEXT( "" );

   ZeroMemory( &ofn, sizeof( ofn ) );

   ofn.lStructSize = sizeof( ofn );
   ofn.hwndOwner   = hwnd;
   ofn.lpstrFilter = TEXT( "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0" );
   ofn.lpstrFile   = szFileName;
   ofn.nMaxFile    = MAX_PATH;
   ofn.Flags       = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
   ofn.lpstrDefExt = TEXT( "txt" );

   if ( GetOpenFileName( &ofn ) )
   {
      HWND hEdit = GetDlgItem( hwnd, IDC_MAIN_EDIT );
      if ( LoadTextFileToEdit( hEdit, szFileName ) )
      {
         SendDlgItemMessage( hwnd, IDC_MAIN_STATUS, SB_SETTEXT, 0, ( LPARAM ) TEXT( "Opened..." ) );
         SendDlgItemMessage( hwnd, IDC_MAIN_STATUS, SB_SETTEXT, 1, ( LPARAM ) szFileName );
      }
   }
}

void DoFileSave( HWND hwnd )
{
   OPENFILENAME ofn;
   TCHAR        szFileName[MAX_PATH] = TEXT( "" );

   ZeroMemory( &ofn, sizeof( ofn ) );

   ofn.lStructSize = sizeof( ofn );
   ofn.hwndOwner   = hwnd;
   ofn.lpstrFilter = TEXT( "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0" );
   ofn.lpstrFile   = szFileName;
   ofn.nMaxFile    = MAX_PATH;
   ofn.lpstrDefExt = TEXT( "txt" );
   ofn.Flags       = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

   if ( GetSaveFileName( &ofn ) )
   {
      HWND hEdit = GetDlgItem( hwnd, IDC_MAIN_EDIT );
      if ( SaveTextFileFromEdit( hEdit, szFileName ) )
      {
         SendDlgItemMessage( hwnd, IDC_MAIN_STATUS, SB_SETTEXT, 0, ( LPARAM ) TEXT( "Saved..." ) );
         SendDlgItemMessage( hwnd, IDC_MAIN_STATUS, SB_SETTEXT, 1, ( LPARAM ) szFileName );
      }
   }
}

LRESULT CALLBACK WndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
   switch ( msg )
   {
   case WM_CREATE:
   {
      HFONT hfDefault;
      HWND hEdit;

      HWND hTool;
      TBBUTTON tbb[3];
      TBADDBITMAP tbab;

      HWND hStatus;
      int statwidths[ ] = { 100, -1 };

      // Create Edit Control

      hEdit = CreateWindowEx( WS_EX_CLIENTEDGE, TEXT( "EDIT" ), TEXT( "" ),
                              WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
                              0, 0, 100, 100, hwnd, ( HMENU ) IDC_MAIN_EDIT, GetModuleHandle( NULL ), NULL );
      if ( hEdit == NULL )
         MessageBox( hwnd, TEXT( "Could not create edit box." ), TEXT( "Error" ), MB_OK | MB_ICONERROR );

      hfDefault = ( HFONT ) GetStockObject( DEFAULT_GUI_FONT );
      SendMessage( hEdit, WM_SETFONT, ( WPARAM ) hfDefault, MAKELPARAM( FALSE, 0 ) );

      // Create Toolbar

      hTool = CreateWindowEx( 0, TOOLBARCLASSNAME, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0,
                              hwnd, ( HMENU ) IDC_MAIN_TOOL, GetModuleHandle( NULL ), NULL );
      if ( hTool == NULL )
         MessageBox( hwnd, TEXT( "Could not create tool bar." ), TEXT( "Error" ), MB_OK | MB_ICONERROR );

      // Send the TB_BUTTONSTRUCTSIZE message, which is required for
      // backward compatibility.
      SendMessage( hTool, TB_BUTTONSTRUCTSIZE, ( WPARAM ) sizeof( TBBUTTON ), 0 );

      tbab.hInst = HINST_COMMCTRL;
      tbab.nID = IDB_STD_SMALL_COLOR;
      SendMessage( hTool, TB_ADDBITMAP, 0, ( LPARAM ) &tbab );

      ZeroMemory( tbb, sizeof( tbb ) );
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

      SendMessage( hTool, TB_ADDBUTTONS, sizeof( tbb ) / sizeof( TBBUTTON ), ( LPARAM ) &tbb );

      // Create Status bar
      hStatus = CreateWindowEx( 0, STATUSCLASSNAME, NULL,
                                WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0,
                                hwnd, ( HMENU ) IDC_MAIN_STATUS, GetModuleHandle( NULL ), NULL );

      SendMessage( hStatus, SB_SETPARTS, sizeof( statwidths ) / sizeof( int ), ( LPARAM ) statwidths );
      SendMessage( hStatus, SB_SETTEXT, 0, ( LPARAM ) TEXT( "Hi there :)" ) );
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
      hTool = GetDlgItem( hwnd, IDC_MAIN_TOOL );
      SendMessage( hTool, TB_AUTOSIZE, 0, 0 );

      GetWindowRect( hTool, &rcTool );
      iToolHeight = rcTool.bottom - rcTool.top;

      // Size status bar and get height
      hStatus = GetDlgItem( hwnd, IDC_MAIN_STATUS );
      SendMessage( hStatus, WM_SIZE, 0, 0 );

      GetWindowRect( hStatus, &rcStatus );
      iStatusHeight = rcStatus.bottom - rcStatus.top;

      // Calculate remaining height and size edit
      GetClientRect( hwnd, &rcClient );

      iEditHeight = rcClient.bottom - iToolHeight - iStatusHeight;

      hEdit = GetDlgItem( hwnd, IDC_MAIN_EDIT );
      SetWindowPos( hEdit, NULL, 0, iToolHeight, rcClient.right, iEditHeight, SWP_NOZORDER );
   }
   break;
   case WM_CLOSE:
      DestroyWindow( hwnd );
      break;
   case WM_DESTROY:
      PostQuitMessage( 0 );
      break;
   case WM_COMMAND:
      switch ( LOWORD( wParam ) )
      {
      case ID_FILE_EXIT:
         PostMessage( hwnd, WM_CLOSE, 0, 0 );
         break;
      case ID_FILE_NEW:
         SetDlgItemText( hwnd, IDC_MAIN_EDIT, TEXT( "" ) );
         break;
      case ID_FILE_OPEN:
         DoFileOpen( hwnd );
         break;
      case ID_FILE_SAVEAS:
         DoFileSave( hwnd );
         break;
      }
      break;
   default:
      return DefWindowProc( hwnd, msg, wParam, lParam );
   }
   return 0;
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine, int nCmdShow )
{
   WNDCLASSEX wc;
   HWND       hwnd;
   MSG        Msg;

   InitCommonControls( );

   wc.cbSize        = sizeof( WNDCLASSEX );
   wc.style         = 0;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = hInstance;
   wc.hIcon         = LoadIcon( NULL, IDI_APPLICATION );
   wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
   wc.hbrBackground = ( HBRUSH ) ( COLOR_WINDOW + 1 );
   wc.lpszMenuName  = MAKEINTRESOURCE( IDR_MAINMENU );
   wc.lpszClassName = g_szClassName;
   wc.hIconSm       = LoadIcon( NULL, IDI_APPLICATION );

   if ( !RegisterClassEx( &wc ) )
   {
      MessageBox( NULL, TEXT( "Window Registration Failed!" ), TEXT( "Error!" ),
                  MB_ICONEXCLAMATION | MB_OK );
      return 0;
   }

   hwnd = CreateWindowEx(
      0,
      g_szClassName,
      TEXT( "theForger's Tutorial Application" ),
      WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
      CW_USEDEFAULT, CW_USEDEFAULT, 480, 320,
      NULL, NULL, hInstance, NULL );

   if ( hwnd == NULL )
   {
      MessageBox( NULL, TEXT( "Window Creation Failed!" ), TEXT( "Error!" ),
                  MB_ICONEXCLAMATION | MB_OK );
      return 0;
   }

   ShowWindow( hwnd, nCmdShow );
   UpdateWindow( hwnd );

   while ( GetMessage( &Msg, NULL, 0, 0 ) > 0 )
   {
      TranslateMessage( &Msg );
      DispatchMessage( &Msg );
   }
   return Msg.wParam;
}
