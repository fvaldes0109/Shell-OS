#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "stringshelpers.h"
#include "constants.h"

int _change_working_dir(char fullRoute[], char workingDir[]);


void pwd(char output[], char workingDir[]) {

    strcpy(output, workingDir);
}

void ls(char output[], char workingDir[]) {

    char *files[DIR_MAX_FILES];
    int isFolder[DIR_MAX_FILES]; // Para saber si el i-esimo elemento es archivo o carpeta 

    DIR *dir;
    struct dirent *ent;
    struct stat st;
    dir = opendir(workingDir);

    // Leer los nombres de los archivos y carpetas en el directorio
    int num_words = 0;
    while ((ent = readdir(dir)) != NULL) {

        char *filename = ent->d_name;
        char filepath[FILEPATH_MAX];
        sprintf(filepath, "%s/%s", workingDir, filename);

        if (strcmp(filename, ".") != 0 && strcmp(filename, "..") != 0) {
            
            stat(filepath, &st);
            
            if (S_ISREG(st.st_mode)) {

                isFolder[num_words] = 0;

                char *left = "\x1b[00m";
                char *right = "";
                files[num_words] = malloc(strlen(left) + strlen(filename) + strlen(right) + 1);
                sprintf(files[num_words], "%s%s%s", left, filename, right);

                num_words++;
            }
            else if (S_ISDIR(st.st_mode)) {

                isFolder[num_words] = 1;

                char *left = "\x1b[34m";
                char *right = "\x1b[0m";
                files[num_words] = malloc(strlen(left) + strlen(filename) + strlen(right) + 1);
                sprintf(files[num_words], "%s%s%s", left, filename, right);

                num_words++;
            }

        }
    }

    // Cerrar el directorio
    closedir(dir);

    qsort(files, num_words, sizeof(char *), strcmp_nocap);

    // Colocar las palabras en el arreglo output
    int k = 0;
    for (int i = 0; i < num_words; i++) {

        for (int j = 0; j < strlen(files[i]); j++) {
            output[k] = files[i][j];
            k++;
        }
        if (i < num_words - 1) output[k++] = ' ';
    }
}

void cd(char output[], char newRoute[], char workingDir[]) {

    // Keep a copy of workingDir
    char original[strlen(workingDir)];
    strcpy(original, workingDir);

    if (newRoute[0] == '/') {
        if(_change_working_dir(newRoute, workingDir) != 0) {
            sprintf(output, "La ruta \"%s\" no existe", newRoute);
            return;
        }
    }

    else {
        char **folders = malloc(FOLDER_DEPTH_MAX * sizeof(char *));
        int num_folders = strsplit(newRoute, "/", &folders);

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

                sprintf(output, "El directorio \"%s\" no existe", newWorkingDir);
                strcpy(workingDir, original);
                return;
            }
        }
    }

    sprintf(output, "Cambiado al directorio %s", workingDir);
}

void history(char output[], char *history_arr[], int historyIndex) {

    for (int i = 0; i < historyIndex; i++) {

        char entry[INPUT_MAX_LENGTH];
        sprintf(entry, "%d: %s", i + 1, history_arr[i]);
        if (i < historyIndex - 1) strcat(entry, "\n");
        strcat(output, entry);
    }
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