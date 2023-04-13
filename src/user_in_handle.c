#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
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

        int last_out_fd = open(".temp_last_out_fd", O_RDONLY | O_CREAT, 0644);
        int new_out_fd = open(".temp_new_out_fd", O_RDWR | O_CREAT | O_TRUNC, 0644);

        if (flags[i] == 0 && (i == 0 || flags[i - 1] != 1)) {
            
            int status = run(argc, argv, (i > 0 ? last_out_fd : STDIN_FILENO), (i < n_instructions - 1 ? new_out_fd : STDOUT_FILENO));
            if (status != 0) return 1;
        }
        else if (flags[i] == -3 && (i == 0 || flags[i - 1] != 1)) {

            int in_fd = (i == 0 ? STDIN_FILENO : last_out_fd);

            int status = run(argc, argv, in_fd, new_out_fd);
            if (status != 0) return 1;
        }
        else if (flags[i] == -2 || flags[i] == -1) {
            
            // If flags[i] == -2, then we need to append to the file, otherwise we need to overwrite it.
            // If the file doesn't exist, then we need to create it.
            int out_fd = open(instructions[i + 1], O_WRONLY | O_CREAT | (flags[i] == -2 ? O_APPEND : O_TRUNC), 0644);

            if (i > 0 && flags[i - 1] == 1) {

                // Write the contents of the last_out_fd to out_fd
                char buffer[OUTPUT_MAX_LENGTH];
                int n_bytes = read(last_out_fd, buffer, OUTPUT_MAX_LENGTH);
                write(out_fd, buffer, n_bytes);
            }
            else {
                
                int status = run(argc, argv, (i > 0 ? last_out_fd : STDIN_FILENO), out_fd);
                close(out_fd);
                if (status != 0) return status;
            }
            i++;

        }
        else if (flags[i] == 1) {
            
            int in_fd = open(instructions[i + 1], O_RDONLY);
            if(in_fd == -1) {
                char *error ="Redirect: archivo no encontrado\n";
                write(STDERR_FILENO, error, strlen(error));
                return 1;
            }

            int status = run(argc, argv, in_fd, i < n_instructions - 2 ? new_out_fd : STDOUT_FILENO);
            if (status != 0) return 1;

            close(in_fd);
        }
        close(new_out_fd);
        close(last_out_fd);

        if (i == 0 || flags[i - 1] != 1) {

            // Move the contents of .temp_new_out_fd to .temp_last_out_fd
            int temp_new_out_fd = open(".temp_new_out_fd", O_RDONLY);
            int temp_last_out_fd = open(".temp_last_out_fd", O_WRONLY | O_CREAT | O_TRUNC, 0644);
            char buffer[OUTPUT_MAX_LENGTH];
            int n_bytes = read(temp_new_out_fd, buffer, OUTPUT_MAX_LENGTH);
            write(temp_last_out_fd, buffer, n_bytes);
            close(temp_new_out_fd);
            close(temp_last_out_fd);
        }

        free(argv);
    }
    
    if (access(".temp_new_out_fd", F_OK) != -1) remove(".temp_new_out_fd");
    if (access(".temp_last_out_fd", F_OK) != -1) remove(".temp_last_out_fd");

    return 0;
}
