#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "command.h"
#include "parser.h"

char name[256];

void
issuePrompt(FILE *pf)
{
    fprintf(pf, "%s> ", name);
    fflush(pf);
}

void applyRedirection(command *cmd){

}

void runCommand(command *cmd){

}

void changeDirectory(command *cmd){
    printf("command: ");
    printCommand(cmd, stdout);
    printf("\nArguments:");
    printArguments(cmd->pa, stdout);
    printf("\n");
}

void setEnVars(){

}

void unsetEnVars(){

}

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
                //if the command given is not a built in command, run it.

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