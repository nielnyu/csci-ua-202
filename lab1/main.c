#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//
// // Symbol struct - we limit names to 8 characters
// struct Symbol {
//   char name[8];
//   int addr;
// };

// StringBuffer struct - helpful struct to get next
// non-whitespace char to avoid formatting issues
struct StringBuffer {
  char program[1200];
  int atChar; // pointer for head of buffer
};

char next(struct StringBuffer *s) {
  while (s->atChar < 1200 && s->program[s->atChar] != '\0' && (s->program[s->atChar] == ' ' || s->program[s->atChar] == '\n')) {
    s->atChar++;
  }

  char toReturn = s->program[s->atChar];
  s->atChar++;

  if (s->atChar >= 1200) return '\0';
  else return toReturn;
}

// main method - we run the passes independently, first
// building the symbol table, then calculating the addresses
int main( int argc, char *argv[] ) {

  // if (argc != 2) {
  //   printf("Please provide only the filename as a command-line argument.");
  //   return 1;
  // }
  //
  // char filename[] = argv[1];

  struct StringBuffer *s = malloc(sizeof(struct StringBuffer));

  char program[] = "A      \n\n\n\n     B C D E F";

  strcpy(s->program, program);
  s->atChar = 0;

  int i = 0;

  for (i = 0; i < 4; i++) {
    printf("%c\n", next(s));
  }

  free(s);
  return 0;
}
