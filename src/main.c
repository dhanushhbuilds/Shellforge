#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    char *line = NULL;
    size_t len = 0;
    char *args[64];

    while (1) {
        printf("shellforge$ ");
        fflush(stdout);

        if (getline(&line, &len, stdin) == -1) {
            break;
        }

        // Remove trailing newline
        line[strcspn(line, "\n")] = '\0';

        int i = 0;

        // Split input into tokens
        char *token = strtok(line, " \t");

        while (token != NULL && i < 63) {
            args[i++] = token;
            token = strtok(NULL, " \t");
        }

        // NULL-terminate argument list
        args[i] = NULL;

        // Skip empty input
        if (i == 0) {
            continue;
        }

        // Exit shell
        if (strcmp(args[0], "exit") == 0) {
            break;
        }

        // --- WEEK 5: BUILT-IN cd COMMAND ---
        if (strcmp(args[0], "cd") == 0) {

            if (args[1] == NULL) {
                fprintf(stderr, "shellforge: missing path parameter\n");
            } else {
                if (chdir(args[1]) != 0) {
                    perror("shellforge: cd");
                }
            }

            // Do not fork for cd
            continue;
        }

        // --- EXTERNAL COMMAND EXECUTION ---
        pid_t pid = fork();

        if (pid == 0) {

            // Child process
            execvp(args[0], args);

            // Only reached if execvp() fails
            perror("shellforge: execution error");
            exit(EXIT_FAILURE);

        } else if (pid > 0) {

            // Parent process
            waitpid(pid, NULL, 0);

        } else {

            // fork() failed
            perror("shellforge: fork");
        }
    }

    free(line);

    return 0;
}
