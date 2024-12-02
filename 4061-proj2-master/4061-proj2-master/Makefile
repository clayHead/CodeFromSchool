all: shell.c util.o build
	gcc shell.c -o shell util.o

util.o: util.c util.h
	gcc -c util.c

build: grep du cd
	echo "Build completed"

grep: grep.c
	gcc grep.c -o grep

du: du.c
	gcc du.c -o du

cd: cd.c
	gcc cd.c -o cd

clean:
	rm -f *.o shell grep du
