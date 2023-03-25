#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "stringshelpers.h"
#include "constants.h"
#include "commands.h"

char workingDir[OUTPUT_MAX_LENGTH];

void init() {

    getcwd(workingDir, sizeof(workingDir));
}

void run(char command[]) {
    
    char words[INPUT_MAX_WORDS][INPUT_MAX_LENGTH];

    splitInput(command, words);

    if (strcmp(words[0], "pwd") == 0) {

        char output[OUTPUT_MAX_LENGTH];
        pwd(output, workingDir);
        printf("%s\n", output);
    }
    
    if (strcmp(words[0], "ls") == 0) {

        char output[OUTPUT_MAX_LENGTH];
        ls(output, workingDir);
        printf("%s\n", output);
    }
}


