#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "constants.h"
#include "commands.h"

void history_push(char command[], int updateFile);


char workingDir[OUTPUT_MAX_LENGTH];
char *history_arr[10];
int historyIndex = 0;

void init() {

    getcwd(workingDir, sizeof(workingDir));
    
    FILE *file = fopen(".history", "r");
    if (file) {

        int i = 0;
        char line[INPUT_MAX_LENGTH];
        while (fgets(line, sizeof(line), file)) {
            line[strlen(line) - 1] = '\0';
            history_push(line, 0);
        }
        fclose(file);
    }
}

int execute(char *bin_path, char **argv, int stdin_fd, int stdout_fd) {
    
    int fd[2];
    if(pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if(pid == -1) {
        perror("fork");
        return 1;
    } else if(pid == 0) {
        // redirigir la salida estándar del proceso hijo al pipe
        close(fd[0]); // cerrar el extremo de lectura del pipe
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]); // cerrar el extremo de escritura del pipe
        
        // redirigir la entrada estándar del proceso hijo al descriptor de archivo dado
        dup2(stdin_fd, STDIN_FILENO);
        
        // ejecutar el programa en el directorio enviado con los argumentos que se pasaron
        execvp(bin_path, argv);
        perror("execvp");
        exit(1);
    } else {
        // leer la salida del pipe y guardarla en el file descriptor dado
        close(fd[1]); // cerrar el extremo de escritura del pipe
        char buffer;
        ssize_t n;
        while ((n = read(fd[0], &buffer, 1)) > 0) {
            if (write(stdout_fd, &buffer, 1) != 1) {
                perror("write");
                return 1;
            }
        }

        if (n == -1) {
            perror("read");
            return 1;
        }
        close(fd[0]); // cerrar el extremo de lectura del pipe
        
        // esperar a que el proceso hijo termine
        int status;
        waitpid(pid, &status, 0);
        if(!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            fprintf(stderr, "El proceso hijo terminó de forma anormal\n");
            return 1;
        }
    }
    return 0;
}

int run(int argc, char **argv, int stdin_fd, int stdout_fd) {

    int status = 0;
    if (strcmp(argv[0], "pwd") == 0) {

        status = pwd(workingDir, stdout_fd);
    }
    else if (strcmp(argv[0], "cd") == 0) {

        if (argc == 1) return 1;
        status = cd(argv[1], workingDir, stdout_fd);
    }
    else if (strcmp(argv[0], "history") == 0) {

        status = history(history_arr, historyIndex, stdout_fd);
    }
    else if (strcmp(argv[0], "again") == 0) {

        if (argc == 1) return 1;
        status = again(atoi(argv[1]), history_arr, historyIndex, stdout_fd);
    }
    else if (strcmp(argv[0], "help") == 0) {

        status = help((argc > 1 ? argv[1] : "default"), stdout_fd);
    }
    else if (strcmp(argv[0], "exit") == 0) {

        exit(0);
    }
    else {

        char *path = getenv("PATH"); // obtener el valor de la variable PATH
        char command_path[FILEPATH_MAX]; // buffer para la ruta completa del comando

        int found = 0;
        // buscar en el directorio local
        char local_bin[FOLDER_DEPTH_MAX];
        strcpy(local_bin, workingDir);
        strcat(local_bin, "/");
        strcat(local_bin, argv[0]);
        if (access(local_bin, X_OK) == 0) {
            status = execute(local_bin, argv, stdin_fd, stdout_fd);
            found = 1;
        }

        if (found == 0) {
            // buscar el comando en cada directorio del PATH
            char *path_copy = strdup(path); // hacer una copia de la cadena PATH
            char *dir = strtok(path_copy, ":"); // dividir la copia del PATH en directorios

            while (dir != NULL) {
                sprintf(command_path, "%s/%s", dir, argv[0]); // construir la ruta completa
                if (access(command_path, X_OK) == 0) { // verificar si el comando existe y es ejecutable
                    // ejecutar el comando con la entrada y salida especificadas
                    // (Aquí debería ir el código para ejecutar el comando y colocar la salida en 'output')
                    status = execute(command_path, argv, stdin_fd, stdout_fd);
                    found = 1;
                    break;
                }
                dir = strtok(NULL, ":"); // pasar al siguiente directorio
            }
            free(path_copy);
        }

        if (found == 0) {
            
            char *error = "Comando no encontrado";
            write(stdout_fd, error, strlen(error));
            return 1;
        }
    }

    return status;
}

void history_push(char command[], int updateFile) {

    if (updateFile == 1) {
        
        command[strcspn(command, "\n")] = 0; // Elimina el salto de linea al final de la entrada

        // Buscar ocurrencias de 'again X' en el comando
        char* again_ptr = strstr(command, "again ");
        while (again_ptr != NULL) {
            int x;
            sscanf(again_ptr + 6, "%d", &x); // Leer el valor de X después de 'again '
            if (x <= historyIndex && x > 0) {
                // Reemplazar 'again X' por el elemento del historial correspondiente
                char *continue_pos = again_ptr + 6 + (x == 10 ? 2 : 1);
                char *tail = strdup(continue_pos);
                char* history_item = history_arr[x - 1];
                int history_len = strlen(history_item);
                int replace_len = snprintf(NULL, 0, "%s", history_item);
                char* replace_str = malloc(replace_len + 1);
                snprintf(replace_str, replace_len + 1, "%s", history_item);
                strncpy(again_ptr, replace_str, history_len);
                strcpy(again_ptr + history_len, tail);
            }
            else {
                printf("Error: %d está fuera del rango del historial\n", x);
            }
            again_ptr = strstr(again_ptr + 1, "again "); // Buscar la próxima ocurrencia de 'again '
        }

    }

    if (historyIndex == 10) {
        for (int i = 0; i < 9; i++) {
            strcpy(history_arr[i], history_arr[i + 1]);
        }
    }

    if (historyIndex == 10) historyIndex = 9;

    history_arr[historyIndex] = malloc((strlen(command) + 1));
    strcpy(history_arr[historyIndex], command);
    historyIndex++;

    if (updateFile == 1) {
        FILE* file = fopen(".history", "w");

        for (int i = 0; i < historyIndex; i++) {
            fprintf(file, "%s\n", history_arr[i]);
        }

        fclose(file);
    } 
}
