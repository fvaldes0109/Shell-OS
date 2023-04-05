#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "runner.h"
#include "constants.h"
#include "stringshelpers.h"

int main() {

    init();

    while (1) {
        
        printf("\x1b[33mmi-consola\x1b[0m $ ");

        char user_input[INPUT_MAX_LENGTH];
        fgets(user_input, INPUT_MAX_LENGTH, stdin);

        if (user_input[strlen(user_input) - 1] == '\n') user_input[strlen(user_input) - 1] = '\0';
        if (user_input[0] != ' ') history_push(user_input, 1);

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

            free(words);
        }
        if (strlen(last_output) > 0) printf("%s\n", last_output);
    }
    
    return 0;
}