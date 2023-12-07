# FLAGS=-fsanitize=leak -fsanitize=address -Wall -Wextra -Werror
FLAGS=-Wall -Wextra -Werror

all: clean s21_grep

s21_grep: s21_grep.o
	gcc $(FLAGS) s21_grep.o -o s21_grep

s21_grep.o:
	gcc -c $(FLAGS) grep.c -o s21_grep.o

clean:
	rm -rf *.o s21_grep