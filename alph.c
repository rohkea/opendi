#include "common.h"
#include "alph.h"

void alphFreeAString(AlphabeticString *s) {
	if (s != NULL) {
		if (s->text != NULL) {
			free(s->text);
		}
		free(s);
	}
}

/* returned string should be freed with alphFreeAString */
AlphabeticString *alphToAString(const AlphabetData *ad, const wchar_t *s) {
	unsigned short int *res;
	AlphabeticString *as;
	int allocated, current;
	int from, to, centre, comparisonResult;
	
	allocated = 16;
	res = malloc(allocated * sizeof(unsigned short int));
	current = 0;	
	
	while (*s) {
		from = 0;
		to = ad->numEntries;
		while (from < to - 1) {
			centre = (from + to) / 2;
			comparisonResult = wcsncmp(s,
					ad->entries[centre].text,
					ad->entries[centre].entryLength);
			if (comparisonResult == 0) {
				from = centre;
				to = centre;
				break;
			}
			else if (comparisonResult > 0) {
				from = centre + 1;
			}
			else {
				to = centre;
			}
		}
		/* TODO: make the behaviour greedy */
		
		comparisonResult = wcsncmp(s, ad->entries[from].text,
					ad->entries[from].entryLength);
		if (comparisonResult == 0) {
			res[current] = ad->entries[from].code;
			current++;
			if (current == allocated) {
				allocated *= 2;
				res = realloc(res, allocated *
						sizeof(unsigned short int));
			}
			s += wcslen(ad->entries[from].text);
		}
		else {
			s++;
		}
	}
	res[current] = L'\0';
	
	as = malloc(sizeof(AlphabeticString));
	as->stringLength = current;
	as->text = res;
	
	return as;
}

int alphCompare(const AlphabeticString *s1, const AlphabeticString *s2) {
	int i;
	unsigned short int *t1, *t2;
	
	t1 = s1->text;
	t2 = s2->text;
	do {
		if (*t1 != *t2) {
			return *t1 - *t2;
		}
		else {
			t1++;
			t2++;
		}
	} while (*t1 && *t2);
	
	if (*t1 != 0) {
		return 0x7FFF;
	}
	
	if (*t2 != 0) {
		return -0x8000;
	}
	
	return 0;
}

/* For debugging */
void alphPrintAString(const AlphabeticString *a) {
	int i;
	
	for (i = 0; i <= a->stringLength; i++) {
		printf("%d ", a->text[i]);
	}
	puts("\n");
}
