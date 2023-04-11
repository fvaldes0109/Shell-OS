#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "stringshelpers.h"
#include "constants.h"
#include "user_in_handle.h"


int _change_working_dir(char fullRoute[], char workingDir[]);

int pwd(char output[], char workingDir[]) {

    strcpy(output, workingDir);
    return 0;
}

int cd(char output[], char newRoute[], char workingDir[]) {

    // Keep a copy of workingDir
    char original[strlen(workingDir)];
    strcpy(original, workingDir);

    if (newRoute[0] == '/') {
        if(_change_working_dir(newRoute, workingDir) != 0) {
            sprintf(output, "La ruta \"%s\" no existe\n", newRoute);
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

                sprintf(output, "El directorio \"%s\" no existe\n", newWorkingDir);
                strcpy(workingDir, original);
                return 1;
            }
        }
    }

    sprintf(output, "Cambiado al directorio %s\n", workingDir);
    return 0;
}

int history(char output[], char *history_arr[], int historyIndex) {

    int i = (historyIndex == 10 ? 1 : 0);
    for (; i < historyIndex; i++) {

        char entry[INPUT_MAX_LENGTH];
        sprintf(entry, "%d: %s", i + (historyIndex == 10 ? 0 : 1), history_arr[i]);
        if (i < historyIndex - 1) strcat(entry, "\n");
        strcat(output, entry);
    }

    char new_line[20];
    sprintf(new_line, "%d: history", i + (historyIndex == 10 ? 0 : 1));
    if (historyIndex > 0) strcat(output, "\n");
    strcat(output, new_line);

    return 0;
}

int again(char output[], int n, char *history_arr[], int historyIndex) {

    if (n > historyIndex) {
        sprintf(output, "No hay tantos comandos en el historial\n");
        return 1;
    }

    return process_input(history_arr[n - 1], 0);
}

int help(char output[], char *keyword) {

    char route[FOLDER_DEPTH_MAX] = ".help/";
    strcat(route, keyword);

    FILE *file = fopen(route, "r");
    if (file == NULL) {
        sprintf(output, "No se encontró ayuda para el comando \"%s\"\n", keyword);
        return 1;
    }

    char line[INPUT_MAX_LENGTH];
    while (fgets(line, INPUT_MAX_LENGTH, file) != NULL) {
        strcat(output, line);
    }

    fclose(file);

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
