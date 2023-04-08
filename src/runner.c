#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "stringshelpers.h"
#include "constants.h"
#include "commands.h"

void history_push(char command[], int updateFile);

void run(char *command, char *input, char *output);


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

void process_input(char user_input[]) {

    if (user_input[strlen(user_input) - 1] == '\n') user_input[strlen(user_input) - 1] = '\0';

    char* commands[INPUT_MAX_WORDS];
    int flags[INPUT_MAX_WORDS];
    int n_commands = parse_input(user_input, commands, flags);

    char last_output[OUTPUT_MAX_LENGTH] = "";

    for (size_t i = 0; i < n_commands; i++) {
        
        char **words = malloc(INPUT_MAX_WORDS * sizeof(char *));
        int input_words = strsplit(commands[i], " \t\n", &words, 1);

        if (flags[i] == 0) {

            run(words[0], words[1], last_output);
        }
        else if (flags[i] == -3) {

            run(words[0], last_output, last_output);
        }
        else if (flags[i] == -2 || flags[i] == -1) {

            FILE* file = fopen(words[0], (flags[i] == -2 ? "a" : "w"));
            fprintf(file, "%s\n", last_output);
            fclose(file);
            last_output[0] = '\0';
        }
        else if (flags[i] == 1) {

            FILE* file = fopen(commands[i + 1], "r");
            
            /* Get the number of bytes */
            fseek(file, 0L, SEEK_END);
            long numbytes = ftell(file);
            
            /* reset the file position indicator to 
            the beginning of the file */
            fseek(file, 0L, SEEK_SET);	
            
            /* copy all the text into the buffer */
            fread(last_output, sizeof(char), numbytes, file);
            fclose(file);
            i++;
        }

        free(words);
    }
    if (strlen(last_output) > 0) printf("%s\n", last_output);

    if (user_input[0] != ' ') history_push(user_input, 1);
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
        
        echo(output, input);
    }
    else if (strcmp(command, "again") == 0) {

        if (strlen(input) == 0) return;
        again(output, atoi(input), history_arr, historyIndex);
    }
    else if (strcmp(command, "exit") == 0) {

        exit(0);
    }
    else strcpy(output, "Comando desconocido");
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
