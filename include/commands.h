int pwd(char workingDir[], int stdout_fd);

int cd(char newRoute[], char workingDir[], int stdout_fd);

int history(char *history_arr[], int historyIndex, int stdout_fd);

int again(int n, char *history_arr[], int historyIndex, int stdout_fd);

int help(char *keyword, char *rootDir, int stdout_fd);
