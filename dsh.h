/*
 ============================================================================
 Name        : dsh.h
 Author      : Madison Sanchez-Forman
 Version     : 2.26.22
 Description : Contains prototypes for functions in dsh.c
 ============================================================================
 */
#define MAXBUF 256
#define HISTORY_LEN 100

void find_motd();
/************* Functions used to parse and launch commands *************/
void parse_command(char *cmd, char *parsed[]);
void dsh_launch(char **args);
void absolute(char **input);
void relative(char *input);
/************* Fucntions used to find paths of relative commands *************/
char *get_path(char *cmd);
int check_path(char *path);
/************* Functions used for builtin commands *************/
void update_history(char *cmd, char *history[], int size);
void my_pwd();
void my_cd(char *input);
int my_exit();
void builtin_handler(char **args);
/************* Helper Functions *************/
int is_builtin(char *cmd);
int is_background(char *cmd);
int is_absolute(char *args);
