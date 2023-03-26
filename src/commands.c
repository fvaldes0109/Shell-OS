#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "stringshelpers.h"
#include "constants.h"

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

void cd(char newRoute[], char workingDir[]) {

    char **folders = malloc(FOLDER_DEPTH_MAX * sizeof(char *));
    int num_folders = strsplit(newRoute, "/", &folders);

    for (int i = 0; i < num_folders; i++) {

        char *newFolder = folders[i];

        char newWorkingDir[FILEPATH_MAX];
        sprintf(newWorkingDir, "%s/%s", workingDir, newFolder);

        DIR *dir;
        dir = opendir(newWorkingDir);

        // Handle . and .. cases
        if (strcmp(newFolder, ".") == 0) continue;
        else if (strcmp(newFolder, "..") == 0) {

            int i = strlen(workingDir) - 1;
            while (workingDir[i] != '/') i--;
            workingDir[i] = '\0';
            printf("Cambiado al directorio %s\n", workingDir);
        }
        else if (dir) {

            strcpy(workingDir, newWorkingDir);
            printf("Cambiado al directorio %s\n", workingDir);
            closedir(dir);
        }
        else {

            printf("El directorio \"%s\" no existe\n", newFolder);
        }
    }
}
