#include <windows.h>

#include "resource.h"

BOOL CALLBACK DlgProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch ( msg )
   {
   case WM_INITDIALOG:
      // This is where we set up the dialog box, and initialise any default values

      SetDlgItemTextW(wnd, IDC_TEXT, L"This is a string");
      SetDlgItemInt(wnd, IDC_NUMBER, 5, FALSE);
      break;
   case WM_COMMAND:
      switch ( LOWORD(wParam) )
      {
      case IDC_ADD:
      {
         // When somebody clicks the Add button, first we get the number of
         // they entered
         BOOL bSuccess;
         int  times = GetDlgItemInt(wnd, IDC_NUMBER, &bSuccess, FALSE);

         if ( bSuccess )
         {
            // Then we get the string they entered
            // First we need to find out how long it is so that we can
            // allocate some memory
            int len = GetWindowTextLengthW(GetDlgItem(wnd, IDC_TEXT));

            if ( len > 0 )
            {
               // Now we allocate, and get the string into our buffer
               int    i;
               WCHAR* buf;

               buf = (WCHAR*) GlobalAlloc(GPTR, len + 1);
               GetDlgItemTextW(wnd, IDC_TEXT, buf, len + 1);

               // Now we add the string to the list box however many times
               // the user asked us to.
               for ( i = 0; i < times; i++ )
               {
                  int index = (int) SendDlgItemMessageW(wnd, IDC_LIST, LB_ADDSTRING, 0, (LPARAM) buf);

                  // Here we are associating the value nTimes with the item
                  // just for the heck of it, we'll use it to display later.
                  // Normally you would put some more useful data here, such
                  // as a pointer.
                  SendDlgItemMessageW(wnd, IDC_LIST, LB_SETITEMDATA, (WPARAM) index, (LPARAM) times);
               }

               // Dont' forget to free the memory!
               GlobalFree((HANDLE) buf);

               // let's update the text message of the number of added items
               // it's kinda stupid to not update the message when adding strings
               SetDlgItemInt(wnd, IDC_SHOWCOUNT, times, FALSE);
            }
            else
            {
               MessageBoxW(wnd, L"You didn't enter anything!", L"Warning", MB_OK);
            }
         }
         else
         {
            MessageBoxW(wnd, L"Couldn't translate that number :(", L"Warning", MB_OK);
         }

      }
      break;

      case IDC_REMOVE:
      {
         // When the user clicks the Remove button, we first get the number
         // of selected items
         HWND list  = GetDlgItem(wnd, IDC_LIST);
         int  count = (int) SendMessageW(list, LB_GETSELCOUNT, 0, 0);

         if ( count != LB_ERR )
         {
            if ( count != 0 )
            {
               // And then allocate room to store the list of selected items.
               int  i;
               int* buf = (int*) GlobalAlloc(GPTR, sizeof(int) * count);

               SendMessageW(list, LB_GETSELITEMS, (WPARAM) count, (LPARAM) buf);

               // Now we loop through the list and remove each item that was
               // selected.

               // WARNING!!!
               // We loop backwards, because if we removed items
               // from top to bottom, it would change the indexes of the other
               // items!!!

               for ( i = count - 1; i >= 0; i-- )
               {
                  SendMessageW(list, LB_DELETESTRING, (WPARAM) buf[i], 0);
               }

               GlobalFree(buf);
            }
            else
            {
               MessageBoxW(wnd, L"No items selected.", L"Warning", MB_OK);
            }
         }
         else
         {
            MessageBoxW(wnd, L"Error counting items :(", L"Warning", MB_OK);
         }
      }
      break;

      case IDC_CLEAR:
         SendDlgItemMessageW(wnd, IDC_LIST, LB_RESETCONTENT, 0, 0);
         break;

      case IDC_LIST:
         switch ( HIWORD(wParam) )
         {
         case LBN_SELCHANGE:
         {
            // Get the number of items selected.
            HWND list  = GetDlgItem(wnd, IDC_LIST);
            int  count = (int) SendMessageW(list, LB_GETSELCOUNT, 0, 0);

            if ( count != LB_ERR )
            {
               // We only want to continue if one and only one item is
               // selected.
               if ( count == 1 )
               {
                  // Since we know ahead of time we're only getting one
                  // index, there's no need to allocate an array.

                  int index;
                  int err = (int) SendMessageW(list, LB_GETSELITEMS, (WPARAM) 1, (LPARAM) &index);

                  if ( err != LB_ERR )
                  {
                     // Get the data we associated with the item above
                     // (the number of times it was added)
                     int data = (int) SendMessageW(list, LB_GETITEMDATA, (WPARAM) index, 0);

                     SetDlgItemInt(wnd, IDC_SHOWCOUNT, data, FALSE);
                  }
                  else
                  {
                     MessageBoxW(wnd, L"Error getting selected item :(", L"Warning", MB_OK);
                  }
               }
               else
               {
                  // No items selected, or more than one
                  // Either way, we aren't going to process this.
                  SetDlgItemTextW(wnd, IDC_SHOWCOUNT, L"-");
               }
            }
            else
            {
               MessageBoxW(wnd, L"Error counting items :(", L"Warning", MB_OK);
            }
         }
         break;
         }
         break;
      }
      break;

   case WM_CLOSE:
      EndDialog(wnd, 0);
      break;

   default:
      return FALSE;
   }
   return TRUE;
}

int WINAPI wWinMain(_In_     HINSTANCE inst,
                    _In_opt_ HINSTANCE prevInst,
                    _In_     PWSTR     cmdLine,
                    _In_     int       cmdShow)
{
   return (int) DialogBoxParamW(inst, MAKEINTRESOURCE(IDD_MAIN), NULL, (DLGPROC) DlgProc, 0L);
}
