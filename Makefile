HEADERS = shell.h

default: shell

shell: shell.c $(HEADERS)
	gcc -Wall -Werror -std=c99 -o shell shell.c -lreadline

run: shell
	./shell

debug: shell
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./shell

clean:
	-rm -f shell