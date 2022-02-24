/*
 * dsh.c
 *
 *  Created on: Aug 2, 2013
 *      Author: chiu
 */
#include "dsh.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <errno.h>
#include <err.h>
#include <sys/stat.h>
#include <string.h>
#include "builtins.h"

#define DELIM " "
#define SLASH "/"

// TODO: Your function definitions (declarations in dsh.h)
/**
 * @brief find_motd() searches for .dsh_motd using getenv(). If the file is not found,
 * it returns 0 and exits. If it is found, the MOTD is printed to the screen and 1 is returned.
 * @return int 0 if file not found, 1 if file is found. 
 */
int find_motd()
{   
    char file_name[] = ".dsh_motd";
	const char *home = getenv("HOME");
	if(home){
	
	size_t size = strlen(home)+strlen(file_name);
	char *path = malloc(size);
    char c;
	strcpy(path, home);
	strcpy(path, file_name);
	if(0==access(path, F_OK)){ /*file has been found*/
		FILE *fptr = fopen(file_name,"r");
        if(fptr==NULL){        /*if the file was found but couldnt be opened return 0*/
            return 0;   
        }
        c = fgetc(fptr);
        while(c != EOF){
            printf("%c",c);
            c = fgetc(fptr);
        }
        printf("\n");
        fclose(fptr);
        return 1;
	}else {
        return 0;
	}
    } 
    return 0;
}

int is_background(char command[])
{
    int len = strlen(command);
    if(command[len-1]=='&'){
        return 1;
    }else {
        return 0;
    }
}


char **parse_command(char *cmd)
{
char **tokens = malloc(MAXBUF*sizeof(char*));
char *token;
int i = 0;

if(!tokens){
    printf("Error: allocation of memory failed.\n");
    return NULL;
}

token = strtok(cmd, " \n");
while(token != NULL){
    
    tokens[i]=token;
    i++;
    token = strtok(NULL, " \n");
    
}
tokens[i] = NULL;
return tokens;
}

    

void dsh_launch(char **args){
//printf("%s , %s\n", args[0], args[1]);
pid_t cpid = fork();

if(cpid == 0 ){
    int flag = execvp(args[0], args);
    if(flag<0){
        printf("Error: 95 %s\n", args[0]);
    }
} else {
    int status;
    waitpid(cpid,&status,0);
    return;
}
}

int absolute(char **args)
{
if(access(args[0], F_OK|X_OK)==0) {
   //printf("104: %d\n", access(temp[0], F_OK|X_OK));
  // printf("114: command: %s, arg: %s\n", temp[0], temp[1]);
dsh_launch(args);
    return 1;
} else {
   //printf("109: %s\n",input);
   //printf("110: %d\n", access(input, F_OK|X_OK));
    return 0;
}

}
int is_absolute(char *args){
    if(args[0]=='/'){
        return 1;
    }else {return 0;}
}
int dsh_loop(){
    char cmd[MAXBUF];
    while(1){
        printf("dsh> ");
        fgets(cmd, sizeof(cmd), stdin);
        cmd[strcspn(cmd, "\n")]=0;
        if(!is_absolute(cmd) && !is_builtin(cmd)){
            printf("139: %s\n", cmd);
            relative(cmd);
        }else {
            char **args = parse_command(cmd);
            if(is_builtin(args[0])){
                if(strcmp(args[0], "exit")==0){
                    return 0;
                }
                builtin_handler(args);
            } else if(is_absolute(args[0])){
                printf("is absolute\n");
                absolute(args);
            }
        }
    }
    return 0;
}
int relative(char *input)
{
   char c = '/';//use constant
   char cwd[MAXBUF];
   getcwd(cwd, sizeof(cwd));
   
   strncat(cwd, &c,1); //change to concat
   strcat(cwd, input);
    
   char **temp = parse_command(cwd);
   absolute(temp);
   printf("about to check paths\n");
        if(check_paths(input)){
            return 1;
        } else {
            return 0;
           printf("Error: command not found.");
        }

}

char **get_paths(char *cmd)
{
int i = 0;
char *token, *path = getenv("PATH"), **paths = malloc(MAXBUF*sizeof(char*));
char temp_path[MAXBUF];
memcpy(temp_path, path, strlen(path));

//printf("161: %s\n", temp_path);

token = strtok(temp_path, ":");

while(token!=NULL){
    paths[i]=token;
    token = strtok(NULL, ":");  
    i++;  
}

paths[i]=NULL;

int j = 0;
while(paths[j]!=NULL){
       paths[j] = concat(paths[j], SLASH);
       paths[j] = concat(paths[j], cmd);
       j++;
}
// for(int i = 0; i <sizeof(paths);i++){
//     printf("185: %s\n", paths[i]);
// }
return paths;
}

int check_paths(char *cmd)
{
char **all_paths = get_paths(cmd);
int i = 0;
while(all_paths[i]!=NULL){
    char **temp = parse_command(all_paths[i]);
    if(absolute(temp)){
        return 0;
    }
    i++;
}
return -1;
}


char *concat(char *str1, char *str2)
{
int len = 0, i = 0, j=0;
len = strlen(str1) + strlen(str2);
char *temp = malloc(sizeof(char)*(len));
while(str1[i]!='\0'){
    temp[i] = str1[i];
    i++;
}
while(str2[j]!='\0'){
    temp[i++]=str2[j++];
}
temp[i]='\0';
return temp;
}
int is_builtin(char *cmd){
    
    if(strncmp(cmd,"cd",2)==0||strcmp(cmd, "exit")==0||strcmp(cmd, "pwd")==0){
        return 1;
    } else { 
        return 0; 
        }
}


int my_pwd()
{
    char temp[MAXBUF];
    if(getcwd(temp, sizeof(temp))!=NULL)
    {
        printf("%s\n", temp);
    } 
    return 1;
}

int my_cd(char *dest)
{   
int flag = chdir(dest);
if(flag == 0){
    my_pwd();
    return 0;
} else {
    printf("Error: directory '%s' could not be found.\n", dest);
    return 1;
    }
}


void builtin_handler(char **args)
{
if(chk_builtin(args[0])==CMD_PWD){
    my_pwd();
} else if(chk_builtin(args[0])==CMD_CD){
    my_cd(args[1]);
}
}