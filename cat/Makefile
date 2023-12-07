FLAGS=-Wall -Wextra -Werror

all: clean s21_cat

s21_cat: s21_cat.o
	gcc $(FLAGS) s21_cat.o -o s21_cat

s21_cat.o:
	gcc -c $(FLAGS) cat.c -o s21_cat.o

clean:
	rm -rf *.o s21_cat