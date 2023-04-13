#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "stringshelpers.h"
#include "constants.h"
#include "user_in_handle.h"


int _change_working_dir(char fullRoute[], char workingDir[]);

int pwd(char workingDir[], int stdout_fd) {

    char copy[OUTPUT_MAX_LENGTH];
    strcpy(copy, workingDir);
    add_line_break(copy);
    // Write the current working directory to stdout
    write(stdout_fd, copy, strlen(copy));

    return 0;
}

int cd(char newRoute[], char workingDir[], int stdout_fd) {

    // Keep a copy of workingDir
    char original[strlen(workingDir)];
    strcpy(original, workingDir);
    char *bad_result = "cd: La ruta no existe\n";

    if (newRoute[0] == '/') {
        if(_change_working_dir(newRoute, workingDir) != 0) {
            write(STDERR_FILENO, bad_result, strlen(bad_result));
            return 1;
        }
    }

    else {
        char **folders = malloc(FOLDER_DEPTH_MAX * sizeof(char *));
        int num_folders = strsplit(newRoute, "/", &folders, 0);

        for (int i = 0; i < num_folders; i++) {

            char *newFolder = folders[i];

            char newWorkingDir[FILEPATH_MAX];
            sprintf(newWorkingDir, "%s/%s", workingDir, newFolder);

            // Handle . and .. cases
            if (strcmp(newFolder, ".") == 0) continue;
            else if (strcmp(newFolder, "..") == 0) {

                int i = strlen(workingDir) - 1;
                while (workingDir[i] != '/') i--;
                workingDir[i] = '\0';
            }
            else if (_change_working_dir(newWorkingDir, workingDir) != 0) {

                write(STDERR_FILENO, bad_result, strlen(bad_result));
                strcpy(workingDir, original);
                return 1;
            }
        }
    }
    return 0;
}

int history(char *history_arr[], int historyIndex, int stdout_fd) {

    char output[OUTPUT_MAX_LENGTH] = "";
    int i = (historyIndex == HISTORY_MAX ? 1 : 0);
    for (; i < historyIndex; i++) {

        char entry[INPUT_MAX_LENGTH];
        sprintf(entry, "%d: %s", i + (historyIndex == HISTORY_MAX ? 0 : 1), history_arr[i]);
        if (i < historyIndex - 1) strcat(entry, "\n");
        strcat(output, entry);
    }

    char new_line[INPUT_MAX_LENGTH];
    sprintf(new_line, "%d: history", i + (historyIndex == HISTORY_MAX ? 0 : 1));
    if (historyIndex > 0) strcat(output, "\n");
    strcat(output, new_line);

    add_line_break(output);
    write(stdout_fd, output, strlen(output));

    return 0;
}

int again(int n, char *history_arr[], int historyIndex, int stdout_fd) {

    if (n > historyIndex) {

        char *error = "again: El número de comando no existe\n";
        write(STDERR_FILENO, error, strlen(error));
        return 1;
    }

    return process_input(history_arr[n - 1], 0);
}

int help(char *keyword, char *rootDir, int stdout_fd) {

    char route[FOLDER_DEPTH_MAX] = "";
    sprintf(route, "%s/.help/%s", rootDir, keyword);
    printf("Route: %s\n", route);

    FILE *file = fopen(route, "r");
    if (file == NULL) {

        char *error = "help: No existe la ayuda para ese comando\n";
        write(STDERR_FILENO, error, strlen(error));
        return 1;
    }

    char output[OUTPUT_MAX_LENGTH];
    char line[OUTPUT_MAX_LENGTH];
    while (fgets(line, OUTPUT_MAX_LENGTH, file) != NULL) {
        strcat(output, line);
    }

    fclose(file);
    write(stdout_fd, output, strlen(output));

    return 0;
}

int _change_working_dir(char fullRoute[], char workingDir[]) {

    DIR *dir;
    dir = opendir(fullRoute);

    if (dir) {

        strcpy(workingDir, fullRoute);
        closedir(dir);
        return 0;
    }
    else {
        return 1;
    }
}
