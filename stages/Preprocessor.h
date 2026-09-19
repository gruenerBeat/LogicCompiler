#ifndef PREPROCESSOR_HPP
#define PREPROCESSOR_HPP

#include <stdio.h>

struct Macro {
  char original[1024];
  char replacement[1024];
};

int preprocess(char *file, FILE **preprocessedFile, int originalFile);

#endif
