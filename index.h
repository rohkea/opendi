#define MAX_DICTFILE_SIZE 1024 * 1024
#define MAX_PROP_NAME_SIZE 512
#define MAX_PROP_VALUE_SIZE 512

typedef struct _IndexReaderProperty {
	char *name;
	char **values;
	int numValues;
	int allocdValues;
} IndexReaderProperty;

typedef struct _IndexReaderEntry {
	char *name;
	int numLines;
	char **lines;
} IndexReaderEntry;

typedef struct _IndexReaderData {
	unsigned char *buffer;
	int fileSize;
	int currentPosition;
	char *indexSeparator;
	IndexReaderProperty *props;
	int numProps;
	int allocdProps;
	int lastProp;
} IndexReaderData;
