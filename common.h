#define DEBUG 1

#ifdef DEBUG
#define DEBUG_PRINT0(x) fputs(x, stderr);
#else
#define DEBUG_PRINT0(x) ;
#endif

#define UNICODE
#define _UNICODE


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <tchar.h>
#include "windows.h"

typedef struct _AlphabetEntry {
	int entryLength;
	wchar_t *text;
	unsigned short int code;
} AlphabetEntry;

typedef struct _AlphabetData {
	int numEntries;
	AlphabetEntry *entries;
} AlphabetData;

typedef struct _AlphabeticString {
	int stringLength;
	unsigned short int *text;
} AlphabeticString;

typedef struct _DictionaryData {
	TCHAR *name;
	wchar_t *alphabetName;
	AlphabetData *alphabet;
	int startsFrom;
	int numEntries;
	AlphabeticString *entries;
} DictionaryData;

typedef struct _ProgramData {
	int numDicts;
	DictionaryData *dicts;
	HWND hTextInput, hOkButton, hDictList, hDrawingPane;
	int nTextInputHeight, nOkButtonHeight, nOkButtonWidth;
	WNDPROC fnTextInputProc;
} ProgramData;
