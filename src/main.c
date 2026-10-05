#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {

    // Declares a pointer to the input line
    char *line = NULL;

    // Stores the allocated size of the input buffer
    size_t len = 0;

    // Array of pointers to command arguments
    char *args[64];

    while (1) {

        // Display prompt
        printf("shellforge$ ");

        // Prevent output buffering
        fflush(stdout);

        // Read user input
        // Ctrl+D causes getline() to return -1
        if (getline(&line, &len, stdin) == -1) {
            break;
        }

        // Remove trailing newline
        line[strcspn(line, "\n")] = '\0';

        int i = 0;

        // Split input into tokens using space/tab
        char *token = strtok(line, " \t");

        // Store tokens in args[]
        while (token != NULL && i < 63) {
            args[i++] = token;
            token = strtok(NULL, " \t");
        }

        // Argument list must end with NULL
        args[i] = NULL;

        // Ignore empty input
        if (i == 0) {
            continue;
        }

        // Exit shell
        if (strcmp(args[0], "exit") == 0) {
            break;
        }

        // --- FORK CHILD GENERATION ENGINE ---
        pid_t pid = fork();

        if (pid == 0) {

            // Child process

            execvp(args[0], args);

            // Only reached if execvp() fails
            perror("Command execution error");
            exit(EXIT_FAILURE);
        }

        else if (pid > 0) {

            // Parent process
            // Wait for child to finish
            waitpid(pid, NULL, 0);
        }

        else {

            // fork() failed
            perror("Fork creation error");
        }
    }

    // Free getline() allocated memory
    free(line);

    return 0;
}
