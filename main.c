#include <stdio.h>

#include "runner.h"
#include "constants.h"

int main() {

    init();

    while (1) {
        
        printf("mi-consola $ ");

        char input[INPUT_MAX_LENGTH];
        fgets(input, INPUT_MAX_LENGTH, stdin);

        run(input);
    }
    
    return 0;
}