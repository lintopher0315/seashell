HEADERS = shell.h

default: shell

shell: shell.c $(HEADERS)
	gcc -Wall -Werror -std=c99 -o shell shell.c -lreadline

clean:
	-rm -f shell