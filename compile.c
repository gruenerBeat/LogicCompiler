#include "stages/Preprocessor.h"
#include <stdio.h>

int main(int argc, char *argv[]) {

  if (argc != 2) {
    printf("Invalid Arguments");
    return 1;
  }

  FILE *fptr = NULL;
  FILE **fptr1 = &fptr;
  if (preprocess(argv[1], fptr1) != 0) {
    printf("Compilation failed at Preprocessing stage\n");
    fclose(*fptr1);
    return 1;
  }

  fclose(*fptr1);
  return 0;
}
