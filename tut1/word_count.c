#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
    FILE* nufile = fopen(argv[1], "r");
    char str[1000];
    char* token;
    int count = 0;
    
    while(fgets(str, 1000, nufile) != NULL){
        token = strtok(str, " ");
        while(token != NULL && strcmp(token, "\n") != 0){
            count++;
            token = strtok(NULL, " ");
        }
    }

    printf("%d %s\n", count, argv[1]);
    
    fclose(nufile);
}