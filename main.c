#include <stdio.h>

#include "./src/runner.c"
#include "./src/constants.c"

int main() {

    while (1) {
        
        printf("mi-consola $ ");

        char input[INPUT_MAX_LENGTH];
        fgets(input, INPUT_MAX_LENGTH, stdin);

        run(input);
    }
    
    return 0;
}