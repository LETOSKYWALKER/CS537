//referenced the blog https://c-for-dummies.com/blog/?p=1112 for getline() function defination and copied the buffer malloc test case in the example

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

//char path[10][120];
char* paths[120];   
int pathCount;

void error(){
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
}

void path(char* command){
    int i = 1;
    while(command[i]!=NULL){
        strcpy(paths[i-1], command[i]);
        i++
    }
    pathCount = i-2;
}

int execute(char* command[], char* destination[]){
    int i = 0;
    char* searchPaths[120];
    int usablePath = 0;
    int pid;
    int stat;
    int usable = 0;

    if(command[0] == NULL){
        return EXIT_FAILURE;
    }

    while(paths[i]!=NULL){
        if(!strcpy(searchPaths[i], paths[i])){//process search pathes
            error();
            return EXIT_FAILURE;
        }
        searchPaths[i][strlen(searchPaths[i])] = '/';
        searchPaths[i][strlen(searchPaths[i])+1] = '\0';
        strcat(searchPaths[i], paths[i]); //the basic use of strcat refered to https://www.tutorialspoint.com/c_standard_library/c_function_strcat.htm
        i++;
    }

    for(int idx = 0; idx < i; idx++){//find usable path
        if(access(searchPaths[idx], X_OK) == 0){
            usablePath = idx;
            usable = 1;
            break;
        }
    }

    if(usable == 0){//check if any path is usable
        error();
        return EXIT_FAILURE;
    }

    pid = fork(); //except inclass material, my use of fork also refered to https://www.geeksforgeeks.org/fork-system-call/
    if(pid<0){// check if fork is successful
        error();
        return EXIT_FAILURE;
    }else if(pid == 0){
        if(destination != NULL){
            int fd_out = open(destination[0],O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
            if (fd_out > 0){
                dup2(fd_out, STDOUT_FILENO);
                fflush(stdout);
            }
        }
        execv(searchPaths[usablePath], command);
    }else{
        waitpid(pid, &status, 0)
    }
    return 0;
}

void redirection(char* dest, char* gotline){
    dest[0] = '\0';
    dest = dest + 1; //ignore the ">" and go for the destination file
    int idx = 0;
    int index = 0;
    char* cmd[120];
    char* dst[120];
    char* delims = " \n\t";

     while((cmd[idx] = strtok(gotline, delims)) != NULL){
        idx++;
    } 

    if(cmd[0] == NULL){// check if we successfully split the line
        error();
        return;
    }

    while((dst[index] = strtok(dest, delims)) != NULL){
        index++;
    } 

    if(dst[0] == NULL){// check if we successfully split the line
        error();
        return;
    }
    execute(cmd, dst);
    return;
}

int read_command(char* user_input[], FILE* file){
    size_t bufsize = 120;//the select of buffer size refers to blog https://stackoverflow.com/questions/20036074/length-of-line-in-getline-function-c
    int result = 0;
    char* buffer  = (char *)malloc(bufsize * sizeof(char));
    //int iptct;
    int i = 0;
    char* delims = " \n\t";
    char* redest = NULL;

    if( buffer == NULL) //check if the malloc was successful
    {
        error();
        return EXIT_FAILURE;
    }

    if (getline(&buffer, &bufsize, file)== -1){//check if could get the line successfully
        error();
        return EXIT_FAILURE;
    }
    
    if((redest = strchr(buffer, '>')) != NULL){
        redirection(redest, buffer);
        return 0;
    }

    while((user_input[i] = strtok(buffer, delims)) != NULL){// this part splits the line we get into commands
        i++;
    } //the use of strtok() here referenced tutorial https://www.geeksforgeeks.org/strtok-strtok_r-functions-c-examples

    if(user_input[0] == NULL){// check if we successfully split the line
        error();
        return EXIT_FAILURE;
    }

    if (user_input[0] == EOF){//handle the case of encounter EOF maker
        exit(0);
    }

    if (strcmp(user_input[0], "exit") == 0){
        if (i != 1){
            error(); // check if user passes any other arg, if so, they are invalid
            return EXIT_FAILURE;

        }
        exit(0);
    }

    if(strcmp(user_input[0], "cd") == 0){
        if (i != 2){
            error(); // check if user passes any other arg, if so, they are invalid
            return EXIT_FAILURE;
        }
        int result = chdir(user_input[1]);
        if (result == -1){
            error(); // check if the cd was successful
            return EXIT_FAILURE;
        }
        return 0;
    }

    if(strcmp(user_input[0], "path") == 0){
        if (i < 1){
            error(); // check if user enough args
            return EXIT_FAILURE;
        }
        /*if(i == 1){
            return 0;
        }*/
        for(int idx = 0; idx <= pathCount; i++){
            free
            paths[idx] = NULL;
        }
        path(user_input);
        return 0;
    }
    
    return -1;
}

int main(int argc, char* argv[]){
    int loop_status = 1;
    int result;
    size_t bufsize = 120;
    char* input = (char *)malloc(bufsize * sizeof(char));
    FILE *file

    if( input == NULL) //check if the malloc was successful
        {
            error();
            exit(1);
        }

    while(loop_status){
        //fprintf(stdout, "wish> ");//before each interaction with user, print wish> at the start of lane
        if(argc == 1){//interactive mode
        fprintf(stdout, "wish> ");//before each interaction with user, print wish> at the start of lane
            result = read_command(input, stdin);

            if(result == 0){// which means a bulid in is called
                continue;
            }
            if(result == EXIT_FAILURE){
                break;
            }
            if(execute(input, NULL) == EXIT_FAILURE){// if an error occured, continue instead of break
                continue;
            }
            free(input);
        }else if (argc == 2){//batch mode
            file = fopen(argv[1], "r");
            if(file == NULL){
                error();
                exit(1);
            }
            
            while(1){
                result = read_command(input, file);
                if(result == 0){// which means a bulid in is called
                    continue;
                }   
                if(result == EXIT_FAILURE){
                    break;
                }
                if(execute(input, NULL) == EXIT_FAILURE){
                    continue;
                }
                free(input);
            }
            break;
        }else{//    the case when the number of arguments is not correct
            error();
            exit(1);
        }
    }

    return 0;
}