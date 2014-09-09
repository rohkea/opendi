#include "common.h"
#include "alph.h"
#define IDC_OKBUTTON 1001

/* TODO: add letters with macrons and breves */
AlphabetEntry latin_iu_alphabet_entries[] = {
        {1, L"A", 1},
        {1, L"B", 2},
        {1, L"C", 3},
        {1, L"D", 4},
        {1, L"E", 5},
        {1, L"F", 6},
        {1, L"G", 7},
        {1, L"H", 8},
        {1, L"I", 9},
        {1, L"J", 9},
        {1, L"K", 10},
        {1, L"L", 11},
        {1, L"M", 12},
        {1, L"N", 13},
        {1, L"O", 14},
        {1, L"P", 15},
        {1, L"Q", 16},
        {1, L"R", 17},
        {1, L"S", 18},
        {1, L"T", 19},
        {1, L"U", 20},
        {1, L"V", 20},
        {1, L"W", 21},
        {1, L"X", 22},
        {1, L"Y", 23},
        {1, L"Z", 24},
        {1, L"a", 1},
        {1, L"b", 2},
        {1, L"c", 3},
        {1, L"d", 4},
        {1, L"e", 5},
        {1, L"f", 6},
        {1, L"g", 7},
        {1, L"h", 8},
        {1, L"i", 9},
        {1, L"j", 9},
        {1, L"k", 10},
        {1, L"l", 11},
        {1, L"m", 12},
        {1, L"n", 13},
        {1, L"o", 14},
        {1, L"p", 15},
        {1, L"q", 16},
        {1, L"r", 17},
        {1, L"s", 18},
        {1, L"t", 19},
        {1, L"u", 20},
        {1, L"v", 20},
        {1, L"w", 21},
        {1, L"x", 22},
        {1, L"y", 23},
        {1, L"z", 24}
};

AlphabetData latin_iu_alphabet = {26 * 2, latin_iu_alphabet_entries};

AlphabeticString tmp_index_1[] = {
	{2, (short int []){1, 2, 0}}, /* cv */
	{3, (short int []){1, 2, 1, 0}}, /* cvc */
	{2, (short int []){2, 1, 0}}, /* vc */
	{3, (short int []){2, 1, 2, 0}} /* vcv */
};

AlphabeticString tmp_index_2[] = {
	{1, (short int []){1, 0}}, /* a */
	{2, (short int []){1, 3, 0}}, /* ac */
	{2, (short int []){2, 1, 0}}, /* ba */
	{3, (short int []){2, 2, 3, 0}}, /* bbc */
	{2, (short int []){3, 1, 0}}, /* ca */
	{2, (short int []){3, 3, 0}} /* cc */
};

DictionaryData tmp_dicts[] = {
	{L"Test1", L"Latin", 10, 4, tmp_index_1},
	{L"Test2", L"Latin", 3, 6, tmp_index_2}
};

const TCHAR className[] = TEXT("Thesaurorum verborum...");




void dictSearchWord(HWND hwnd) {
	TCHAR text[512];
	ProgramData *pd;

	pd = (ProgramData *)GetWindowLong(hwnd, 0);
	GetWindowText(pd->hTextInput, text, 512);
	MessageBox(0, text, TEXT("Поиск"), MB_OK);
}

void dictSetDictionaries(HWND hwnd, int nDicts, DictionaryData *dd) {
	ProgramData *pd;
	int i;
	
	pd = (ProgramData *)GetWindowLong(hwnd, 0);
	
	SendMessage(pd->hDictList, LB_RESETCONTENT, 0, 0);
	for (i = 0; i < nDicts; i++) {
		SendMessage(pd->hDictList, LB_ADDSTRING, 0, (LPARAM)dd[i].name);
	}
	pd->numDicts = nDicts;
	pd->dicts = dd;
}

LRESULT CALLBACK dictTextInputProc(HWND hwnd, UINT msg, WPARAM wParam,
					LPARAM lParam) {
	HWND hParentWindow;
	ProgramData *pd;
	
	hParentWindow = (HWND)GetWindowLong(hwnd, GWL_HWNDPARENT);
	pd = (ProgramData *)GetWindowLong(hParentWindow, 0);
	
	if (!pd) {
		return -1;
	}
	
	switch (msg) {
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_RETURN:
			dictSearchWord(hParentWindow);
			break;
		}
		break;
	}
	
	return CallWindowProc(pd->fnTextInputProc, hwnd, msg, wParam, lParam);
}

LRESULT dictMainWindowCreated(HWND hwnd, CREATESTRUCT *cs) {
	ProgramData *pd;
	HGDIOBJ hDefaultFont;
	int textHei, bnTextWid, bnTextHei;
	const TCHAR *bnText = TEXT("Найти");	
	RECT clRect;
	int wid, hei;

	DEBUG_PRINT0("Allocating ProgramData...\n");
	pd = malloc(sizeof(ProgramData));
	//TODO: use SetWindowLongPtr for 64-bit compatibility?
	SetWindowLong(hwnd, 0, (LONG)pd);
	
	textHei = getFontHeight(hDefaultFont);
	getTextSize(hDefaultFont, bnText, &bnTextWid, &bnTextHei);
	if (bnTextHei > textHei) {
		textHei = bnTextHei;
	}
	pd->nOkButtonHeight = bnTextHei + 15;
	pd->nOkButtonWidth = bnTextWid + 15;
	
	GetClientRect(hwnd, &clRect);
	wid = clRect.right - clRect.left;
	hei = clRect.bottom - clRect.top;
	
	hDefaultFont = GetStockObject(DEFAULT_GUI_FONT);
	pd->nTextInputHeight = textHei + 15;
	DEBUG_PRINT0("Creating text input...\n");
	pd->hTextInput = CreateWindowEx(
		WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("Привет! Это проверка"),
		WS_CHILD|WS_VISIBLE,
		0, 0,
		wid - pd->nOkButtonWidth, pd->nTextInputHeight,
		hwnd, NULL, cs->hInstance, 0
	);
	
	if (!pd->hTextInput) {
		DEBUG_PRINT0("Cannot create a text input field!\n");
	}
	
	SendMessage(pd->hTextInput, WM_SETFONT,
			(WPARAM)hDefaultFont, MAKELPARAM(FALSE, 0));
	pd->fnTextInputProc = (WNDPROC)SetWindowLong(pd->hTextInput,
						GWL_WNDPROC,
						(DWORD)dictTextInputProc);
	
	pd->hOkButton = CreateWindowEx(
		WS_EX_CLIENTEDGE, TEXT("BUTTON"), bnText,
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		wid - pd->nOkButtonWidth, 0,
		pd->nOkButtonWidth, pd->nOkButtonHeight,
		hwnd, NULL, cs->hInstance, NULL
	);
	SetWindowLong(pd->hOkButton, GWL_ID, IDC_OKBUTTON);
	SendMessage(pd->hOkButton, WM_SETFONT,
			(WPARAM)hDefaultFont, MAKELPARAM(FALSE, 0));
	
	pd->hDictList = CreateWindowEx(
		WS_EX_CLIENTEDGE, TEXT("LISTBOX"), NULL,
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOINTEGRALHEIGHT,
		0, pd->nTextInputHeight,
		wid, hei - pd->nTextInputHeight,
		hwnd, NULL, cs->hInstance, 0
	);
	
	SendMessage(pd->hDictList, WM_SETFONT,
			(WPARAM)hDefaultFont, MAKELPARAM(FALSE, 0));
	
	return 0;
}

LRESULT dictMainWindowDestroyed(HWND hwnd) {
	ProgramData *pd;
	
	DEBUG_PRINT0("Freeing ProgramData...\n");
	//TODO: use GetWindowLongPtr for 64-bit compatibility?
	pd = (ProgramData *)GetWindowLong(hwnd, 0);
	free(pd);
	SetWindowLong(hwnd, 0, 0);
	
	PostQuitMessage(0);
}

void dictMainWindowResized(HWND hwnd, int width, int height) {
	ProgramData *pd;
	
	pd = (ProgramData *)GetWindowLong(hwnd, 0);
	MoveWindow(pd->hTextInput,
			0, 0,
			width - pd->nOkButtonWidth, pd->nTextInputHeight,
			true);

	MoveWindow(pd->hOkButton,
			width - pd->nOkButtonWidth, 0,
			pd->nOkButtonWidth, pd->nOkButtonHeight,
			true);
	MoveWindow(pd->hDictList,
			0, pd->nTextInputHeight,
			width, height - pd->nTextInputHeight,
			true);
	
	InvalidateRect(hwnd, NULL, false);
}

LRESULT CALLBACK MainWindowWndProc(HWND hwnd, UINT msg,
					WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CREATE:
		return dictMainWindowCreated(hwnd, (CREATESTRUCT *)lParam);
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		dictMainWindowDestroyed(hwnd);
		break;
	case WM_SIZE:
		dictMainWindowResized(hwnd, LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_OKBUTTON:
			dictSearchWord(hwnd);
			break;
		}
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

bool dictRegisterMainWindowClass(HINSTANCE hInstance) {
	WNDCLASSEX wc;
	
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = MainWindowWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(ProgramData *);
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = className;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	
	if (RegisterClassEx(&wc)) {
		return true;
	}
	else {
		MessageBox(NULL, TEXT("Cannot register window class!"),
				TEXT("Error"), MB_ICONEXCLAMATION | MB_OK);
		return false;

	}
}

bool dictMainLoop(HINSTANCE hInstance, int nCmdShow) {
	HWND hwnd;
	MSG msg;
	
	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		className,
		TEXT("Dictionary index"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
		NULL, NULL, hInstance, NULL
	);
	
	if (hwnd == NULL) {
		MessageBox(NULL, TEXT("Cannot create main window!"),
				TEXT("Error"), MB_ICONEXCLAMATION | MB_OK);
		return false;
	}
	dictSetDictionaries(hwnd, 2, tmp_dicts);
	
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}


int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
			LPSTR lpCmdLine, int nCmdShow) {
	alphPrintAString(alphToAString(&latin_iu_alphabet, L"this is a test"));
	
	if (!dictRegisterMainWindowClass(hInstance)) {
		return -1;
	}
	
	if (dictMainLoop(hInstance, nCmdShow)) {
		return 0;
	}
	else {
		return -1;
	}
}
