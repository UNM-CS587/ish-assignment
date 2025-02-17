#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <pwd.h>

#include "command.h"
#include "parser.h"

char name[256];
struct passwd* pwd;

void
issuePrompt(FILE *pf)
{
    fprintf(pf, "%s%%> ", name);
    fflush(pf);
}

void applyRedirection(command *cmd){

}

void changeDirectory(command *cmd){
    if(cmd->pa->nArgs == 1){
        printf("Returning to home directory\n");
        pwd = getpwent();
        chdir(pwd->pw_dir);
    }else{
        if(cmd->pa->nArgs > 2){
            fprintf(stderr, "Too many arguments\n");
        }else if(chdir(cmd->pa->azArgs[1]) != 0){
            perror("invalid path");
        }
    }
}

//requires a linked list data structure.
void setEnVars(){

}

void unsetEnVars(){

}

//requires a map data structure
void alias(){

}

void unalias(){

}

void processCommands(FILE *pf, int interactive)
{
    int eof;
    command *first = NULL;
    resetParser(pf);
    do {
        command *cmd;
        if (interactive) issuePrompt(stdout);
        first = cmd = nextCommand(&eof);
        while (cmd) {
            //Check all built-in commands first
            if (strcmp(cmd->zCmd, "cd") == 0){
                changeDirectory(cmd);
            }else if(strcmp(cmd->zCmd, "setenv") == 0){
                setEnVars();
            }else if(strcmp(cmd->zCmd, "unsetenv") == 0){
                unsetEnVars();
            }else if(strcmp(cmd->zCmd, "alias") == 0){
                alias();
            }else if(strcmp(cmd->zCmd, "unalias") == 0){
                unalias();
            }else if(strcmp(cmd->zCmd, "quit") == 0){
                exit(0);
            }else{
                //if the command given is not a builtin command, run it.
                int pid = fork();
                if(pid != 0){
                    waitpid(pid, NULL, 0);
                }else{
                    //basic cmd execution, need to handle redirections
                    //and environment passthrough variables.
                    char* envp = NULL;
                    //add string token to find the path for running the command
                    //must handle relative and absolute paths for a given command
                    execve(cmd->zCmd, cmd->pa->azArgs, &envp);
                }
            }
            //fprintf(stdout, "\n");
	    cmd = cmd->pcNext;
        }
        if (first) destroyCommand(first);
    } while (!eof);
}

int main(int argc, char **argv){
    if(0){
        //ishrc = fopen(.....)
	    //processCommands(ishrc, 0);
        //fclose(ishrc);
    }
    
    gethostname(name, sizeof(name));
	processCommands(stdin, 1);
	return 0;
}