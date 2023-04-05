#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "stringshelpers.h"
#include "constants.h"
#include "commands.h"

void history_push(char command[], int updateFile);


char workingDir[OUTPUT_MAX_LENGTH];
char *history_arr[10];
int historyIndex = 0;

void init() {

    getcwd(workingDir, sizeof(workingDir));
    
    FILE *file = fopen(".history", "r");
    if (file) {

        int i = 0;
        char line[INPUT_MAX_LENGTH];
        while (fgets(line, sizeof(line), file)) {
            line[strlen(line) - 1] = '\0';
            history_push(line, 0);
        }
        fclose(file);
    }
}

void run(char *command, char *input, char *output) {

    if (strcmp(command, "pwd") == 0) {

        pwd(output, workingDir);
    }
    else if (strcmp(command, "ls") == 0) {

        ls(output, workingDir);
    }
    else if (strcmp(command, "cd") == 0) {

        if (strlen(input) == 0) return;
        cd(output, input, workingDir);
    }
    else if (strcmp(command, "history") == 0) {

        history(output, history_arr, historyIndex);
    }
    else if (strcmp(command, "echo") == 0) {

        history(output, history_arr, historyIndex);
    }
    else strcpy(output, "Comando desconocido");


    printf("%s\n", output);
}

void history_push(char command[], int updateFile) {

    if (historyIndex == 10) {
        for (int i = 0; i < 9; i++) {
            strcpy(history_arr[i], history_arr[i + 1]);
        }
    }

    if (historyIndex == 10) historyIndex = 9;

    history_arr[historyIndex] = malloc(strlen(command) + 1);
    strcpy(history_arr[historyIndex], command);
    historyIndex++;

    if (updateFile == 1) {
        FILE* file = fopen(".history", "w");

        for (int i = 0; i < historyIndex; i++) {
            fprintf(file, "%s\n", history_arr[i]);
        }

        fclose(file);
    } 

}
