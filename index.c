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

int idxFindProp(IndexReaderData *ird, char *propName) {
	int i;
	
	for (i = 0; i < ird->numProps; i++) {
		if (strcmp(ird->props[i].name, propName) == 0) {
			return i;
		}
	}
	
	return -1;
}

/* name and value are malloc’d strings that should be freed */
void idxCreateProp(IndexReaderProperty *prop, char *name, char *value) {
	DEBUG_PRINT2("CREATING A PROPERTY %s = '%s'\n", name, value);
	prop->name = name;
	prop->allocdValues = 1;
	prop->numValues = 1;
	prop->values = malloc(sizeof(char *));
	prop->values[0] = value;
}

void idxAppendPropValue(IndexReaderProperty *prop, char *value) {
	/* TODO: start everything anew if property read from a new file */
	DEBUG_PRINT2("APPENDING %s += '%s'\n", prop->name, value);
	if (prop->numValues >= prop->allocdValues) {
		prop->allocdValues *= 2;
		realloc(prop->values, sizeof(char *) *
					prop->allocdValues);
	}
	prop->values[prop->numValues] = value;
	prop->numValues++;
}

/* name and value are malloc’d strings that should be freed */
void idxAppendProp(IndexReaderData *ird, char *name, char *value) {
	int propIndex;
	IndexReaderProperty *prop;
	
	propIndex = idxFindProp(ird, name);
	if (propIndex == -1) {
		/* TODO: allocate the new property */
		if (ird->numProps >= ird->allocdProps) {
			ird->allocdProps *= 2;
			realloc(ird->props, sizeof(IndexReaderProperty) *
						ird->allocdProps);
		}
		
		idxCreateProp(&ird->props[ird->numProps], name, value);
		ird->lastProp = ird->numProps;
		
		ird->numProps++;
	}
	else {
		/* TODO: check if property was read from a different file */
		free(name);
		idxAppendPropValue(&ird->props[propIndex], value);
		ird->lastProp = propIndex;
	}
}

bool idxParseLine(IndexReaderData *ird) {
	int i, propIndex;
	char *propName, *propValue;
	
	DEBUG_PRINT1("Parsing line, current position is %d\n",
			ird->currentPosition);
	i = ird->currentPosition;
	if (isspace(ird->buffer[ird->currentPosition])) {
		/* TODO: read next item in the last property */
		DEBUG_PRINT0("Continuing previous property...");
		propValue = idxReadPropValue(ird);
		DEBUG_PRINT1("Value: %s\n", propValue);
		
		if (ird->lastProp > -1) {
			DEBUG_PRINT1("Continuing prop No. %d\n", ird->lastProp);
			idxAppendPropValue(&ird->props[ird->lastProp], propValue);
		}
		else {
			/* No last prop? Then just drop the value */
			free(propValue);
		}
	}
	else {
		DEBUG_PRINT0("Staring a new property...\n");
		/* TODO: read next property */
		propName = idxReadPropName(ird);
		DEBUG_PRINT1("Property name: %s\n", propName);
		propValue = idxReadPropValue(ird);
		DEBUG_PRINT1("Property value: %s\n", propValue);
		/* TODO: check if it’s not in use already and add/append */
		
		idxAppendProp(ird, propName, propValue);
	}
}

void idxCleanIRD(IndexReaderData *ird) {
	if (ird->buffer) {
		free(ird->buffer);
	}
	
	if (ird->props) {
		/* todo: free each property’s values */
		free(ird->props);
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
		free(ird.buffer);
		return NULL;
	}
	ird.allocdProps = 8;
	ird.numProps = 0;
	ird.lastProp = -1;
	ird.props = malloc(sizeof(IndexReaderProperty) * ird.allocdProps);
	
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
	idxCleanIRD(&ird);
	return NULL;
}
