#include "Preprocessor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int preprocess(char *file, FILE **preprocessedFile, int originalFile) {

  FILE *fptr;
  fptr = fopen(file, "r");

  if (fptr == NULL) {
    return 2;
  }

  char firstPassLineBuffer[1024];
  int index = 1;

  char *outputFileName = strtok(file, ".");
  strcat(outputFileName, ".pre.loc");
  FILE *tmpFptr = fopen(outputFileName, "w");
  if (tmpFptr != NULL) {
    fclose(tmpFptr);
    remove(outputFileName);
  }

  *preprocessedFile = fopen(outputFileName, "a+");

  while (fgets(firstPassLineBuffer, sizeof(firstPassLineBuffer), fptr)) {
    if (firstPassLineBuffer[0] == '$') {
      char *preservedLine = malloc(1024);
      strcpy(preservedLine, firstPassLineBuffer);
      char *ptr = strtok(firstPassLineBuffer, " ");
      if (strcmp(ptr, "$use") == 0) {

        ptr = strtok(NULL, " ");
        int size = (int)strlen(ptr) - 1;

        if (ptr[size - 1] != ';') {
          printf("Expected ; at end of Preprocessor statement: %s", ptr);
          printf("In: %s Line: %d\n", file, index);
          fclose(fptr);
          free(preservedLine);
          return 1;
        }

        if (ptr[0] != '\'' && ptr[0] != '\"') {
          printf("Unknown Symbol %c at %s", ptr[0], ptr);
          printf("In: %s Line: %d\n", file, index);
          fclose(fptr);
          free(preservedLine);
          return 1;
        }

        if (ptr[0] != ptr[size - 2]) {
          printf("Expected closing %c at %s", ptr[0], ptr);
          printf("In: %s Line: %d\n", file, index);
          fclose(fptr);
          free(preservedLine);
          return 1;
        }

        if (ptr[0] == '\"') {
          ptr = strtok(ptr, "\"");
          FILE *temp = NULL;
          FILE **logFptr = &temp;
          int result = preprocess(ptr, logFptr, 0);
          if (result > 0) {
            if (result == 2) {
              printf("Used logic-file %s does not exist\n", ptr);
              printf("In: %s Line: %d\n", file, index);
              fclose(*logFptr);
              fclose(fptr);
              free(preservedLine);
              return 1;
            }
            fclose(*logFptr);
            fclose(fptr);
            free(preservedLine);
            return 1;
          }

          fseek(*logFptr, 0, SEEK_SET);
          char usedFileBuffer[1024];
          while (fgets(usedFileBuffer, sizeof(usedFileBuffer), *logFptr)) {
            fprintf(*preprocessedFile, "%s", usedFileBuffer);
          }

          fclose(*logFptr);
          remove(ptr);
        }
      } else if (strcmp(ptr, "$macro") == 0) {

        ptr = strtok(NULL, " ");
        ptr = strtok(NULL, " ");

        if (strcmp(ptr, ":=") != 0) {
          printf("Unknown operation \"%s\" in Preprocessor statement\n", ptr);
          printf("In: %s Line: %d\n", file, index);
          fclose(fptr);
          free(preservedLine);
          return 1;
        }

        ptr = strtok(NULL, " ");
        int size = (int)strlen(ptr) - 1;
        if (ptr[size - 1] != ';') {
          printf("Expected ; at end of Preprocessor statement: %s", ptr);
          printf("In: %s Line: %d\n", file, index);
          fclose(fptr);
          free(preservedLine);
          return 1;
        }
        fprintf(*preprocessedFile, "%s", preservedLine);

      } else {

        printf("Invalid Preprocessor Statement: %s\n", ptr);
        printf("In: %s Line: %d\n", file, index);
        fclose(fptr);
        free(preservedLine);
        return 1;
      }
      free(preservedLine);
    } else {

      fprintf(*preprocessedFile, "%s", firstPassLineBuffer);
    }
    index++;
  }

  fclose(fptr);
  if (originalFile == 0) {
    return 0;
  }

  fseek(*preprocessedFile, 0, SEEK_SET);

  FILE *tempFile = fopen("pre.temp", "w+");
  if (tempFile == NULL) {
    return 1;
  }

  char secondLineBuffer[1024];
  struct Macro *dictionary = malloc(128 * sizeof(struct Macro));
  int macroIndex = 0;
  int realloctaionCount = 1;
  while (fgets(secondLineBuffer, sizeof(secondLineBuffer), *preprocessedFile)) {
    if (secondLineBuffer[0] == '$') {

      char *ptr = strtok(secondLineBuffer, " ");
      if (strcmp(ptr, "$macro") == 0) {
        if (macroIndex >= 128) {
          realloctaionCount++;
          struct Macro *temp = realloc(dictionary, realloctaionCount * 128 *
                                                       sizeof(struct Macro));
          if (temp == NULL) {
            free(dictionary);
            return 1;
          }
          dictionary = temp;
        }
        ptr = strtok(NULL, " ");
        char *source = malloc(128);
        strcpy(source, ptr);
        ptr = strtok(NULL, " ");
        ptr = strtok(NULL, " ");
        ptr = strtok(ptr, ";");
        struct Macro *macro = malloc(sizeof(struct Macro));
        strcpy(macro->original, source);
        strcpy(macro->replacement, ptr);
        dictionary[macroIndex] = *macro;
        macroIndex++;
      }
    } else {
      if (macroIndex != 0) {

        for (int i = 0; i < macroIndex; i++) {
          char source[256];
          char target[256];
          strcpy(source, dictionary[i].original);
          strcpy(target, dictionary[i].replacement);
          char *substr = strstr(secondLineBuffer, source);
          while (substr != NULL) {
            int pos = substr - secondLineBuffer;
            int substrLen = strlen(source);
            char shiftBuffer[1024];
            strcpy(shiftBuffer, secondLineBuffer);
            int shift = strlen(target) - substrLen;
            for (int j = 0; j < 1024; j++) {
              if (j >= pos + substrLen) {
                if (!(j + shift > 1023 || j + shift < 0)) {
                  shiftBuffer[j + shift] = secondLineBuffer[j];
                }
              }
            }

            for (int k = 0; k < strlen(dictionary[i].replacement); k++) {
              shiftBuffer[pos + k] = dictionary[i].replacement[k];
            }

            strcpy(secondLineBuffer, shiftBuffer);
            substr = strstr(secondLineBuffer, source);
          }
        }
      }
      fprintf(tempFile, "%s", secondLineBuffer);
    }
  }

  fclose(*preprocessedFile);
  remove(outputFileName);
  *preprocessedFile = fopen(outputFileName, "w+");

  fseek(tempFile, 0, SEEK_SET);

  char thirdLineBuffer[1024];
  while (fgets(thirdLineBuffer, sizeof(thirdLineBuffer), tempFile)) {
    fprintf(*preprocessedFile, "%s", thirdLineBuffer);
  }

  fclose(tempFile);
  remove("pre.temp");
  return 0;
}
