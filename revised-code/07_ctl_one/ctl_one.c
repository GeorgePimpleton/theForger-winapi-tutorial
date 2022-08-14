#include <windows.h>

#include "resource.h"

BOOL CALLBACK DlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
   switch (Message)
   {
   case WM_INITDIALOG:
      // This is where we set up the dialog box, and initialise any default values

      SetDlgItemTextW(hwnd, IDC_TEXT, L"This is a string");
      SetDlgItemInt(hwnd, IDC_NUMBER, 5, FALSE);
      break;

   case WM_COMMAND:
      switch (LOWORD(wParam))
      {
      case IDC_ADD:
      {
         // When somebody clicks the Add button, first we get the number of
         // they entered

         BOOL bSuccess;
         int nTimes = GetDlgItemInt(hwnd, IDC_NUMBER, &bSuccess, FALSE);
         if (bSuccess)
         {
            // Then we get the string they entered
            // First we need to find out how long it is so that we can
            // allocate some memory

            int len = GetWindowTextLengthW(GetDlgItem(hwnd, IDC_TEXT));
            if (len > 0)
            {
               // Now we allocate, and get the string into our buffer

               int    i;
               WCHAR* buf;

            #pragma warning (disable : 28183)
               buf = (WCHAR*) GlobalAlloc(GPTR, len + 1);
            #pragma warning (disable : 6386)
               GetDlgItemTextW(hwnd, IDC_TEXT, buf, len + 1);

               // Now we add the string to the list box however many times
               // the user asked us to.

               for (i = 0; i < nTimes; i++)
               {
                  int index = (int) SendDlgItemMessageW(hwnd, IDC_LIST, LB_ADDSTRING, 0, (LPARAM) buf);

                  // Here we are associating the value nTimes with the item
                  // just for the heck of it, we'll use it to display later.
                  // Normally you would put some more useful data here, such
                  // as a pointer.
                  SendDlgItemMessageW(hwnd, IDC_LIST, LB_SETITEMDATA, (WPARAM) index, (LPARAM) nTimes);
               }

               // Dont' forget to free the memory!
               GlobalFree((HANDLE) buf);
            }
            else
            {
               MessageBoxW(hwnd, L"You didn't enter anything!", L"Warning", MB_OK);
            }
         }
         else
         {
            MessageBoxW(hwnd, L"Couldn't translate that number :(", L"Warning", MB_OK);
         }

      }
      break;

      case IDC_REMOVE:
      {
         // When the user clicks the Remove button, we first get the number
         // of selected items

         HWND hList = GetDlgItem(hwnd, IDC_LIST);
         int count = (int) SendMessageW(hList, LB_GETSELCOUNT, 0, 0);
         if (count != LB_ERR)
         {
            if (count != 0)
            {
               // And then allocate room to store the list of selected items.

               int  i;
               int* buf = (int*) GlobalAlloc(GPTR, sizeof(int) * count);
               SendMessageW(hList, LB_GETSELITEMS, (WPARAM) count, (LPARAM) buf);

               // Now we loop through the list and remove each item that was
               // selected.

               // WARNING!!!
               // We loop backwards, because if we removed items
               // from top to bottom, it would change the indexes of the other
               // items!!!

               for (i = count - 1; i >= 0; i--)
               {
               #pragma warning (disable : 28182)

                  SendMessageW(hList, LB_DELETESTRING, (WPARAM) buf[i], 0);
               }

               GlobalFree(buf);
            }
            else
            {
               MessageBoxW(hwnd, L"No items selected.", L"Warning", MB_OK);
            }
         }
         else
         {
            MessageBoxW(hwnd, L"Error counting items :(", L"Warning", MB_OK);
         }
      }
      break;

      case IDC_CLEAR:
         SendDlgItemMessageW(hwnd, IDC_LIST, LB_RESETCONTENT, 0, 0);
         break;

      case IDC_LIST:
         switch (HIWORD(wParam))
         {
         case LBN_SELCHANGE:
         {
            // Get the number of items selected.

            HWND hList = GetDlgItem(hwnd, IDC_LIST);
            int count  = (int) SendMessageW(hList, LB_GETSELCOUNT, 0, 0);
            if (count != LB_ERR)
            {
               // We only want to continue if one and only one item is
               // selected.

               if (count == 1)
               {
                  // Since we know ahead of time we're only getting one
                  // index, there's no need to allocate an array.

                  int index;
                  int err = (int) SendMessageW(hList, LB_GETSELITEMS, (WPARAM) 1, (LPARAM) &index);
                  if (err != LB_ERR)
                  {
                     // Get the data we associated with the item above
                     // (the number of times it was added)

                     int data = (int) SendMessageW(hList, LB_GETITEMDATA, (WPARAM) index, 0);

                     SetDlgItemInt(hwnd, IDC_SHOWCOUNT, data, FALSE);
                  }
                  else
                  {
                     MessageBoxW(hwnd, L"Error getting selected item :(", L"Warning", MB_OK);
                  }
               }
               else
               {
                  // No items selected, or more than one
                  // Either way, we aren't going to process this.
                  SetDlgItemTextW(hwnd, IDC_SHOWCOUNT, L"-");
               }
            }
            else
            {
               MessageBoxW(hwnd, L"Error counting items :(", L"Warning", MB_OK);
            }
         }
         break;
         }
         break;
      }
      break;

   case WM_CLOSE:
      EndDialog(hwnd, 0);
      break;

   default:
      return FALSE;
   }
   return TRUE;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                    _In_ PWSTR lpCmdLine, _In_ int nCmdShow)
{
   return (int) DialogBoxW(hInstance, MAKEINTRESOURCEW(IDD_MAIN), NULL, (DLGPROC) DlgProc);
}
