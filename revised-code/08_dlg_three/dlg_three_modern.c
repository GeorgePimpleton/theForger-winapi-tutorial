#include <windows.h>
#include "resource.h"

BOOL CALLBACK DlgProc( HWND, UINT, WPARAM, LPARAM );

int WINAPI wWinMain( _In_     HINSTANCE inst,
                     _In_opt_ HINSTANCE prevInst,
                     _In_     PWSTR     cmdLine,
                     _In_     int       cmdShow )
{
   UNREFERENCED_PARAMETER( prevInst );
   UNREFERENCED_PARAMETER( cmdLine );
   UNREFERENCED_PARAMETER( cmdShow );

   return ( int ) DialogBoxParamW( inst, MAKEINTRESOURCEW( IDD_MAIN ), NULL, ( DLGPROC ) DlgProc, 0L );
}
BOOL CALLBACK DlgProc( HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
   static HBRUSH g_background = NULL;

   switch ( msg )
   {
   case WM_INITDIALOG:
      g_background = CreateSolidBrush( RGB( 55, 55, 55 ) );

      SendMessageW( wnd, WM_SETICON, ICON_SMALL, ( LPARAM ) LoadIconW( NULL, MAKEINTRESOURCEW( IDI_APPLICATION ) ) );
      SendMessageW( wnd, WM_SETICON, ICON_BIG, ( LPARAM ) LoadIconW( NULL, MAKEINTRESOURCEW( IDI_APPLICATION ) ) );
      break;

   case WM_CLOSE:
      EndDialog( wnd, 0 );
      break;

   case WM_CTLCOLORDLG:
      return ( BOOL ) ( LONG_PTR ) g_background;

   case WM_CTLCOLORSTATIC:
   {
      HDC dcStatic = ( HDC ) wParam;

      SetTextColor( dcStatic, RGB( 255, 255, 255 ) );
      SetBkMode( dcStatic, TRANSPARENT );

      return ( BOOL ) ( LONG_PTR ) g_background;
   }
   break;

   case WM_COMMAND:
      switch ( LOWORD( wParam ) )
      {
      case IDOK:
         EndDialog( wnd, 0 );
         break;
      }
      break;

   case WM_DESTROY:
      DeleteObject( g_background );
      break;

   default:
      return FALSE;
   }
   return TRUE;
}
