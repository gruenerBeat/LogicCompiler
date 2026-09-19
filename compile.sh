gcc -c stages/Preprocessor.c
gcc -c stages/LexicalAnalyser.c
gcc compile.c Preprocessor.o LexicalAnalyser.o -o compile
./compile example.loc
