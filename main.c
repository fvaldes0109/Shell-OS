#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "user_in_handle.h"
#include "constants.h"
#include "runner.h"

int main() {

    init();
    
    while (1) {
        
        char workingDir[FILEPATH_MAX];
        gwd(workingDir);

        printf("\x1b[33mbetter-call-shell\x1b[0m:\x1b[32m%s \x1b[0m$ ",workingDir);

        char user_input[INPUT_MAX_LENGTH] = "";
        fgets(user_input, INPUT_MAX_LENGTH, stdin);

        for (size_t i = 0; i < strlen(user_input); i++) {
            if (user_input[i] == '#') user_input[i] = '\0';
        }
        
        process_input(user_input, 1);
    }
    
    return 0;
}
