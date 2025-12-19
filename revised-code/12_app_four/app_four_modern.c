#include <windows.h>
#include <commctrl.h>
#include "resource.h"

#pragma comment (lib, "comctl32.lib")

PCWSTR g_className      = L"myWindowClass";
PCWSTR g_childClassName = L"myMDIChildWindowClass";

#define IDC_MAIN_MDI    101
#define IDC_MAIN_TOOL   102
#define IDC_MAIN_STATUS 103

#define IDC_CHILD_EDIT 101

#define ID_MDI_FIRSTCHILD 50000

HWND g_MDIClient  = NULL;
HWND g_mainWindow = NULL;

BOOL LoadTextFileToEdit( HWND, PCWSTR );
BOOL SaveTextFileFromEdit( HWND, PCWSTR fileName );
void DoFileOpen( HWND );
void DoFileSave( HWND );
HWND CreateNewMDIChild( HWND );
LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK MDIChildWndProc( HWND, UINT, WPARAM, LPARAM );
BOOL SetUpMDIChildWindowClass( HINSTANCE );

int WINAPI wWinMain( _In_     HINSTANCE inst,
                     _In_opt_ HINSTANCE prevInst,
                     _In_     PWSTR    cmdLine,
                     _In_     int      cmdShow )
{
   UNREFERENCED_PARAMETER( prevInst );
   UNREFERENCED_PARAMETER( cmdLine );

   WNDCLASSEXW wc = { 0 };
   HWND        wnd;
   MSG         msg;

   InitCommonControls( );

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
   wc.lpszClassName = g_className;

   if ( !RegisterClassExW( &wc ) )
   {
      MessageBoxW( NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK );
      return 0;
   }

   if ( !SetUpMDIChildWindowClass( inst ) )
   {
      return 0;
   }

   wnd = CreateWindowExW( 0,
                          g_className,
                          L"theForger's Tutorial Application",
                          WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                          CW_USEDEFAULT, CW_USEDEFAULT, 480, 320,
                          NULL, NULL, inst, NULL );

   if ( wnd == NULL )
   {
      MessageBoxW( NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK );
      return 0;
   }

   g_mainWindow = wnd;

   ShowWindow( wnd, cmdShow );
   UpdateWindow( wnd );

   while ( GetMessageW( &msg, NULL, 0, 0 ) > 0 )
   {
      if ( !TranslateMDISysAccel( g_MDIClient, &msg ) )
      {
         TranslateMessage( &msg );
         DispatchMessageW( &msg );
      }
   }

   return ( int ) msg.wParam;
}

BOOL LoadTextFileToEdit( HWND edit, PCWSTR fileName )
{
   HANDLE file;
   BOOL   success = FALSE;

   file = CreateFileW( fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );

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

BOOL SaveTextFileFromEdit( HWND edit, PCWSTR fileName )
{
   HANDLE file;
   BOOL   success = FALSE;

   file = CreateFileW( fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

   if ( file != INVALID_HANDLE_VALUE )
   {
      DWORD textLength;

      textLength = GetWindowTextLengthW( edit );

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
   OPENFILENAMEW ofn;
   WCHAR         fileName[MAX_PATH] = L"";

   ZeroMemory( &ofn, sizeof( ofn ) );

   ofn.lStructSize = sizeof( ofn );
   ofn.hwndOwner   = wnd;
   ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
   ofn.lpstrFile   = fileName;
   ofn.nMaxFile    = MAX_PATH;
   ofn.Flags       = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
   ofn.lpstrDefExt = L"txt";

   if ( GetOpenFileNameW( &ofn ) )
   {
      HWND edit = GetDlgItem( wnd, IDC_CHILD_EDIT );

      if ( LoadTextFileToEdit( edit, fileName ) )
      {
         SendDlgItemMessageW( g_mainWindow, IDC_MAIN_STATUS, SB_SETTEXT, 0, ( LPARAM ) L"Opened..." );
         SendDlgItemMessageW( g_mainWindow, IDC_MAIN_STATUS, SB_SETTEXT, 1, ( LPARAM ) fileName );

         SetWindowTextW( wnd, fileName );
      }
   }
}

void DoFileSave( HWND wnd )
{
   OPENFILENAMEW ofn;
   WCHAR         fileName[MAX_PATH] = L"";

   ZeroMemory( &ofn, sizeof( ofn ) );

   ofn.lStructSize = sizeof( ofn );
   ofn.hwndOwner   = wnd;
   ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
   ofn.lpstrFile   = fileName;
   ofn.nMaxFile    = MAX_PATH;
   ofn.lpstrDefExt = L"txt";
   ofn.Flags       = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

   if ( GetSaveFileNameW( &ofn ) )
   {
      HWND edit = GetDlgItem( wnd, IDC_CHILD_EDIT );

      if ( SaveTextFileFromEdit( edit, fileName ) )
      {
         SendDlgItemMessageW( g_mainWindow, IDC_MAIN_STATUS, SB_SETTEXT, 0, ( LPARAM ) L"Saved..." );
         SendDlgItemMessageW( g_mainWindow, IDC_MAIN_STATUS, SB_SETTEXT, 1, ( LPARAM ) fileName );

         SetWindowTextW( wnd, fileName );
      }
   }
}

HWND CreateNewMDIChild( HWND MDIClient )
{
   MDICREATESTRUCTW mcs;
   HWND             child;

   mcs.szTitle = L"[Untitled]";
   mcs.szClass = g_childClassName;
   mcs.hOwner  = GetModuleHandleW( NULL );
   mcs.x       = mcs.cx = CW_USEDEFAULT;
   mcs.y       = mcs.cy = CW_USEDEFAULT;
   mcs.style   = MDIS_ALLCHILDSTYLES;

   child = ( HWND ) SendMessageW( MDIClient, WM_MDICREATE, 0, ( LONGLONG ) &mcs );

   if ( !child )
   {
      MessageBoxW( MDIClient, L"MDI Child creation failed.", L"Oh Oh...", MB_ICONEXCLAMATION | MB_OK );
   }
   return child;
}

LRESULT CALLBACK WndProc( HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
   switch ( msg )
   {
   case WM_CREATE:
   {
      HWND        tool;
      TBBUTTON    tbb[3];
      TBADDBITMAP tbab;

      HWND status;
      int  statwidths[ ] = { 100, -1 };

      CLIENTCREATESTRUCT ccs;

      // Create MDI Client

      // Find window menu where children will be listed
      ccs.hWindowMenu  = GetSubMenu( GetMenu( wnd ), 2 );
      ccs.idFirstChild = ID_MDI_FIRSTCHILD;

      g_MDIClient = CreateWindowExW( WS_EX_CLIENTEDGE, L"mdiclient", NULL,
                                     WS_CHILD | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL | WS_VISIBLE,
                                     CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                     wnd, ( HMENU ) IDC_MAIN_MDI, GetModuleHandleW( NULL ), ( LPVOID ) &ccs );

      if ( g_MDIClient == NULL )
      {
         MessageBoxW( wnd, L"Could not create MDI client.", L"Error", MB_OK | MB_ICONERROR );
      }

      // Create Toolbar

      tool = CreateWindowExW( 0, TOOLBARCLASSNAMEW, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0,
                              wnd, ( HMENU ) IDC_MAIN_TOOL, GetModuleHandleW( NULL ), NULL );

      if ( tool == NULL )
      {
         MessageBoxW( wnd, L"Could not create tool bar.", L"Error", MB_OK | MB_ICONERROR );
      }

      // Send the TB_BUTTONSTRUCTSIZE message, which is required for
      // backward compatibility.
   #pragma warning (disable : 6387)
      SendMessageW( tool, TB_BUTTONSTRUCTSIZE, ( WPARAM ) sizeof( TBBUTTON ), 0 );

      tbab.hInst = HINST_COMMCTRL;
      tbab.nID   = IDB_STD_SMALL_COLOR;
      SendMessageW( tool, TB_ADDBITMAP, 0, ( LPARAM ) &tbab );

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

      SendMessageW( tool, TB_ADDBUTTONS, sizeof( tbb ) / sizeof( TBBUTTON ), ( LPARAM ) &tbb );

      // Create Status bar

      status = CreateWindowExW( 0, STATUSCLASSNAMEW, NULL,
                                WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0,
                                wnd, ( HMENU ) IDC_MAIN_STATUS, GetModuleHandleW( NULL ), NULL );

      SendMessageW( status, SB_SETPARTS, sizeof( statwidths ) / sizeof( int ), ( LPARAM ) statwidths );
      SendMessageW( status, SB_SETTEXT, 0, ( LPARAM ) L"Hi there :)" );
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

      HWND MDI;
      int  MDIHeight;
      RECT client;

      // Size toolbar and get height

      tool = GetDlgItem( wnd, IDC_MAIN_TOOL );
      SendMessageW( tool, TB_AUTOSIZE, 0, 0 );

      GetWindowRect( tool, &toolRect );
      toolHeight = toolRect.bottom - toolRect.top;

      // Size status bar and get height

      status = GetDlgItem( wnd, IDC_MAIN_STATUS );
      SendMessageW( status, WM_SIZE, 0, 0 );

      GetWindowRect( status, &statusRect );
      statusHeight = statusRect.bottom - statusRect.top;

      // Calculate remaining height and size edit

      GetClientRect( wnd, &client );

      MDIHeight = client.bottom - toolHeight - statusHeight;

      MDI = GetDlgItem( wnd, IDC_MAIN_MDI );
      SetWindowPos( MDI, NULL, 0, toolHeight, client.right, MDIHeight, SWP_NOZORDER );
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
         CreateNewMDIChild( g_MDIClient );
         break;

      case ID_FILE_OPEN:
      {
         HWND hChild = CreateNewMDIChild( g_MDIClient );

         if ( hChild )
         {
            DoFileOpen( hChild );
         }
      }
      break;

      case ID_FILE_CLOSE:
      {
         HWND hChild = ( HWND ) SendMessageW( g_MDIClient, WM_MDIGETACTIVE, 0, 0 );

         if ( hChild )
         {
            SendMessageW( hChild, WM_CLOSE, 0, 0 );
         }
      }
      break;

      case ID_WINDOW_TILE:
         SendMessageW( g_MDIClient, WM_MDITILE, 0, 0 );
         break;

      case ID_WINDOW_CASCADE:
         SendMessageW( g_MDIClient, WM_MDICASCADE, 0, 0 );
         break;

      default:
      {
         if ( LOWORD( wParam ) >= ID_MDI_FIRSTCHILD )
         {
            DefFrameProcW( wnd, g_MDIClient, WM_COMMAND, wParam, lParam );
         }
         else
         {
            HWND hChild = ( HWND ) SendMessageW( g_MDIClient, WM_MDIGETACTIVE, 0, 0 );

            if ( hChild )
            {
               SendMessageW( hChild, WM_COMMAND, wParam, lParam );
            }
         }
      }
      }
      break;
   }

   return DefFrameProcW( wnd, g_MDIClient, msg, wParam, lParam );
}

LRESULT CALLBACK MDIChildWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
   switch ( msg )
   {
   case WM_CREATE:
   {
      HFONT fontDefault;
      HWND  edit;

      // Create Edit Control

      edit = CreateWindowExW( WS_EX_CLIENTEDGE, L"EDIT", L"",
                              WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
                              0, 0, 100, 100, hwnd, ( HMENU ) IDC_CHILD_EDIT, GetModuleHandleW( NULL ), NULL );

      if ( edit == NULL )
      {
         MessageBoxW( hwnd, L"Could not create edit box.", L"Error", MB_OK | MB_ICONERROR );
      }

      fontDefault = ( HFONT ) GetStockObject( DEFAULT_GUI_FONT );
      SendMessageW( edit, WM_SETFONT, ( WPARAM ) fontDefault, MAKELPARAM( FALSE, 0 ) );
   }
   break;

   case WM_MDIACTIVATE:
   {
      HMENU menu;
      HMENU fileMenu;
      UINT  enableFlag;

      menu = GetMenu( g_mainWindow );

      if ( hwnd == ( HWND ) lParam )
      {
         //being activated, enable the menus
         enableFlag = MF_ENABLED;
      }
      else
      {
         //being de-activated, gray the menus
         enableFlag = MF_GRAYED;
      }

      EnableMenuItem( menu, 1, MF_BYPOSITION | enableFlag );
      EnableMenuItem( menu, 2, MF_BYPOSITION | enableFlag );

      fileMenu = GetSubMenu( menu, 0 );
      EnableMenuItem( fileMenu, ID_FILE_SAVEAS, MF_BYCOMMAND | enableFlag );

      EnableMenuItem( fileMenu, ID_FILE_CLOSE, MF_BYCOMMAND | enableFlag );
      EnableMenuItem( fileMenu, ID_FILE_CLOSEALL, MF_BYCOMMAND | enableFlag );

      DrawMenuBar( g_mainWindow );
   }
   break;

   case WM_COMMAND:
      switch ( LOWORD( wParam ) )
      {
      case ID_FILE_OPEN:
         DoFileOpen( hwnd );
         break;

      case ID_FILE_SAVEAS:
         DoFileSave( hwnd );
         break;

      case ID_EDIT_CUT:
         SendDlgItemMessageW( hwnd, IDC_CHILD_EDIT, WM_CUT, 0, 0 );
         break;

      case ID_EDIT_COPY:
         SendDlgItemMessageW( hwnd, IDC_CHILD_EDIT, WM_COPY, 0, 0 );
         break;

      case ID_EDIT_PASTE:
         SendDlgItemMessageW( hwnd, IDC_CHILD_EDIT, WM_PASTE, 0, 0 );
         break;
      }
      break;

   case WM_SIZE:
   {
      HWND edit;
      RECT client;

      // Calculate remaining height and size edit

      GetClientRect( hwnd, &client );

      edit = GetDlgItem( hwnd, IDC_CHILD_EDIT );
      SetWindowPos( edit, NULL, 0, 0, client.right, client.bottom, SWP_NOZORDER );
   }
   return DefMDIChildProcW( hwnd, msg, wParam, lParam );
   }

   return DefMDIChildProcW( hwnd, msg, wParam, lParam );
}

BOOL SetUpMDIChildWindowClass( HINSTANCE inst )
{
   WNDCLASSEXW wc = { 0 };

   wc.cbSize        = sizeof( WNDCLASSEXW );
   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = MDIChildWndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = inst;
   wc.hIcon         = ( HICON ) LoadImageW( NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR );
   wc.hIconSm       = ( HICON ) LoadImageW( NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR );
   wc.hCursor       = ( HCURSOR ) LoadImageW( NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED );
   wc.hbrBackground = ( HBRUSH ) ( COLOR_3DFACE + 1 );
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = g_childClassName;

   if ( !RegisterClassExW( &wc ) )
   {
      MessageBoxW( 0, L"Could Not Register Child Window", L"Oh Oh...", MB_ICONEXCLAMATION | MB_OK );
      return FALSE;
   }
   else
      return TRUE;
}
