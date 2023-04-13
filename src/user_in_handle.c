#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#include "constants.h"
#include "runner.h"
#include "stringshelpers.h"

int process_command(char command[]);

int process_input(char user_input[], int add_to_history) {
    
    if (user_input[strlen(user_input) - 1] == '\n') user_input[strlen(user_input) - 1] = '\0';

    char* commands[INPUT_MAX_WORDS];
    int flags[INPUT_MAX_WORDS];
    int n_commands = parse_input(user_input, commands, flags);
    int prev_status = 0;

    for (size_t i = 0; i < n_commands; i++) {

        if (prev_status == 0 && flags[i] == 3) return 0;
        else if(prev_status == 1 && flags[i] == 2) return 1;
        prev_status = process_command(commands[i]);
    }

    if (add_to_history != 0 && user_input[0] != ' ') history_push(user_input, 1);
}

int process_command(char command[]) {
    char* instructions[INPUT_MAX_WORDS];
    int flags[INPUT_MAX_WORDS];
    int n_instructions = parse_command(command, instructions, flags);

    char last_output[OUTPUT_MAX_LENGTH] = "";

    for (size_t i = 0; i < n_instructions; i++) {
        
        char **argv = malloc(INPUT_MAX_WORDS * sizeof(char *));
        int argc = strsplit(instructions[i], " \t\n", &argv, 0);
        argv[argc] = NULL;        

        if (n_instructions == 1 && argc == 1) {
            if (strcmp(argv[0], "true") == 0) return 0;
            if (strcmp(argv[0], "false") == 0) return 1;
        }

        int temp_fd = open(".temp_fd", O_RDWR | O_CREAT, 0644);

        if (flags[i] == 0 && (i == 0 || flags[i - 1] != 1)) {

            int status = run(argc, argv, (i > 0) ? temp_fd : 0, i < n_instructions - 1 ? temp_fd : 0);
            if (status != 0) return 1;
        }
        // else if (flags[i] == -3) {

        //     FILE *temp_file = fopen(".temp", "w");
        //     fprintf(temp_file, "%s", last_output);
        //     fclose(temp_file);

        //     int fd = open(".temp", O_RDONLY);
        //     int status = run(argc, argv, fd, last_output);
        //     if (status != 0) return status;
        //     close(fd);

        //     remove(".temp");
        // }
        else if (flags[i] == -2 || flags[i] == -1) {
            
            // If flags[i] == -2, then we need to append to the file, otherwise we need to overwrite it.
            // If the file doesn't exist, then we need to create it.
            int out_fd = open(instructions[i + 1], O_WRONLY | O_CREAT | (flags[i] == -2 ? O_APPEND : O_TRUNC), 0644);

            if (i > 0 && flags[i - 1] == 1) {

                // Write the contents of the temp_fd to out_fd
                char buffer[OUTPUT_MAX_LENGTH];
                int n_bytes = read(temp_fd, buffer, OUTPUT_MAX_LENGTH);
                write(out_fd, buffer, n_bytes);
            }
            else {
                
                int status = run(argc, argv, (i > 0 ? temp_fd : STDIN_FILENO), out_fd);
                close(out_fd);
                if (status != 0) return status;
            }
            i++;

        }
        else if (flags[i] == 1) {
            
            int in_fd = open(instructions[i + 1], O_RDONLY);
            if(in_fd == -1) {
                char *error = "Error: archivo no encontrado\n";
                write(STDERR_FILENO, error, strlen(error));
                return 1;
            }

            int status = run(argc, argv, in_fd, i < n_instructions - 2 ? temp_fd : STDOUT_FILENO);
            if (status != 0) return 1;

            close(in_fd);
        }

        close(temp_fd);
        free(argv);
    }

    return 0;
}
