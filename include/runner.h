void init();

int run(int argc, char **argv, int stdin_fd, char *output);

void history_push(char command[], int updateFile);
