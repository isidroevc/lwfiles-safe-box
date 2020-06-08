clean:
		rm a.out
all:
	gcc main.c -l sqlite3 -l uuid
