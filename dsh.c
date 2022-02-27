/* dsh.c Unix Shell in C
 * Author: Madison Forman | Version: 2/26/22 */
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

#define DELIM " \n"
#define SLASH "/"
int bckgrnd_flag = 0;
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
int is_background(char *command)
{
    int len = strlen(command);
    if (command[len - 1] == '&')
    {
        command[strcspn(command, "&")] = 0;
        return 1;
    }
    else
    {
        return 0;
    }
}
/**
 * @brief parse_command() takes a string and tokenizes it by DELIM. it returns an array where each index contains the token in between delim
 * @param cmd line to parse
 * @return char** parsed line as char array
 */
char **parse_command(char *cmd)
{
    int i = 0, buf = MAXBUF;
    char **tokens = malloc(buf * sizeof(char *)), *token;

    if (!tokens)
    {
        printf("Error: allocation of memory failed.\n");
        return NULL;
    }
    if (is_background(cmd))
    {
        bckgrnd_flag = 1;
        cmd[strlen(cmd) - 1] = '\0';
    }
    token = strtok(cmd, DELIM);
    while (token != NULL)
    {
        tokens[i] = token;
        i++;
        token = strtok(NULL, DELIM);
    }
    tokens[i] = NULL;
    return tokens;
}
/**
 * @brief dsh_launch forks a new child to begin a process
 *
 * @param args char array parsed from parse_command()
 */
void dsh_launch(char **args)
{
    pid_t cpid = fork();

    if (cpid == 0)
    {
        int flag = execvp(args[0], args);
        if (flag < 0)
        {
            printf("Error forking. Please try again.\n");
        }
    }
    else if (!bckgrnd_flag)
    {
        int status;
        waitpid(cpid, &status, 0);
    }
}
/**
 * @brief absolute() checks if a command is accessible, if it is it calls dsh_launch along with any arguments
 *
 * @param args char array parsed from parse_command()
 * @return int 1 if launch successful 0 if not
 */
int absolute(char **args)
{
    if (access(args[0], F_OK | X_OK) == 0)
    {
        dsh_launch(args);
        return 1;
    }
    else
    {
        return 0;
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
 * if not it calls check_paths() which will build an array of paths and call absolute
 * on each one
 *
 * @param input string command
 */
int relative(char *input)
{
    char c = '/'; // use constant
    char cwd[MAXBUF];
    input[strcspn(input, "&")] = 0;

    getcwd(cwd, sizeof(cwd));
    strncat(cwd, &c, 1);
    strcat(cwd, input);
    char **temp = parse_command(cwd);
    absolute(temp);
    if (check_paths(input))
    {
        return 1;
    }
    return 0;
}
/**
 * @brief check paths checks all possible paths with command user input
 * @param cmd command input
 */
int check_paths(char *cmd)
{
    char **all_paths = get_paths(cmd);
    int i = 0;
    while (all_paths[i] != NULL)
    {
        char **temp = parse_command(all_paths[i]);
        if (absolute(temp))
        {
            return 1;
        }
        i++;
    }
    return 0;
}
/**
 * @brief get_paths() builds all possible paths based on $PATH and then concatenates each
 * with the users input
 * @param cmd user input
 * @return char** all full paths with user input
 */
char **get_paths(char *cmd)
{
    int i = 0, buf = 100000;
    char *token, *path = getenv("PATH"), **paths = malloc(buf * sizeof(char *));
    char temp_path[MAXBUF];
    memcpy(temp_path, path, strlen(path));

    token = strtok(temp_path, ":");
    while (token != NULL)
    {
        paths[i] = token;
        token = strtok(NULL, ":");
        i++;
    }
    paths[i] = NULL;

    char *temp;
    for (int j = 0; j < sizeof(paths); j++)
    {
        if (paths[j] != NULL)
        {
            temp = strdup(paths[j]); // duplicate input
            strcat(temp, SLASH);
            strcat(temp, cmd);

            paths[j] = temp;
        }
    }
    return paths;
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
        char *temp = (char *)malloc(sizeof(cmd) * sizeof(char));
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
