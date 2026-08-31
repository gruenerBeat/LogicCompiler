gcc -c stages/Preprocessor.c
gcc compile.c Preprocessor.o -o compile
./compile example.loc
