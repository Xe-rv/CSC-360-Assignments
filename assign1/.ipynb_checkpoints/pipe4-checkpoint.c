/* gopipe.c
 *
 * CSC 360, Spring 2024
 *
 * Execute up to four instructions, piping the output of each into the
 * input of the next.
 *
 * Please change the following before submission:
 *
 * Author: Gong Rex
 * Login:  xerv@uvic.ca 
 */


/* Note: The following are the **ONLY** header files you are
 * permitted to use for this assignment! */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <wait.h>

// for debugging
#include <stdio.h>

#define MAX_ARG 8
#define MAX_LEN 80
#define MAX_CMD 4

int counter = 0; // keeps track of amount of inputs
char* cmd_args[MAX_CMD][MAX_ARG]; // array of tokenized inputs
int i;

void arg_tok(char* cmdline) {
    char* token;
    int num_tokens = 0;

    token = strtok(cmdline, " ");
    while(token != NULL && num_tokens < MAX_ARG - 1) {
        cmd_args[counter][num_tokens++] = token;
        token = strtok(NULL, " ");
    }

    cmd_args[counter][num_tokens] = NULL;
    
}

// stores up to 4 user inputs into an array
void get_inputs() {
    char input[MAX_CMD][MAX_LEN]; // array of user inputs
    
    // stores user inputs till it hits max or until user enters to exit
    for(i = 0; i < MAX_CMD; i++) {
        read(0, input[i], MAX_LEN);

        // leaves loop when user just enters without an input
        if(input[i] == NULL || strcmp(input[i], "\n") == 0) {
            break;
        }

        input[i][strlen(input[i])-1] = '\0';
        arg_tok(input[i]);
        counter++;
    }
}

int main() {
    int fd[counter][2]; // number of pipes required depending on number of inputs
    pid_t pid;
    int status;

    // gets all user inputs
    get_inputs();

    for(i = 0; i < counter - 1; i++) {
        pipe(fd[i]);
    }

    // loops through each cmd and pipes output to the next cmd
    // does this till it gets to the final cmd where it outputs the final cmd
    for(i = 0; i < counter; i++) {
        pid = fork();
        if(pid == 0) {
            // redirects stdout to pipe out
            if (i > 0) {
                dup2(fd[i][0], 0);
                close(fd[i][0]);
                close(fd[i][1]);
            }

            // redirects stdin to pipe in
            if(i < counter - 1) {
                dup2(fd[i+1][1], 1);
                close(fd[i+1][1]);
                close(fd[i+1][0]);
            } 
            
            // executes the cmd arguments 
            execvp(cmd_args[i][0], cmd_args[i]);
        }
        
    }
    
    // closes every parent pipe
    for(i = 0; i < counter - 1; i++) {
        close(fd[i][0]);
        close(fd[i][1]);
    }

    // waits for child process
    for(i = 0; i < counter; i++) {
        waitpid(pid, &status, 0);
    }
}
