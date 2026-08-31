#include "token.h"
#include <ctype.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *tokenize(char *file, int *tokenCount) {

  FILE *fptr;
  fptr = fopen(file, "r");

  if (fptr == NULL) {
    return NULL;
  }

  char *fileBuffer = malloc(256);
  if (fileBuffer == NULL) {
    return NULL;
  }

  int offset = 0;
  int allocationCount = 1;

  do {

    if (offset >= 256 * allocationCount) {
      allocationCount++;
      char *temp = realloc(fileBuffer, 256 * allocationCount);
      if (temp == NULL) {
        free(temp);
        free(fileBuffer);
        return NULL;
      } else {
        fileBuffer = temp;
      }
    }

    char character = fgetc(fptr);
    if (character == EOF) {
      break;
    }

    fileBuffer[offset] = character;

    offset++;

  } while (1);

  int tokenProcess = 0;
  int tokenNumber = 0;
  allocationCount = 1;
  char *strings = malloc(1024 * 1024);
  for (int i = 0; i < sizeof(fileBuffer); i++) {
    if (isalpha(fileBuffer[i])) {
      tokenProcess++;
    } else {
      tokenProcess = 0;
      tokenNumber++;
    }
    if (tokenNumber >= 1024 * allocationCount) {
      allocationCount++;
      char *temp = realloc(strings, 1024 * 1024 * allocationCount);
      if (temp == NULL) {
        free(temp);
        free(strings);
        free(fileBuffer);
        return NULL;
      } else {
        strings = temp;
      }
    }
    strings[tokenNumber * 1024 + tokenProcess] = fileBuffer[i];
  }

  *tokenCount = tokenNumber;

  return strings;

  free(fileBuffer);
  fclose(fptr);
}
