#include <stdio.h>

#include "../constants.c"

void splitInput(char input[], char words[INPUT_MAX_WORDS][INPUT_MAX_LENGTH]) {

    int len = strlen(input);

    // Remover el salto de línea del final de la cadena, si existe
    if (input[len - 1] == '\n') {
        input[len - 1] = '\0';
        len--;
    }
    
    int i = 0, j = 0, num_words = 0;
    // Almacenar cada palabra en el arreglo de strings
    while (i < len && num_words < INPUT_MAX_WORDS) {

        // Saltar los espacios en blanco y las tabulaciones
        while (i < len && (input[i] == ' ' || input[i] == '\t')) {
            i++; 
        }
        
        if (i < len) {

            // Encontrar el final de la palabra actual
            j = i;
            while (j < len && input[j] != ' ' && input[j] != '\t') {
                j++; 
            }
            
            // Copiar la palabra actual en el arreglo de strings
            int k = 0;
            while (i < j) {
                words[num_words][k] = input[i];
                i++;
                k++; 
            }
            words[num_words][k] = '\0'; // Agregar el caracter nulo al final de la palabra
            num_words++; // Incrementar el contador de palabras almacenadas
        }
    }
    
    // Imprimir las palabras almacenadas en el arreglo de strings
    // printf("Las palabras en la cadena son:\n");
    // for (i = 0; i < num_words; i++) {
    //     printf("%s\n", words[i]);
    // }
}