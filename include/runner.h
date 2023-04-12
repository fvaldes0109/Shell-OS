void init();

int run(int argc, char **argv, int stdin_fd, int stdout_fd);

void history_push(char command[], int updateFile);
