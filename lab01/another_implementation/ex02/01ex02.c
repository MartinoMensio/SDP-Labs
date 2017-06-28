#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>

#define LINE_MAX_LEN 512

int print_enabled = 1;
FILE *file;

void child_sig_hndlr(int signo) {
    if(signo == SIGALRM) {
        kill(getppid(), SIGUSR2);
        exit(0);
    }
}

void child() {
    signal(SIGALRM, child_sig_hndlr);
    srand(time(NULL));

    alarm(60);

    while(1) {
        sleep(rand() % 10 + 1);
        kill(getppid(), SIGUSR1);
    }
}

void parent_sig_hndlr(int signo) {
    if(signo == SIGUSR1) {
        print_enabled = (print_enabled + 1) % 2;
    }
    if(signo == SIGUSR2) {
        fclose(file);
        exit(0);
    }
}

void parent(char *file_path, int cpid) {
    int line_n;
    char line[LINE_MAX_LEN];

    signal(SIGUSR1, parent_sig_hndlr);
    signal(SIGUSR2, parent_sig_hndlr);
    
    file = fopen(file_path, "r");
    if(file == NULL) {
        fprintf(stderr, "Impossible to open file %s for reading\n", file_path);
        kill(cpid, SIGKILL);
        return;
    }

    while(1) {
        line_n = 0;
        while(fgets(line, LINE_MAX_LEN, file)) {
            line_n++;
            if(print_enabled) {
                printf("LINE %d: %s", line_n, line);
            }
        }
        rewind(file);
    }
}

int main(int argc, char *argv[]) {
    int cpid;

    if(argc != 2) {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        return 1;
    }
    cpid = fork();
    if(cpid < 0) {
        perror("Error forking");
        return 2;
    }
    if(cpid == 0) {
        child();
    } else {
        parent(argv[1], cpid);
    }
    return 0;
}