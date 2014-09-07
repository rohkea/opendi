CC=/usr/bin/i586-mingw32msvc-gcc

dict.exe: dict.o helper.o
	$(CC) -o dict.exe dict.o helper.o -lgdi32 

dict.o: dict.c common.h
	$(CC) -c -o dict.o dict.c

helper.o: helper.c common.h
	$(CC) -c -o helper.o helper.c

run: dict.exe
	wine dict.exe

clean:
	rm -rf dict.exe *.o

.PHONY: run clean
