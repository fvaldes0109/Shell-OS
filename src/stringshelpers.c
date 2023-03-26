#include <stdio.h>
#include <string.h>
#include <ctype.h>

int strsplit(char *str, char *delim, char ***output) {

    int i = 0;
    char *token = strtok(str, delim);

    while (token != NULL) {
        if (strcmp(token, "") != 0) {
            (*output)[i] = token;
            token = strtok(NULL, delim);
            i++;
        }
    }
    return i;
}

// Función para comparar dos cadenas de caracteres ignorando mayúsculas y minúsculas
int strcmp_nocap(const void *a, const void *b) {
    
    char *str1 = *(char **)a;
    char *str2 = *(char **)b;
    char c1, c2;

    // Saltarse los colores
    str1 += 4;
    str2 += 4;

    while (*str1 && *str2) {
        c1 = tolower(*str1);
        c2 = tolower(*str2);
        if (c1 != c2) {
            return c1 - c2;
        }
        str1++;
        str2++;
    }
    return 0;
}