#include <stdio.h>

#include "runner.h"
#include "constants.h"

int main() {

    init();

    while (1) {
        
        printf("\x1b[33mmi-consola\x1b[0m $ ");

        char input[INPUT_MAX_LENGTH];
        fgets(input, INPUT_MAX_LENGTH, stdin);

        run(input);
    }
    
    return 0;
}