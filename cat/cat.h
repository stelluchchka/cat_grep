#ifndef CAT_H
#define CAT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFSIZE 256

void cat1(FILE* fp);
int cat2(char* flag, FILE* fp);

void number(FILE* fp, int ifNonblank);
void end_of_line(FILE* fp, int ifV);
void squeeze_blank(FILE* fp);
void tab(FILE* fp, int ifV);
int flag_v_char(const unsigned char c);

#endif  // CAT_H