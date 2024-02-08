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
char input[MAX_CMD][MAX_LEN]; // stores all of the users inputs
int i;

// tokenizes inputs and executes the command
void exe_tok(char* cmdline) {
    char* cmd_args[MAX_ARG]; // array of tokenized inputs
    char* token;
    int num_tokens = 0;

    token = strtok(cmdline, " ");
    while(token != NULL && num_tokens < MAX_ARG - 1) {
        cmd_args[num_tokens] = token;
        num_tokens++;
        token = strtok(NULL, " ");
    }

    cmd_args[num_tokens] = NULL;

    execvp(cmd_args[0], cmd_args);
}

// stores up to 4 user inputs into an array
void get_inputs() {
    // stores user inputs till it hits max or until user enters to exit
    for(i = 0; i < MAX_CMD; i++) {
        read(0, input[i], MAX_LEN);

        // leaves loop when user just enters without an input
        if(input[i][0] == '\n') {
            break;
        }

        input[i][strlen(input[i])-1] = '\0';
        counter++;
    }
}

int main() {
    int pipes;
    pid_t pid;
    pid_t child_pids[MAX_CMD];
    int status;

    // gets all user inputs
    get_inputs();

    // number of pipes required depending on number of inputs
    pipes = counter - 1;
    int fd[2*pipes]; 

    // loops through each cmd and pipes output to the next cmd
    // does this till it gets to the final cmd where it outputs the final cmd
    for(i = 0; i < counter; i++) {
        pipe(fd + i * 2);

        // creates the child 
        pid = fork();

        if(pid == 0) { // child pipe
            
            // redirects stdout to next pipe but only if its not the final cmd
            if(i < pipes) {
                dup2(fd[i*2+1], 1);
            } 
            
            // redirects stdin from previous pipe but only if its not the first cmd
            if (i > 0) {
                dup2(fd[(i-1)*2], 0);
            }

            // closes every child pipe
            for(int j = 0; j < 2 * pipes; j++) {
                close(fd[j]);
            }
            
            // executes the cmd arguments 
            exe_tok(input[i]);
        }  else {// Store the process ID of the child process
            child_pids[i] = pid;
        }
        
    }
    
    // closes every parent pipe
    for(i = 0; i < 2 * pipes; i++) {
        close(fd[i]);
    }

    // waits for child process
    for(i = 0; i < counter; i++) {
        waitpid(child_pids[i], &status, 0);
    }
}
