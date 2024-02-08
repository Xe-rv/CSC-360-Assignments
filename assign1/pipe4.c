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

// dir and the 7 possible args
#define MAX_ARG 9 
#define MAX_LEN 80
#define MAX_CMD 4

int cmd_counter = 0; // keeps track of amount of inputs
char input[MAX_CMD][MAX_LEN]; // stores all of the users inputs

// tokenizes inputs and executes the command
void exe_tok(char* cmdline) {
    char* cmd_args[MAX_ARG]; // array of tokenized inputs
    char* token;
    int num_tokens = 0;

    // tokenizes the arguments and stores it for execution
    token = strtok(cmdline, " ");
    while(token != NULL && num_tokens < MAX_ARG - 1) {
        cmd_args[num_tokens] = token;
        num_tokens++;
        token = strtok(NULL, " ");
    }

    cmd_args[num_tokens] = NULL;

    // excutes the cmd
    execvp(cmd_args[0], cmd_args);
}

// stores up to 4 user inputs into an array
void get_inputs() {
    // stores user inputs till it hits max or until user enters to exit
    for(int i = 0; i < MAX_CMD; i++) {
        read(0, input[i], MAX_LEN);

        // leaves loop when user just enters without an input
        if(input[i][0] == '\n') {
            break;
        }

        input[i][strlen(input[i])-1] = '\0';
        cmd_counter++;
    }
}

int main() {
    int pipes;
    pid_t pid;
    int status;

    // gets all user inputs
    get_inputs();

    // number of pipes required depending on number of inputs
    pipes = cmd_counter - 1;
    int fd[pipes][2]; 

    // loops through each cmd and pipes output to the next cmd
    // does this till it gets to the final cmd where it outputs the final cmd
    for(int i = 0; i < cmd_counter; i++) {
        pipe(fd[i]);

        // creates the child 
        pid = fork();

        if(pid == 0) { // child pipe
            
            // redirects stdout to next pipe but only if its not the final cmd
            if(i < pipes) {
                dup2(fd[i][1], 1);
            } 
            
            // redirects stdin from previous pipe but only if its not the first cmd
            if (i > 0) {
                dup2(fd[i-1][0], 0);
            }

            // closes every child pipe
            for(int j = 0; j < pipes; j++) {
                close(fd[j][0]);
                close(fd[j][1]);
            }
            
            // tokenizes the input executes the cmd arguments 
            exe_tok(input[i]);
        } 
    }
    
    // closes every parent pipe
    for(int i = 0; i < pipes; i++) {
        close(fd[i][0]);
        close(fd[i][1]);
    }

    // waits for all the child processes to finish
    for(int i = 0; i < cmd_counter; i++) {
        waitpid(pid, &status, 0);
    }
}
