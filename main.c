#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "user_in_handle.h"
#include "constants.h"
#include "runner.h"

int main() {

    init();

    while (1) {
        
        printf("\x1b[33mmi-consola\x1b[0m $ ");

        char user_input[INPUT_MAX_LENGTH] = "";
        fgets(user_input, INPUT_MAX_LENGTH, stdin);

        for (size_t i = 0; i < strlen(user_input); i++) {
            if (user_input[i] == '#') user_input[i] = '\0';
        }
        
        process_input(user_input);
    }
    
    return 0;
}
