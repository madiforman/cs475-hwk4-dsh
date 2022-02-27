/*
 * dsh.h
 *
 *  Created on: Aug 2, 2013
 *      Author: chiu
 */

#define MAXBUF 256
#define HISTORY_LEN 100

void find_motd();
int is_background(char *cmd);
char **parse_command(char *cmd);

void dsh_launch(char **args);
int absolute(char **input);

int relative(char *input);
char **get_paths(char *token);

int check_paths(char *cmd);

void my_pwd();
void my_cd(char *input);
int my_exit();
void builtin_handler(char **args);
int is_builtin(char *cmd);

void update_history(char *cmd, char *history[], int size);
int is_absolute(char *args);
