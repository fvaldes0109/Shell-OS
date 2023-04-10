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

void execute(char *bin_path, char **argv, int stdin_fd, char *output) {
    
    int fd[2];
    if(pipe(fd) == -1) {
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork();
    if(pid == -1) {
        perror("fork");
        exit(1);
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
        // leer la salida del pipe y guardarla en el parámetro output
        close(fd[1]); // cerrar el extremo de escritura del pipe
        ssize_t n = read(fd[0], output, OUTPUT_MAX_LENGTH-1);
        if(n == -1) {
            perror("read");
            exit(1);
        }
        output[n] = '\0'; // agregar el caracter de terminación de cadena
        close(fd[0]); // cerrar el extremo de lectura del pipe
        
        // esperar a que el proceso hijo termine
        int status;
        waitpid(pid, &status, 0);
        if(!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            fprintf(stderr, "El proceso hijo terminó de forma anormal\n");
            exit(1);
        }
    }
}

void run(int argc, char **argv, int stdin_fd, char *output) {

    if (strcmp(argv[0], "cd") == 0) {

        if (argc == 1) return;
        cd(output, argv[1], workingDir);
    }
    else if (strcmp(argv[0], "history") == 0) {

        history(output, history_arr, historyIndex);
    }
    else if (strcmp(argv[0], "again") == 0) {

        if (argc == 1) return;
        again(output, atoi(argv[1]), history_arr, historyIndex);
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
            execute(local_bin, argv, stdin_fd, output);
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
                    execute(command_path, argv, stdin_fd, output);
                    found = 1;
                }
                dir = strtok(NULL, ":"); // pasar al siguiente directorio
            }
            free(path_copy);
        }

        if (found == 0) strcpy(output, "Comando desconocido");
    }

    size_t len = strlen(output);
    if (strcmp(argv[0], "again") != 0 && output[len - 1] != '\n') {
        output[len] = '\n';
        output[len + 1] = '\0';
    }
}

void history_push(char command[], int updateFile) {


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
