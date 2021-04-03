// FileCabinet.cpp : Defines the entry point for the application.
// https://docs.microsoft.com/en-us/windows/win32/controls/create-a-tree-view-control

#include "stdafx.h"
#include "FileCabinet.h"
#include "commctrl.h"
#include "../lib/tinyxml/tinyxml.h"
#include <string>
#include <vector>
#include <windowsx.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
TCHAR szCardClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance, TCHAR *szWC, WNDPROC wndproc);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	CardWndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Input(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	CardInput(HWND, UINT, WPARAM, LPARAM);
void				CreateControls(HWND hWnd);
void				FillCabinet();
void				showCard(LPNMTREEVIEW);
void				addDrawer();
void				addCard();
void				deleteItem();
TiXmlElement*		getXMLElemenItem(HTREEITEM hItem);
void				saveLastChange();
void				TreeViewContextMenu(LPARAM lParam);
void				createEmtpyCabinetXML();
HWND hMainWindow = NULL;
HWND _hCabinetsWindow = NULL;
HWND _hEditTextWindow = NULL;
TiXmlDocument _Cabinet;
int _DrawerClosed;
int _DrawerOpen;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	InitCommonControls();

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_FILECABINET, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance, szWindowClass, WndProc);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}
	FillCabinet();


	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FILECABINET));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance, TCHAR *szWC, WNDPROC wndproc)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= wndproc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FILECABINET));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_FILECABINET);
	wcex.lpszClassName	= szWC;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_FILECABINET));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

   hInst = hInstance; // Store instance handle in our global variable

   hMainWindow = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 600, 400, NULL, NULL, hInstance, NULL);


   if (!hMainWindow)
   {
      return FALSE;
   }

   ShowWindow(hMainWindow, nCmdShow);
   UpdateWindow(hMainWindow);

   CreateControls(hMainWindow);

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////
/*!
*/
BOOL InitTreeViewImageLists(HWND hwndTV)
{
	HIMAGELIST himl;  // handle to image list 
	HBITMAP hbmp;     // handle to bitmap 
	// Create the image list. 
	if ((himl = ImageList_Create(16,
		16,
		FALSE,
		4, 0)) == NULL)
		return FALSE;

	// Add the open file, closed file, and document bitmaps. 
	hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
	_DrawerClosed = ImageList_Add(himl, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_DRAWEROPEN));
	 _DrawerOpen = ImageList_Add(himl, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	
	hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAPCARDCLOSED));
	ImageList_Add(himl, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAPCARDOPEN));
	ImageList_Add(himl, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	// Fail if not all of the images were added. 
	if (ImageList_GetImageCount(himl) < 4)
		return FALSE;
		
	// Associate the image list with the tree-view control. 
	TreeView_SetImageList(hwndTV, himl, TVSIL_NORMAL);
	
	return TRUE;
}

HTREEITEM AddItemToTree(HWND hwndTV, LPTSTR lpszItem, HTREEITEM pParentItem, TiXmlElement *pCard)
{
	TVITEM tvi;
	TVINSERTSTRUCT tvins;
	HTREEITEM hPrev = (HTREEITEM)TVI_FIRST;
	HTREEITEM hti;
	if (pParentItem != NULL)
		hPrev = pParentItem;
	tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	//tvi.mask = TVIF_TEXT | TVIF_PARAM;

	// Set the text of the item. 
	tvi.pszText = lpszItem;
	tvi.cchTextMax = sizeof(tvi.pszText) / sizeof(tvi.pszText[0]);

	tvi.lParam = (LPARAM)pCard;
	tvins.item = tvi;
	tvins.hInsertAfter = TVI_LAST;

	// Set the parent item based on the specified level. 
	if (pParentItem == NULL)
	{
		tvins.hParent = TVI_ROOT;
		tvins.item.iImage = 0;
		tvins.item.iSelectedImage = 1;
	}
	else
	{
		tvins.hParent = pParentItem;
		tvins.item.iImage = 2;
		tvins.item.iSelectedImage = 3;
	}
	// Add the item to the tree-view control. 
	hPrev = (HTREEITEM)SendMessage(hwndTV, TVM_INSERTITEM,
		0, (LPARAM)(LPTVINSERTSTRUCT)&tvins);

	/*
	if (hPrev == NULL)
		return NULL;
		*/
	// The new item is a child item. Give the parent item a 
	// closed folder bitmap to indicate it now has child items. 
	if (hPrev == NULL)
	{
		hti = TreeView_GetParent(hwndTV, hPrev);
		tvi.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		tvi.hItem = hti;
		tvi.iImage = 0;//g_nClosed;
		tvi.iSelectedImage = 0;//g_nClosed;
		TreeView_SetItem(hwndTV, &tvi);
	}
	if (pParentItem != NULL)
		TreeView_SortChildren(_hCabinetsWindow, pParentItem, false);
	return hPrev;
}

//////////////////////////////////////////////////////////////////////////
/*!
*/
void CreateControls(HWND hWnd)
{
	RECT r;
	GetClientRect(hWnd, &r);
	int width = min(r.right * 0.3, 500);

	_hCabinetsWindow = CreateWindowEx(0,
		WC_TREEVIEW,
		TEXT("Tree View"),
		WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS,
		0,
		0,
		width,
		r.bottom-2,
		hWnd,
		(HMENU)4711,
		hInst,
		NULL);
	HTREEITEM p = NULL;

	// Initialize the image list, and add items to the control. 
	// InitTreeViewImageLists and InitTreeViewItems are application- 
	// defined functions, shown later. 
	
	if (!InitTreeViewImageLists(_hCabinetsWindow) )/*||
		!InitTreeViewItems(_hCabinetsWindow))*/
	{
		DestroyWindow(_hCabinetsWindow);
		_hCabinetsWindow = NULL;
	}
	


	_hEditTextWindow = CreateWindow("EDIT",   // predefined class 
		"Empty Card",      
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | WS_BORDER,  
		width+3,         // starting x position 
		0,         // starting y position 
		r.right - width-5,        // button width 
		r.bottom-2,        // button height 
		hWnd,       // parent window 
		(HMENU)4712,       // No menu 
		(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
		NULL);      // pointer not needed 


}

std::wstring getWstring(const char* Text)
{
	int cSize = (int)strlen(Text);
	size_t ReturnValue = 0;
	std::vector<wchar_t> tmp(cSize + 1);
	mbstowcs_s(&ReturnValue, tmp.data(), cSize + 1, Text, cSize);
	return tmp.data();
}


void FillCabinet()
{
	if (!_Cabinet.LoadFile("FileCabinet.xml"))
	{
		createEmtpyCabinetXML();
		return;
	}
	
	TiXmlElement *l_pRootElement = _Cabinet.RootElement();
	if (NULL != l_pRootElement)
	{
		TiXmlElement *pDrawer = l_pRootElement->FirstChildElement("drawer");
		while (pDrawer)
		{
			HTREEITEM p = NULL;
			p = AddItemToTree(_hCabinetsWindow, const_cast<LPTSTR>(pDrawer->Attribute("name")) , NULL,pDrawer);
			TiXmlElement *pCard = pDrawer->FirstChildElement("card");
			while (pCard)
			{
			
				AddItemToTree(_hCabinetsWindow, const_cast<LPTSTR>(pCard->Attribute("name")), p, pCard );
				pCard = pCard->NextSiblingElement("card");
			}
			pDrawer = pDrawer->NextSiblingElement("drawer");
		}
	}
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			// Parse the menu selections:
			switch (wmId)
			{
			case ID_CABINET_ADDDRAWER:
				addDrawer();
				break;

			case WM_USER + 10:
			case ID_CABINET_ADDCARD:
				addCard();
				break;

			case WM_USER + 11:
				deleteItem();
				break;

			case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;

			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code here...
			//OutputDebugString(L"Paint \n");
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			saveLastChange();
			_Cabinet.SaveFile("FileCabinet.xml");
			PostQuitMessage(0);
			break;

		case WM_CONTEXTMENU: 
			if ( (HWND)wParam == _hCabinetsWindow)
			{
				OutputDebugString("WM_CONTEXTMENU\n");
				TreeViewContextMenu(lParam);
				return TRUE;
			}
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;

		case WM_NOTIFY:
			switch (((LPNMHDR)lParam)->code)
			{
				case TVN_SELCHANGED:
					//OutputDebugString("TVN_SELCHANGED\n");
					showCard((LPNMTREEVIEW)lParam);
					return TRUE;
					break;
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}




// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

////////////////////////////////////////////////////////
// Get the currently selected item and return its
// associated XML-Element
TiXmlElement* getXMLElemenItem(HTREEITEM hItem)
{
	TVITEM item;
	item.hItem = hItem;
	item.mask = TVIF_PARAM | TVIF_STATE;
	item.stateMask = TVIS_EXPANDED;
	TreeView_GetItem(_hCabinetsWindow, &item);
	return (TiXmlElement *)item.lParam;
}
////////////////////////////////////////////////////////
// Get text from the edit window and pass it to the 
// TiXmlElement referenced in the item's lParam
//
void saveChangedText(TVITEM *pItem)
{
	HTREEITEM parent = TreeView_GetParent(_hCabinetsWindow, pItem->hItem);
	if(parent == NULL)  // Drawers have no text.
		return; 

	TiXmlElement *pCard = (TiXmlElement *)pItem->lParam;
	if (pCard == NULL)
		return; // This should not happen.

	std::string x = pCard->Attribute("name");
	int iLength = GetWindowTextLength(_hEditTextWindow) + 1;
	if (iLength > 1)
	{
		char *EditText = new char[iLength];
		GetWindowText(_hEditTextWindow, EditText, iLength);
		if (pCard->FirstChild() == NULL)
		{
			TiXmlText * text = new TiXmlText(EditText);
			pCard->LinkEndChild(text);
		}
		else
		{
			pCard->FirstChild()->SetValue(EditText);
		}
		delete EditText;
	}

}

void saveLastChange()
{
	HTREEITEM selected = TreeView_GetSelection(_hCabinetsWindow);
	TVITEM item;
	ZeroMemory(&item, sizeof(item));
	item.hItem = selected;
	item.mask = TVIF_PARAM | TVIF_STATE | TVIF_TEXT;
	item.stateMask = TVIS_EXPANDED;
	item.pszText = new char[255];
	item.cchTextMax = 254;
	TreeView_GetItem(_hCabinetsWindow, &item);

	saveChangedText(&item);
	delete item.pszText;
}

void showCard(LPNMTREEVIEW tvitem)
{
	saveChangedText(&(tvitem->itemOld));

	TiXmlElement *pCard = (TiXmlElement *)tvitem->itemNew.lParam;
	HTREEITEM parent = TreeView_GetParent(_hCabinetsWindow, tvitem->itemNew.hItem);

	if (parent != NULL)
	{
		if (pCard->GetText() != NULL)
		{
			std::string CardText = pCard->GetText();
			SetWindowText(_hEditTextWindow, CardText.c_str());
		}
		else
			SetWindowText(_hEditTextWindow, "");
	}
	else
	{
		SetWindowText(_hEditTextWindow, "Opened a drawer. Select card!");
	}
}

////////////////////////////////////////////////////////////////////////////////////
//
INT_PTR CALLBACK Input(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK )
		{
			int iLength = GetWindowTextLength(GetDlgItem(hDlg, IDC_EDITDRAWERNAME)) + 1;
			if (iLength > 1)
			{
				char *EditText = new char[iLength];
				GetDlgItemText(hDlg, IDC_EDITDRAWERNAME, EditText, iLength);
				TiXmlElement *pD = new TiXmlElement("drawer");
				pD->SetAttribute("name", EditText);

				TiXmlElement *l_pRootElement = _Cabinet.RootElement();
				if (NULL != l_pRootElement)
				{
					AddItemToTree(_hCabinetsWindow, EditText, NULL, (TiXmlElement*)l_pRootElement->InsertEndChild(*pD));
				}

				delete EditText;
			}
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void addDrawer()
{
	DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOGINPUT), hMainWindow, Input);
}



////////////////////////////////////////////////////////////////////////////////////
//
INT_PTR CALLBACK CardInput(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			int iLength = GetWindowTextLength(GetDlgItem(hDlg, IDC_EDITDCARDNAME)) + 1;
			if (iLength > 1)
			{
				char *EditText = new char[iLength];
				GetDlgItemText(hDlg, IDC_EDITDCARDNAME, EditText, iLength);

				HTREEITEM selected = TreeView_GetSelection(_hCabinetsWindow);
				HTREEITEM parent = TreeView_GetParent(_hCabinetsWindow, selected);
				if (parent == NULL)
				{
					parent = selected;
				}

				TiXmlElement *pD = new TiXmlElement("card");
				pD->SetAttribute("name", EditText);
				pD->LinkEndChild(new TiXmlText(""));
				TiXmlElement *pDrawer = getXMLElemenItem(parent);
				HTREEITEM newCardItem = AddItemToTree(_hCabinetsWindow, EditText, parent, (TiXmlElement*)pDrawer->InsertEndChild(*pD));
				TreeView_Select(_hCabinetsWindow, newCardItem, TVGN_CARET);
				delete EditText;
			}

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


void addCard()
{
	HTREEITEM hSelectedItem = TreeView_GetSelection(_hCabinetsWindow);
	if (hSelectedItem == NULL)
	{
		MessageBox(hMainWindow, "Please select a drawer or card first!", "File Cabinet", MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOGCARDINPUT), hMainWindow, CardInput);
}

/////////////////////////////////////////////////////
// Show context menu 
//
void TreeViewContextMenu(LPARAM lParam)
{
	RECT rcTree;
	HTREEITEM htvItem;
	TVHITTESTINFO htInfo = { 0 };
	RECT winPos, itemPos;

	long xPos = GET_X_LPARAM(lParam);   // x position from message, in screen coordinates
	long yPos = GET_Y_LPARAM(lParam);   // y position from message, in screen coordinates 

	GetWindowRect(_hCabinetsWindow, &rcTree);              // get its window coordinates
	htInfo.pt.x = xPos - rcTree.left;              // convert to client coordinates
	htInfo.pt.y = yPos - rcTree.top;

	if (htvItem = TreeView_HitTest(_hCabinetsWindow, &htInfo))
	{    // hit test
		TreeView_SelectItem(_hCabinetsWindow, htvItem);           // success; select the item

		TreeView_GetItemRect(_hCabinetsWindow, htvItem, &itemPos, TRUE);
		GetWindowRect(_hCabinetsWindow, &winPos);

		HMENU hPopupMenu = CreatePopupMenu();
		InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, WM_USER + 10, "Add Card");
		InsertMenu(hPopupMenu, 1, MF_BYPOSITION | MF_STRING, WM_USER + 11, "Delete");
		SetForegroundWindow(hMainWindow);
		TrackPopupMenu(hPopupMenu, TPM_TOPALIGN | TPM_LEFTALIGN, winPos.left + itemPos.left, winPos.top + itemPos.bottom, 0, hMainWindow, NULL);

	}
}

void deleteItem()
{
	HTREEITEM selected = TreeView_GetSelection(_hCabinetsWindow);
	HTREEITEM parent = TreeView_GetParent(_hCabinetsWindow, selected);
	if (parent != NULL)
	{
		TVITEM item;
		item.hItem = selected;
		item.mask = TVIF_PARAM | TVIF_STATE | TVIF_TEXT;
		item.stateMask = TVIS_EXPANDED;
		item.pszText = new char[255];
		item.cchTextMax = 254;
		TreeView_GetItem(_hCabinetsWindow, &item);
		TiXmlElement *pCard = (TiXmlElement *)item.lParam;
		std::string msg = "Do you want to delete the card\n\"";
		msg += item.pszText;
		msg += +"\" ?";
		if (MessageBox(hMainWindow,  msg.c_str(), "Delete Card", MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			TreeView_DeleteItem(_hCabinetsWindow, selected);
			TiXmlNode *pDrawer = pCard->Parent();
			pDrawer->RemoveChild(pCard);
		}
	}
	else
	{
		if (TreeView_GetChild(_hCabinetsWindow, selected) != NULL)
		{
			MessageBox(hMainWindow, "The drawer ist not empty. Delete the cards first.", "Delete Drawer", MB_OK | MB_ICONEXCLAMATION);
		}
		else
		{
			TVITEM item;
			item.hItem = selected;
			item.mask = TVIF_PARAM | TVIF_STATE | TVIF_TEXT;
			item.stateMask = TVIS_EXPANDED;
			item.pszText = new char[255];
			item.cchTextMax = 254;
			TreeView_GetItem(_hCabinetsWindow, &item);
			TiXmlElement *pDrawer = (TiXmlElement *)item.lParam;
			std::string msg = "Do you want to delete the Drawer\n\"";
			msg += item.pszText;
			msg += +"\" ?";
			if (MessageBox(hMainWindow, msg.c_str(), "Delete Drawer", MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				TreeView_DeleteItem(_hCabinetsWindow, selected);
				TiXmlNode *pRoot = pDrawer->Parent();
				pRoot->RemoveChild(pDrawer);
			}

		}
	}
}

//////////////////////////////////////////////////////////////////////
// Create an emtpy XML structure. Call addDrawer() so the first entries
// can be done.
void createEmtpyCabinetXML()
{
	_Cabinet;
	TiXmlDeclaration * decl = new TiXmlDeclaration("1.0", "", "");
	TiXmlElement * element = new TiXmlElement("filecabinet");
	_Cabinet.LinkEndChild(element);
	addDrawer();
}