#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define CSH_RL_BUFSIZE 1024
#define CSH_TOK_BUFSIZE 64
#define CSH_TOK_DELIM " \t\r\n\a"

void cshell_loop();
char *cshell_read_line();
char **cshell_split_line(char *line);
int cshell_launch(char **args);
int csh_cd(char **args);
int csh_help(char **args);
int csh_exit(char **args);
int csh_num_builtins();
int cshell_execute(char **args);

char *bultin_str[] = {
    "cd",
    "help",
    "exit"
};

int(*bultin_func[]) (char **) = {
    &csh_cd,
    &csh_help,
    &csh_exit
};

int main(int argc, char **argv){

    cshell_loop();

    return 0;
}

void cshell_loop(){
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
	    buffer = realloc(buffer, bufsize);
	    if(!buffer){
		fprintf(stderr, "csh: allocation error\n");
		exit(2);
	    }
	}
    }
}

char **cshell_split_line(char *line){
    int bufsize = CSH_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if(!tokens){
	fprintf(stderr, "csh: allocation error\n");
	exit(3);
    }

    token = strtok(line, CSH_TOK_DELIM);
    while (token != NULL){
	tokens[position] = token;
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
	if(execvp(args[0], args) == -1) perror("csh");
	exit(5);
    }else if(pid < 0) perror("csh");
    else{
	do{
	    wpid = waitpid(pid, &status, WUNTRACED);
	}while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int csh_num_builtins(){
    return sizeof(bultin_str) / sizeof(char *);
}


int csh_cd(char **args){
    if (args[1] == NULL) fprintf(stderr, "csh: expected argument to \"cd\"\n");
    else{
	if (chdir(args[1]) != 0) perror("csh");
    }
    return 1;
}

int csh_help(char **args){
    int i;
    printf("Aldin Ugljanin's CSH\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    for(i = 0; i< csh_num_builtins(); i++){
	printf("   %s\n", bultin_str[i]);
    }
    printf("Use the man command for information on other programs.\n");
    return 1;

}

int csh_exit(char **args){
    return 0;
}

int cshell_execute(char **args){
    int i;
    if (args[0] == NULL) return 1;

    for(i = 0; i < csh_num_builtins(); i++){
	if (strcmp(args[0], bultin_str[i]) == 0) return (*bultin_func[i])(args);
    }

    return cshell_launch(args);
}
