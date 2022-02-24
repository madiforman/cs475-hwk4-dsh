/*
 * dsh.h
 *
 *  Created on: Aug 2, 2013
 *      Author: chiu
 */

#define MAXBUF 256
#define HISTORY_LEN 100

int find_motd();
int is_background(char command[]);
char **parse_command(char *cmd);

void dsh_launch(char **args);
int absolute(char **input);

int relative(char *input);
char **get_paths(char *token);

int check_paths(char *cmd);
char *concat(char *str1, char *str2);

int is_builtin(char *cmd);
int my_pwd();
int my_cd(char *input);
int my_exit();
int dsh_loop();
void builtin_handler(char **args);
int is_absolute(char *args);
// TODO: Your function prototypes below