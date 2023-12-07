#include "grep.h"

int main(int argc, char* argv[]) {
  int err = 0;
  int i = 1;
  if (argc == 1 || argc == 2) {
    printf(
        "usage: grep [-abcDEFGHhIiJLlmnOoqRSsUVvwxZ] [-A num] [-B num] "
        "[-C[num]]\n[-e pattern] [-f file] [--binary-files=value] "
        "[--color=when]\n[--context[=num]] [--directories=action] [--label] "
        "[--line-buffered]\n[--null] [pattern] [file ...]");
    err = 5;
  }
  char big_template[BUFSIZE];
  int e_flag = 0, i_flag = 0, v_flag = 0, c_flag = 0, l_flag = 0, n_flag = 0,
      h_flag = 0, s_flag = 0, f_flag = 0, o_flag = 0;
  while (i < argc && argv[i][0] == '-' && err == 0) {
    int N = strlen(argv[i]);
    for (int k = 1; k < N; k++) {
      switch (argv[i][k]) {
        case 'e':
          e_flag = (e_flag == 0) ? 1 : 2;
          break;
        case 'i':
          i_flag = 1;
          break;
        case 'v':
          v_flag = 1;
          break;
        case 'c':
          c_flag = 1;
          break;
        case 'l':
          l_flag = 1;
          break;
        case 'n':
          n_flag = 1;
          break;
        case 'h':
          h_flag = 1;
          break;
        case 's':
          s_flag = 1;
          break;
        case 'f':
          f_flag = 1;
          break;
        case 'o':
          o_flag = 1;
          break;
        default:
          err = 2;
      }
    }
    if ((e_flag == 1 || e_flag == 2) && err == 0) {
      if (e_flag == 1)
        strcpy(big_template, argv[++i]);
      else {
        strcat(big_template, "|");
        strcat(big_template, argv[++i]);
      }
      e_flag = 2;
    }
    i++;
  }
  char* template = (char*)malloc(sizeof(char) * BUFSIZE);
  if (template == NULL) {
    if (errno) {
      if (!s_flag) {
        fprintf(stderr, "%s ", template);
        perror(template);
      }
      exit(1);
    }
  }

  int n = 0;  // кол-во подходящих строк

  if (err == 0) {
    if (e_flag != 0) {
      snprintf(template, BUFSIZE, "%s", big_template);
    } else if (f_flag == 1) {
      FILE* f = NULL;
      if ((f = fopen(argv[i++], "r")) == NULL) {
        if (errno) {
          if (!s_flag) {
            fprintf(stderr, "%s ", argv[i - 1]);
            perror(argv[i - 1]);
          }
          if (f != NULL) fclose(f);
          if (template != NULL) free(template);
          exit(1);
        }
      }
      size_t bytesRead = fread(template, 1, BUFSIZE, f);
      if (bytesRead < BUFSIZE) {
        template[bytesRead] = '\0';  // Добавляем символ завершения строки
      } else if (errno) {
        if (!s_flag) {
          fprintf(stderr, "%s ", argv[i - 1]);
          perror(argv[i - 1]);
        }
        if (template != NULL) free(template);
        if (f != NULL) fclose(f);
        exit(1);
      }
      if (f != NULL) fclose(f);
    } else {
      if (template == NULL) {
        if (errno) {
          if (!s_flag) {
            fprintf(stderr, "%s ", template);
            perror(template);
          }
          if (template != NULL) free(template);
          exit(1);
        }
      }
      strcpy(template, argv[i++]);
    }

    int N = argc - i;  // кол-во файлов
    if (N == 1) {      // если файл 1
      FILE* file = NULL;
      if ((file = fopen(argv[i++], "r")) == NULL) {
        if (errno) {
          if (!s_flag) {
            fprintf(stderr, "%s ", argv[i - 1]);
            perror(argv[i - 1]);
          }
          if (template != NULL) free(template);
          if (file != NULL) fclose(file);
          exit(1);
        }
      } else {
        if (c_flag == 1)
          printf("%d\n", ifFit(file, template, i_flag, v_flag));
        else
          grep(file, template, i_flag, v_flag, c_flag, n_flag, o_flag, l_flag,
               0, "nofile");
      }
      if (file != NULL) fclose(file);
    } else if (N > 0) {  // если файл не 1
      FILE* file[N];
      for (int k = 0; k < N; k++) {
        if ((file[k] = fopen(argv[i++], "r")) == NULL) {
          if (errno) {
            if (!s_flag) {
              fprintf(stderr, "%s ", argv[i - 1]);
              perror(argv[i - 1]);
            }
            if (template != NULL) free(template);
            if (file[k] != NULL) fclose(file[k]);
            exit(1);
          }
        }

        else {
          n = ifFit(file[k], template, i_flag, v_flag);

          if (c_flag == 1 && l_flag == 1) {
            if (n != 0)
              printf("%s:1\n", argv[i - 1]);
            else
              printf("%s:0\n", argv[i - 1]);
            if (n != 0) printf("%s\n", argv[i - 1]);
          } else if (c_flag == 1 && h_flag == 1) {
            printf("%d\n", n);
          } else if (c_flag == 1) {
            printf("%s:%d\n", argv[i - 1], n);
          } else if (l_flag == 1 && n != 0) {
            printf("%s\n", argv[i - 1]);
          } else if (n != 0) {
            grep(file[k], template, i_flag, v_flag, c_flag, n_flag, o_flag, 0,
                 1, argv[i - 1]);
          }
        }
        if (file[k] != NULL) fclose(file[k]);
      }
    }
  } else {
    err = 6;
  }

  if (template != NULL) free(template);
  return err;
}

void grep(FILE* fp, char* template, int ifIcase, int ifInvert, int ifNum,
          int ifNumOfLine, int ifOnlyTemp, int ifNoPrint, int ifFileName,
          char* filename) {
  char line[BUFSIZE];
  regex_t re;
  regmatch_t matchGroup[1];  // for o
  int n = 0;
  int n_line = 0;
  regcomp(&re, template,
          (ifIcase == 1) ? (REG_ICASE + REG_EXTENDED) : REG_EXTENDED);
  while (fgets(line, sizeof(line), fp) != NULL) {
    n_line++;
    if (regexec(&re, line, 0, NULL, 0) == (ifInvert == 1) ? 1 : 0) {
      n++;
      if (ifNumOfLine == 1 && ifOnlyTemp == 0 && ifFileName == 1 &&
          ifNoPrint == 0) {
        printf("%s:%d:%s", filename, n_line, line);
      } else if (ifNumOfLine == 1 && ifOnlyTemp == 0 && ifFileName == 0 &&
                 ifNoPrint == 0) {
        printf("%d:%s", n_line, line);
      } else if (ifFileName == 1 && ifNoPrint == 0) {
        printf("%s:%s", filename, line);
      } else if (ifOnlyTemp) {
        outputMatches(line, matchGroup, &re);
      } else if (ifNoPrint == 0 && ifFileName == 0) {
        printf("%s", line);
      }
    }
    if (ifNum == 1) printf("%d", n);
  }
  fseek(fp, -sizeof(line), SEEK_END);
  if ((regexec(&re, line, 0, NULL, 0) == (ifInvert == 1) ? 1 : 0) &&
      ifOnlyTemp == 0) {
    fseek(fp, -1, SEEK_END);
    char c = ' ';
    c = fgetc(fp);
    if (c != '\n') printf("\n");
  }
  regfree(&re);
}

int ifFit(FILE* fp, char* template, int ifIcase, int ifInvert) {
  char line[BUFSIZE];
  regex_t re;
  int n = 0;
  regcomp(&re, template,
          (ifIcase == 1) ? (REG_ICASE + REG_EXTENDED) : REG_EXTENDED);
  while (fgets(line, sizeof(line), fp) != NULL) {
    if (regexec(&re, line, 0, NULL, 0) == (ifInvert == 1) ? 1 : 0) {
      n++;
    }
  }
  regfree(&re);
  fseek(fp, 0, SEEK_SET);
  return n;
}

void outputMatches(char* line, regmatch_t* matchGroup, regex_t* preg) {
  int maxMatches = 16;
  char* cursor = line;
  for (int m = 0; (m < maxMatches) && !regexec(preg, cursor, 1, matchGroup, 0);
       m++) {
    unsigned int offset = matchGroup[0].rm_eo;

    char cursorCopy[strlen(cursor) + 1];
    strcpy(cursorCopy, cursor);
    cursorCopy[matchGroup[0].rm_eo] = 0;
    if (strcmp(cursorCopy + matchGroup[0].rm_so, "") == 0) {
      printf("%s\n", cursorCopy + matchGroup[0].rm_so);
    }
    cursor += offset;
  }
}
