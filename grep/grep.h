#ifndef GREP_H
#define GREP_H

#include <errno.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFSIZE 256

void grep(FILE* fp, char* template, int ifIcase, int ifInvert, int ifNum,
          int ifNumOfLine, int ifOnlyTemp, int ifNoPrint, int ifFileName,
          char* filename);
int ifFit(FILE* fp, char* template, int ifIcase, int ifInvert);
void outputMatches(char* line, regmatch_t* matchGroup, regex_t* preg);

#endif  // GREP_H