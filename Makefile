build onNT:
	gcc main.c -o rik0r.exe ./libQL.dll -Wall -g -mwindows -lmingw32 -lSDLmain -lSDL

build forNT:
	i486-mingw32-gcc main.c -o rik0r.exe ./libQL.dll -Wall -g -mwindows -lmingw32 -lSDLmain -lSDL

#Compile for linux, increment the build number, then run it.
build: main.c QL.h
	gcc --std=c99 main.c -o rik0r -fpic ./libQL.so -lm -Wall -g 
	./.incBuildNum
	./rik0r