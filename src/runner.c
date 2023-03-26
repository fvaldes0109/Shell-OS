#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "stringshelpers.h"
#include "constants.h"
#include "commands.h"

char workingDir[OUTPUT_MAX_LENGTH];

void init() {

    getcwd(workingDir, sizeof(workingDir));
}

void run(char command[]) {
    
    char **words = malloc(INPUT_MAX_WORDS * sizeof(char *));

    int input_words = strsplit(command, " \t\n", &words);

    if (strcmp(words[0], "pwd") == 0) {

        char output[OUTPUT_MAX_LENGTH] = "";
        pwd(output, workingDir);
        printf("%s\n", output);
    }
    
    else if (strcmp(words[0], "ls") == 0) {

        char output[OUTPUT_MAX_LENGTH] = "";
        ls(output, workingDir);
        printf("%s\n", output);
    }

    else if (strcmp(words[0], "cd") == 0) {

        char output[OUTPUT_MAX_LENGTH] = "";
        if (input_words == 1) return;
        cd(output, words[1], workingDir);
        printf("%s\n", output);
    }

    else printf("Comando desconocido");
}


