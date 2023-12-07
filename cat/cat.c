#include "cat.h"

int main(int argc, char* argv[]) {
  int err = 0;
  FILE* fp = NULL;
  if (!(argc == 2 || argc == 3))
    err = 1;
  else if (argc == 2 && ((fp = fopen(argv[1], "r")) != NULL)) {
    cat1(fp);
  } else if (argc == 3 && ((fp = fopen(argv[2], "r")) != NULL)) {
    if (cat2(argv[1], fp) != 0) err = 2;
  } else {
    err = 3;
  }
  if (err) {
    printf("err %d", err);
  }
  if (fp != NULL) fclose(fp);
  return 0;
}

void cat1(FILE* fp) {
  char c = ' ';
  fscanf(fp, "%c", &c);
  while (!feof(fp)) {
    printf("%c", c);
    fscanf(fp, "%c", &c);
  }
}

int cat2(char* flag, FILE* fp) {
  if (strcmp(flag, "-b") == 0 || strcmp(flag, "--number-nonblank") == 0) {
    number(fp, 1);
  } else if (strcmp(flag, "-e") == 0) {
    end_of_line(fp, 1);
  } else if (strcmp(flag, "-E") == 0) {
    end_of_line(fp, 0);
  } else if (strcmp(flag, "-n") == 0 || strcmp(flag, "--number") == 0) {
    number(fp, 0);
  } else if (strcmp(flag, "-s") == 0 || strcmp(flag, "--squeeze-blank") == 0) {
    squeeze_blank(fp);
  } else if (strcmp(flag, "-t") == 0) {
    tab(fp, 1);
  } else if (strcmp(flag, "-T") == 0) {
    tab(fp, 0);
  } else {
    return 1;
  }
  return 0;
}

void number(FILE* fp, int ifNonblank) {
  char prev = ' ';
  int row_num = 1;
  int c = fgetc(fp);
  if (c != EOF) {
    printf("%6d\t", row_num);
    row_num++;
  }
  while (c != EOF) {
    if (prev == '\n') {
      if (c != '\n' && (ifNonblank == 1)) {
        printf("%6d\t", row_num);
        row_num++;
      } else if (ifNonblank != 1) {
        printf("%6d\t", row_num);
        row_num++;
      }
    }
    putc(c, stdout);
    prev = c;
    c = fgetc(fp);
  }
}

void end_of_line(FILE* fp, int ifV) {
  char c = ' ';
  fscanf(fp, "%c", &c);
  while (!feof(fp)) {
    if (c == '\n') {
      printf("%c", '$');
    }
    if (flag_v_char((unsigned char)c) && (ifV == 1)) printf("%c", c);
    fscanf(fp, "%c", &c);
  }
}

void squeeze_blank(FILE* fp) {
  char prev = ' ';
  int check_flag = 1;  //  1 - Если, до этого уже была пустая строка
  int c = fgetc(fp);
  while (c != EOF) {
    if (prev == '\n' && c == '\n') {
      if (!check_flag) {
        putc(c, stdout);
        check_flag = 1;
      }
    } else {
      putc(c, stdout);
      if (c != '\n') {
        check_flag = 0;
      }
    }
    prev = c;
    c = fgetc(fp);
  }
}

void tab(FILE* fp, int ifV) {
  char c = ' ';
  fscanf(fp, "%c", &c);
  while (!feof(fp)) {
    if (c == '\t') {
      printf("%s", "^I");
    } else if (flag_v_char((unsigned char)c) && (ifV == 1))
      printf("%c", c);
    fscanf(fp, "%c", &c);
  }
}

int flag_v_char(const unsigned char c) {
  int returnVal = 1;
  if ((((int)c <= 32) || ((int)c >= 127 && (int)c < 160)) && c != '\n' &&
      c != '\t' && c != ' ') {
    if ((int)c == 127) {
      printf("^?");
    } else if ((int)c > 127) {
      printf("M-^%c", c - 64);
    } else if ((int)c <= 32) {
      printf("^%c", c + 64);
    }
    returnVal = 0;
  }
  return returnVal;
}