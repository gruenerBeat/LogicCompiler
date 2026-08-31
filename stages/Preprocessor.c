#include "Preprocessor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int preprocess(char *file, FILE **preprocessedFile) {

  FILE *fptr;
  fptr = fopen(file, "r");

  if (fptr == NULL) {
    return 2;
  }

  char lineBuffer[1024];
  int index = 1;

  char *outputFileName = strtok(file, ".");
  strcat(outputFileName, ".pre.loc");

  *preprocessedFile = fopen(outputFileName, "a+");

  while (fgets(lineBuffer, sizeof(lineBuffer), fptr)) {
    if (lineBuffer[0] == '$') {

      char *ptr = strtok(lineBuffer, " ");
      if (strcmp(ptr, "$use") == 0) {

        ptr = strtok(NULL, " ");
        int size = (int)strlen(ptr) - 1;

        if (ptr[size - 1] != ';') {
          printf("Expected ; at end of Preprocessor statement: %s", ptr);
          printf("In: %s Line: %d\n", file, index);
          fclose(fptr);
          return 1;
        }

        if (ptr[0] != '\'' && ptr[0] != '\"') {
          printf("Unknown Symbol %c at %s", ptr[0], ptr);
          printf("In: %s Line: %d\n", file, index);
          fclose(fptr);
          return 1;
        }

        if (ptr[0] != ptr[size - 2]) {
          printf("Expected closing %c at %s", ptr[0], ptr);
          printf("In: %s Line: %d\n", file, index);
          fclose(fptr);
          return 1;
        }

        if (ptr[0] == '\"') {
          ptr = strtok(ptr, "\"");
          FILE *temp = NULL;
          FILE **logFptr = &temp;
          int result = preprocess(ptr, logFptr);
          if (result > 0) {
            if (result == 2) {
              printf("Used logic-file %s does not exist\n", ptr);
              printf("In: %s Line: %d\n", file, index);
              fclose(*logFptr);
              fclose(fptr);
              return 1;
            }
            fclose(*logFptr);
            fclose(fptr);
            return 1;
          }

          fseek(*logFptr, 0, SEEK_SET);
          char usedFileBuffer[1024];
          while (fgets(usedFileBuffer, sizeof(usedFileBuffer), *logFptr)) {
            printf("%s\n", usedFileBuffer);
            fprintf(*preprocessedFile, usedFileBuffer);
          }

          fclose(*logFptr);
          // remove(ptr);
        }
      } else {

        printf("Invalid Preprocessor Statement: %s\n", ptr);
        printf("In: %s Line: %d\n", file, index);
        fclose(fptr);
        return 1;
      }
    } else {

      fprintf(*preprocessedFile, "%s", lineBuffer);
    }
    index++;
  }

  fclose(fptr);
  return 0;
}
