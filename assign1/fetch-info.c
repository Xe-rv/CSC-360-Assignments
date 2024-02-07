/* getstats.c 
 *
 * CSC 360, Spring 2024
 *
 * - If run without an argument, prints information about 
 *   the computer to stdout.
 *
 * - If run with a process number created by the current user, 
 *   prints information about that process to stdout.
 *
 * Please change the following before submission:
 *
 * Author: Gong Rex
 * Login:  xerv@uvic.ca
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 1024

char infoBox[MAX][MAX]; // stores the info to be outputed
int lineNum = 0; // keeps count of the current ammount of info grabbed 

/*
 * Note: You are permitted, and even encouraged, to add other
 * support functions in order to reduce duplication of code, or
 * to increase the clarity of your solution, or both.
 */

// converts to uptime of the system in to d/h/m/s format to print
void get_uptime(char* time) {
    int seconds, minutes, hours, days;

    // gets the time from the file and converts it from char pointer to integer
    int totalSecs = atoi(time);

    // code from Sean Marrafa link: https://stackoverflow.com/a/28360796
    // converts the total seconds to days, hours, mins, secs before printing
    seconds = (totalSecs % 60);
    minutes = (totalSecs % 3600) / 60;
    hours = (totalSecs % 86400) / 3600;
    days = (totalSecs % (86400 * 30)) / 86400;

    printf("Uptime: %d days, %d hours, %d minutes, %d seconds\n", days, hours, minutes, seconds);
}

// adds the voluntary and involuntary context switch counts together to print
void get_total_ctxt(char* vol, char* nonvol) {
    // seperates the text from the counts
    vol = strtok(vol, "voluntary_ctxt_switches:\t");
    nonvol = strtok(nonvol, "nonvoluntary_ctxt_switches:\t");

    // adds them up then prints 
    int total = atoi(vol) + atoi(nonvol);

    printf("Total context switches: %d\n", total);
    lineNum--;
}

// gets the model name and number of cpu cores 
void get_info(char* file, char* name, char* delim) {
    // combines desired file with proc address for file access
    char pDir[100] = "/proc/";
    strcat(pDir, file);
    
    FILE* procFile = fopen(pDir, "r");
    char info[MAX];
    char infoOG[MAX];
    char* token;
    
    // goes through cpuinfo to get the name and core and stores them into the info array
    while (fgets(info, MAX, procFile) != NULL) {
        strcpy(infoOG, info);
        token = strtok(info, delim);

        if (name == NULL || strcmp(token, name) == 0) {
            sprintf(infoBox[lineNum], "%s", infoOG);
            lineNum++;
            break;
        }
    }

    // closes file
    fclose(procFile);
}


void print_process_info(char * process_num) {
    char check[MAX];
    char stat[MAX];
    char com[MAX];
    int i;

    // checks for the existence of the process number, prints error and ends program if it isn't
    sprintf(check, "/proc/%s", process_num);
    
    if(fopen(check, "r") == NULL) {
        printf("Process number %s not found\n", process_num);
        return;
    }
    
    sprintf(stat, "%s/status", process_num);
    sprintf(com, "%s/comm", process_num);
    
    // prints the process number first 
    printf("Process number: %s \n", process_num);

    // collects the info in order
    get_info(stat, "Name", ":");
    get_info(com, NULL, "");
    get_info(stat, "Threads", ":");
    get_info(stat, "voluntary_ctxt_switches", ":");
    get_info(stat, "nonvoluntary_ctxt_switches", ":");

    // prints the info in order and in proper format
   for(i = 0; i < lineNum; i++) {
       if(i == 1) {
            printf("Filename (if any): %s", infoBox[i]);
       } else if (i == 3){
            get_total_ctxt(infoBox[i], infoBox[i+1]);
       } else {
            printf("%s", infoBox[i]);
       }
   } 
} 

// prints the full info if no argument is passed
void print_full_info() { 
    int i;
    
    // collects the info in order
    get_info("cpuinfo", "model name", "\t");
    get_info("cpuinfo", "cpu cores", "\t");
    get_info("version", NULL, "");
    get_info("meminfo", "MemTotal", ":");
    get_info("uptime", NULL, " ");

    // prints the info in order and in proper format
    for(i = 0; i < lineNum; i++) {
        if (i == 4) {
            get_uptime(infoBox[i]);
        } else {
            printf("%s", infoBox[i]);
        }
    }
}


int main(int argc, char ** argv) {  
    if (argc == 1) {
        print_full_info();
    } else {
        print_process_info(argv[1]);
    }
}
