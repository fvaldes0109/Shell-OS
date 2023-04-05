#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char* trim(char* str);


int strsplit(char *str, char *delim, char ***output, int only_first) {

    int i = 0;
    char *token;
    char *copy = strdup(trim(str)); // Hacemos una copia de la cadena original

    if (only_first == 1) {
        token = strchr(copy, *delim); // Buscamos la primera ocurrencia del delimitador en la copia

        if (token != NULL) {
            *token = '\0'; // Reemplazamos el delimitador por un carácter nulo en la copia
            (*output)[0] = copy; // La primera posición del arreglo de salida es la cadena original
            (*output)[1] = token + strlen(delim) - 2; // La segunda posición del arreglo de salida es la segunda parte de la cadena separada por el delimitador
        } else {
            (*output)[0] = str; // Si no se encuentra el delimitador, la cadena original es la única parte del arreglo de salida
            (*output)[1] = "";
        }
    } else {
        token = strtok(copy, delim); // Separamos la copia en todas las ocurrencias del delimitador
        while (token != NULL) {
            if (strcmp(token, "") != 0) {
                (*output)[i] = token;
                token = strtok(NULL, delim);
                i++;
            }
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

char* trim(char* str) {
    if (str == NULL) {
        return NULL;
    }
    
    size_t len = strlen(str);
    if (len == 0) {
        return str;
    }

    // Eliminar espacios en blanco al final de la cadena
    char* end = str + len - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }
    *(end+1) = '\0';

    // Eliminar espacios en blanco al principio de la cadena
    char* start = str;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }

    // Copiar la cadena sin espacios en blanco al principio ni al final
    memmove(str, start, end - start + 2);

    return str;
}

int parse_input(char* input_str, char** commands, int* flags) {

    char* delimiter = "|<>\n";
    int i = 0;

    // parse the first command
    char* input_copy = strdup(input_str);
    char* token = strtok(input_copy, delimiter);
    
    if (token == NULL) return 0;

    commands[i] = token;
    flags[i] = 0;
    i++;
    // parse the rest of the commands and flags
    while (token != NULL) {
        char* next_token = strtok(NULL, delimiter);
        if (next_token != NULL) {
            flags[i] = 0;
            // determine the flag based on the delimiter
            char* delimiter_pos = strstr(input_str, next_token) - 1;
            while (*delimiter_pos == ' ') {
                delimiter_pos--;
            }
            if (*delimiter_pos == '|') {
                flags[i] = -3;
            } else if (*delimiter_pos == '>') {
                char* prev_delimiter_pos = delimiter_pos - 1;
                while (*prev_delimiter_pos == ' ') {
                    prev_delimiter_pos--;
                }
                if (*prev_delimiter_pos == '>') {
                    flags[i] = -2;
                } else {
                    flags[i] = -1;
                }
            } else if (*delimiter_pos == '<') {
                flags[i - 1] = 1;
            }
            // parse the next command
            commands[i] = trim(next_token);
            i++;
        }
        token = next_token;
    }

    return i;
}