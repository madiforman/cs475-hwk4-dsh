/*
 ==============================================================================
 Name        : dsh.c
 Author      : Madison Sanchez-Forman
 Version     : 2.26.22
 Description : contains code for prototypes defined in dsh.h
    fucntions handle basic relative/absolute/builtin commands. Supports running
    processes in the background denoted by & at the end of input
 ==============================================================================
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

int bckgrnd_flag = 0; // goes high when last char is &
/**
 * @brief find_motd() searches for .dsh_motd using getenv(). If it is found, the MOTD is printed to the screen
 */
void find_motd()
{
    FILE *fptr;
    fptr = fopen(".dsh_motd", "r");
    char c;
    if (fptr != NULL)
    {
        c = fgetc(fptr);
        while (c != EOF)
        {
            printf("%c", c);
            c = fgetc(fptr);
        }
    }
    printf("\n");
    fclose(fptr);
}
/**
 * @brief is_background checks if the second to last char in a string is a '&'
 *
 * @param command string to check
 * @return int 1 if an '&' is found 0 if not
 */
int is_background(char *cmd)
{
    int len = strlen(cmd);
    if (cmd[len - 1] == '&')
    {
        cmd[strcspn(cmd, "&")] = '\0';
        return 1;
    }
    else
    {
        return 0;
    }
}
/**
 * @brief parse_command() takes a string and tokenizes it by " ". it returns an array where each index contains the token in between delim
 * @param cmd line to parse
 * @return char** parsed line as char array
 */
void parse_command(char *cmd, char *parsed[])
{
    int i = 0;

    if (is_background(cmd)) // check for background execution
    {
        bckgrnd_flag = 1;
    }
    char *token = strtok(cmd, " ");
    while (token != NULL)
    {
        parsed[i] = token;
        i++;
        token = strtok(NULL, " ");
    }
    parsed[i] = NULL;
    int j = 0;
    while (parsed[j] != NULL)
    {
        j++;
    }
}
/**
 * @brief dsh_launch forks a new child to begin a process
 *
 * @param args char array parsed from parse_command()
 */
int dsh_launch(char **args)
{
    pid_t cpid = fork();

    if (cpid == 0)
    {
        int flag = execvp(args[0], args);
        if (flag < 0)
        {
            printf("%s: cannot access '%s': No such file or directory\n", args[0], args[1]);
            return 0;
        }
    }
    if (!bckgrnd_flag)
    {
        int status;
        waitpid(cpid, &status, 0);
    }
    return 1;
}
/**
 * @brief absolute() checks if a command is accessible, if it is it calls dsh_launch along with any arguments
 * @param args char array parsed from parse_command()
 * @return int 1 if launch successful 0 if not
 */
void absolute(char **args)
{
    if (access(args[0], F_OK | X_OK) == 0)
    {
        dsh_launch(args);
    }
}
/**
 * @brief is_absolute() checks if the first char in an input is "/"
 *
 * @param input line from user
 * @return int 1 if first char is "/", 0 if not
 */
int is_absolute(char *input)
{
    if (input[0] == '/')
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/**
 * @brief relative first checks if the path to a relative command is in the cwd,
 * if not, it uses get_path() to find the path of the executable and then launches it.
 * @param input string command
 */
void relative(char *input)
{
    // char c = '/';
    // char cwd[MAXBUF];

    // getcwd(cwd, sizeof(cwd)); // first check to see if path to relative is in cwd
    // strncat(cwd, &c, 1);
    // strcat(cwd, input);
    // char *check_cwd[MAXBUF];
    // parse_command(cwd, check_cwd);
    // //this line is commented out because when running in the background, it is causing my output to be
    // // dsh_launch(check_cwd); // if launch executes execv will be called and other code will be ignored

    char *args[MAXBUF]; // else its not in the cwd and we have to find its path if it exists
    parse_command(input, args);
    args[0] = get_path(input);
    dsh_launch(args);
}
/**
 * @brief check paths checks a possible path with command user input.
 * It is used over multiple string paths in get_path()
 * @param cmd command input
 */
int check_path(char *path)
{

    if (access(path, F_OK | X_OK) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/**
 * @brief get_path() builds all possible paths based on $PATH and then concatenates each
 * with the users input, it then calls check path on each full path and returns the correct path if
 * check_path() returns true. If this never happens, null is returned.
 * @param cmd user input
 * @return char** all full paths with user input
 */
char *get_path(char *cmd)
{
    int i = 0;
    char *token, *path = getenv("PATH");
    char temp_path[MAXBUF], *paths[MAXBUF];

    memcpy(temp_path, path, strlen(path));

    token = strtok(temp_path, ":");
    while (token != NULL)
    {
        paths[i] = token;
        token = strtok(NULL, ":");
        i++;
    }
    paths[i] = NULL;
    int j = 0;

    while (paths[j] != NULL)
    {
        char temp[MAXBUF];
        strcpy(temp, paths[j]);
        strcat(temp, "/");
        strcat(temp, cmd);
        if (check_path(temp))
        {
            cmd = strdup(temp);
            return cmd;
        }
        j++;
    }
    return NULL;
}
/**
 * @brief is_builtin does a str comparison on input to check if input is a builtin command
 *
 * @param cmd input string
 * @return int 1 if builtin 0 if not
 */
int is_builtin(char *cmd)
{
    if (strncmp(cmd, "cd", 2) == 0 || strcmp(cmd, "exit") == 0 || strcmp(cmd, "pwd") == 0 || strcmp(cmd, "history") == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/**
 * @brief my_pwd prints the working directory
 */
void my_pwd()
{
    char temp[MAXBUF];
    if (getcwd(temp, sizeof(temp)) != NULL)
    {
        printf("%s\n", temp);
    }
}
/**
 * @brief my_cd calls chdir and changes the working directory
 *
 * @param dest destination for chdir()
 */
void my_cd(char *dest)
{
    int flag = chdir(dest);
    if (flag != 0)
    {
        printf("dsh: No such file or directory.\n");
    }
}
/**
 * @brief update history updates the history of commands input
 *
 * @param cmd cmd to be stored
 * @param history array that holds the commands
 * @param size size of history
 */
void update_history(char *cmd, char *history[], int size)
{

    if (size < HISTORY_LEN)
    {
        char *temp = strdup(cmd);
        strcpy(temp, cmd);
        history[size] = temp;
    }
}
/**
 * @brief builtin_handler calls either my_pwd() or my_cd()
 *
 * @param args parsed array of input
 */
void builtin_handler(char **args)
{
    if (chk_builtin(args[0]) == CMD_PWD)
    {
        my_pwd();
    }
    else if (chk_builtin(args[0]) == CMD_CD)
    {
        my_cd(args[1]);
    }
}
