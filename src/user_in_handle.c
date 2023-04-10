#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#include "constants.h"
#include "runner.h"
#include "stringshelpers.h"

void process_input(char user_input[]) {

    if (user_input[strlen(user_input) - 1] == '\n') user_input[strlen(user_input) - 1] = '\0';

    char* commands[INPUT_MAX_WORDS];
    int flags[INPUT_MAX_WORDS];
    int n_commands = parse_input(user_input, commands, flags);

    for (size_t i = 0; i < n_commands; i++) {

        process_command(commands[i]);
    }

    if (user_input[0] != ' ') history_push(user_input, 1);
}

void process_command(char command[]) {

    char* instructions[INPUT_MAX_WORDS];
    int flags[INPUT_MAX_WORDS];
    int n_instructions = parse_command(command, instructions, flags);

    char last_output[OUTPUT_MAX_LENGTH] = "";

    for (size_t i = 0; i < n_instructions; i++) {
        
        char **argv = malloc(INPUT_MAX_WORDS * sizeof(char *));
        int argc = strsplit(instructions[i], " \t\n", &argv, 0);
        argv[argc] = NULL;        

        if (flags[i] == 0) {

            run(argc, argv, 0, last_output);
        }
        else if (flags[i] == -3) {

            FILE *temp_file = fopen(".temp", "w");
            fprintf(temp_file, "%s", last_output);
            fclose(temp_file);

            int fd = open(".temp", O_RDONLY);
            run(argc, argv, fd, last_output);
            close(fd);

            remove(".temp");
        }
        else if (flags[i] == -2 || flags[i] == -1) {

            FILE* file = fopen(argv[0], (flags[i] == -2 ? "a" : "w"));
            fprintf(file, "%s", last_output);
            fclose(file);
            last_output[0] = '\0';
        }
        else if (flags[i] == 1) {
            
            int stdin_fd = open(instructions[i + 1], O_RDONLY);
            if(stdin_fd == -1) {
                perror("open");
                exit(1);
            }

            run(argc, argv, stdin_fd, last_output);

            close(stdin_fd);
            i++;
        }
        free(argv);
    }
    if (strlen(last_output) > 0) printf("%s", last_output);
}
