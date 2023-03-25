#include <stdio.h>
#include <string.h>

#include "./helpers/strings.c"
#include "./constants.c"

void run(char command[]) {
    
    char words[INPUT_MAX_WORDS][INPUT_MAX_LENGTH];

    splitInput(command, words);
}