#include "LexicalAnalyser.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *tokenize(FILE *file, int *tokenCount) {

  char *characters = malloc(1024);
  int charProgress = 0;
  int reallocationCount = 1;

  if (file == NULL) {
    return NULL;
  }

  char ch = fgetc(file);
  while (ch != EOF) {
    if (charProgress >= 1024) {

      reallocationCount++;
      char *temp = realloc(characters, reallocationCount * 1024);
      if (temp == NULL) {
        return NULL;
      }
      characters = temp;
    }
    if (ch == '\n') {
      ch = fgetc(file);
      continue;
    }
    characters[charProgress] = ch;
    ch = fgetc(file);
    charProgress++;
  }

  int offset = 0;
  char *shiftedArray = malloc(1024);
  reallocationCount = 0;
  for (int i = 0; i < strlen(characters); i++) {
    if (characters[i] != ' ' && i != 0 && i != strlen(characters) &&
        !(isalnum(characters[i - 1]) && isalnum(characters[i]))) {

      shiftedArray[i + offset] = ' ';
      offset++;
    }
    if (i + offset >= reallocationCount * 1024) {
      reallocationCount++;
      char *temp = realloc(shiftedArray, reallocationCount * 1024);
      if (temp == NULL) {
        return NULL;
      }
      shiftedArray = temp;
    }
    shiftedArray[i + offset] = characters[i];
  }

  for (int i = 0; i < strlen(shiftedArray); i++) {
    char character = shiftedArray[i];
  }

  *tokenCount = strlen(shiftedArray);
  fclose(file);
  return shiftedArray;
}
