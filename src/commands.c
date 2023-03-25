#include <string.h>
#include <dirent.h>
#include <stdlib.h>

#include "stringshelpers.h"
#include "constants.h"

void pwd(char output[], char workingDir[]) {

    strcpy(output, workingDir);
}

void ls(char output[], char workingDir[]) {

    char *files[DIR_MAX_FILES];

    DIR *dir;
    struct dirent *ent;
    dir = opendir(workingDir);

    // Leer los nombres de los archivos y carpetas en el directorio
    int num_words = 0;
    while ((ent = readdir(dir)) != NULL) {

        char *fileName = ent->d_name;
        if (strcmp(fileName, ".") != 0 && strcmp(fileName, "..") != 0) {
            
            files[num_words++] = fileName;
        }
    }

    // Cerrar el directorio
    closedir(dir);

    qsort(files, num_words, sizeof(char *), strcmp_nocap);

    // Colocar las palabras en el arreglo output
    int k = 0;
    for (int i = 0; i < num_words; i++) {

        for (int j = 0; j < strlen(files[i]); j++, k++) {
            output[k] = files[i][j];
        }
        if (i < num_words - 1) output[k++] = ' ';
    }
}
