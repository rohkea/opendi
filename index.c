#include "common.h"
#include "index.h"

void idxFreeDict(DictionaryData *dict) {
	if (dict != NULL) {
		if (dict->entries != NULL) {
			free(dict->entries);
		}
		free(dict);
	}
}

bool idxIsIndexStart(IndexReaderData *ird) {
	if (strncmp(ird->indexSeparator, &ird->buffer[ird->currentPosition],
			strlen(ird->indexSeparator)) == 0) {
		return true;
	}
	else {
		return false;
	}
}

/* Returns a malloc’d string */
char *idxReadPropName(IndexReaderData *ird) {
	int i;
	bool capitaliseNext = false;
	char currentChar, word[MAX_PROP_NAME_SIZE], *res;
	
	i = 0;
	while (i + ird->currentPosition < ird->fileSize &&
			i < MAX_PROP_NAME_SIZE - 1 &&
			!isspace(ird->buffer[ird->currentPosition + i])) {
	
		currentChar = ird->buffer[ird->currentPosition + i];
		if (currentChar == '-' || currentChar == '_') {
			capitaliseNext = true;
		}
		else if (capitaliseNext) {
			word[i] = toupper(currentChar);
		}
		else {
			word[i] = currentChar;
		}
		i++;
	}
	
	ird->currentPosition += i;
        word[i] = 0;
	res = malloc(i + 1);
	strcpy(res, word);
	return res;
}


/* Returns a malloc’d string */
char *idxReadPropValue(IndexReaderData *ird) {
	int i;
	char currentChar, word[MAX_PROP_VALUE_SIZE], *res;
	
	i = 0;
        /* Skip spaces */
	while (i + ird->currentPosition < ird->fileSize &&
			i < MAX_PROP_VALUE_SIZE - 1 &&
			isspace(ird->buffer[ird->currentPosition + i])) {
        	i++;
	}
	
	ird->currentPosition += i;
	i = 0;
        
	while (i + ird->currentPosition < ird->fileSize &&
			i < MAX_PROP_VALUE_SIZE - 1 &&
			ird->buffer[ird->currentPosition + i] != '\n') {
	
		
		currentChar = ird->buffer[ird->currentPosition + i];
		word[i] = currentChar;
		i++;
	}
	
	ird->currentPosition += i;
	if (word[i - 1] == '\r') {
		i--;
	}
	word[i] = 0;
	res = malloc(i + 1);
	strcpy(res, word);
	
	/* Skip newline */
	if (ird->buffer[ird->currentPosition] == '\n') {
		ird->currentPosition++;
	}
	
	return res;
}

void idxParseIndex(IndexReaderData *ird) {
	char *line;
	line = idxReadPropValue(ird);
	
	DEBUG_PRINT1("Got line \"%s\"\n", line);
}

bool idxParseLine(IndexReaderData *ird) {
	int i;
	char *propName, *propValue;
	
	DEBUG_PRINT1("Parsing line, current position is %d\n",
			ird->currentPosition);
	i = ird->currentPosition;
	if (isspace(ird->buffer[ird->currentPosition])) {
		/* TODO: read next item in the last property */
		DEBUG_PRINT0("Continuing previous property...");
		propValue = idxReadPropValue(ird);
		DEBUG_PRINT1("Value: %s\n", propValue);
	}
	else {
		DEBUG_PRINT0("Staring a new property...\n");
		/* TODO: read next property */
		propName = idxReadPropName(ird);
		DEBUG_PRINT1("Property name: %s\n", propName);
		propValue = idxReadPropValue(ird);
		DEBUG_PRINT1("Property value: %s\n", propValue);
		/* TODO: check if it’s not in use already and add/append */
	}
}

DictionaryData *idxLoadDict(TCHAR *filename) {
	HANDLE hFile;
	DWORD fileSize, fileSizeHi, read;
	IndexReaderData ird;
	
	hFile = CreateFile(filename, GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, 0);
	fileSize = GetFileSize(hFile, &fileSizeHi);
	
	if (fileSizeHi != 0 || fileSize > MAX_DICTFILE_SIZE) {
		CloseHandle(hFile);
		return NULL;
	}
	
	ird.fileSize = fileSize;
	ird.buffer = malloc(fileSize);
	if(!ReadFile(hFile, ird.buffer, fileSize, &read, NULL)) {
		CloseHandle(hFile);
		return NULL;
	}
	
	CloseHandle(hFile);
	
	ird.currentPosition = 0;
	ird.indexSeparator = "===";
	
	while (ird.currentPosition < ird.fileSize) {
		if (idxIsIndexStart(&ird)) {
			/* TODO: parse index words, one per line */
			idxParseIndex(&ird);
		}
		else {
			/* TODO: do normal parsing instead */
			idxParseLine(&ird);
		}
	}
	
	/* TODO: finish this */
	
	free(ird.buffer);
	return NULL;
}
