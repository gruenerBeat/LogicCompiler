#include "stages/LexicalAnalyser.h"
#include "stages/Preprocessor.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {

  if (argc != 2) {
    printf("Invalid Arguments");
    return 1;
  }

  FILE *fptr = NULL;
  FILE **fptr1 = &fptr;
  if (preprocess(argv[1], fptr1, 1) != 0) {
    printf("Compilation failed at Preprocessing stage\n");
    fclose(*fptr1);
    return 1;
  }

  fseek(fptr, 0, SEEK_SET);

  int tokenCount = 0;
  char *tokens = tokenize(fptr, &tokenCount);
  if (tokens == NULL) {
    printf("Compilation failed at Lexical Analysis stage\n");
    fclose(fptr);
    return 1;
  }

  char *fileName = strtok(argv[1], ".");
  strcat(fileName, ".pre.loc");
  // remove(fileName);

  for (int i = 0; i < tokenCount; i++) {
    printf("%c", tokens[i]);
    if (tokens[i] == '\n') {
      printf("\n");
    }
  }

  return 0;
}
