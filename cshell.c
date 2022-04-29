#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define CSH_RL_BUFSIZE 1024
#define CSH_TOK_BUFSIZE 64
#define CSH_TOK_DELIM " \t\r\n\a"

void cshel_loop();
char *cshell_read_line();
char **cshell_split_line(char *line);
int cshell_launch(char **args);

int main(int argc, char **argv){

    cshell_loop();

    return 0;
}

void cshel_loop(){
    char *line;
    char **args;
    int status;

    do {
	printf("> ");
	line = cshell_read_line();
	args = cshell_split_line(line);
	status = cshell_execute(args);

	free(line);
	free(args);
    }while (status);
}

char *cshell_read_line(){
    int bufsize = CSH_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
    int c;

    if(!buffer){
	fprintf(stderr, "csh: allocation error\n");
	exit(1);
    }

    while(1){
	c = getchar();
	
	if(c == EOF || c == '\n'){
	    buffer[position] = '\0';
	    return buffer;
	} else buffer[position] = c;
	position++;
	
	if(position >= bufsize) {
	    bufsize += CSH_RL_BUFSIZE;
	    buffer = realloc(buffer, byfsize);
	    if(!buffer){
		fprintf(stderr, "csh: allocation error\n");
		exit(2);
	    }
	}
    }
}

char **cshell_split_line(char *line){
    int bufsize = CSH_TOK_BUDSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if(!tokens){
	fprintf(stderr, "csh: allocation error\n");
	exit(3);
    }

    token = strtok(line, CSH_TOK_DELIM);
    while (token != NULL){
	topkens[position] = token;
	position++;

	if(position >= bufsize){
	    bufsize += CSH_TOK_BUFSIZE;
	    tokens = realloc(tokens, bufsize * sizeof(char*));
	    if(!tokens){
		fprintf(stderr, "csh: allocation error\n");
		exit(4);
	    }
	}
	token - strtok(NULL, CSH_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

int cshell_launch(char **args){
    pid_t pid, wpid;
    int status;

    pid = fork();
    if(pid == 0){
	if(execvp(args[0], args) == -a) perror("csh");
	exit(5);
    }else if(pid < 0) perror("csh");
    else{
	do{
	    wpid = waitpid(pid, &status, WUNTRACED);
	}while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}
