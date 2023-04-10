#include "constants.h"

// Función para comparar dos cadenas de caracteres ignorando mayúsculas y minúsculas
int strcmp_nocap(const void *a, const void *b);

int strsplit(char *str, char *delim, char ***output, int only_first);

int parse_input(char* user_input, char** commands, int* flags);

int parse_command(char* command, char** instructions, int* flags);

char* trim(char* str);
