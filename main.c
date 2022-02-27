/*
 ============================================================================
 Name        : Simple Unix Shell
 Author      : Madison Sanchez-Forman
 Version     : 2.26.22
 Description : Uses functions defined in dsh.c to evaluate shell commands
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "dsh.h"

int main(int argc, char **argv)
{
    find_motd(); // print motd
    char cmd[MAXBUF], *history_list[HISTORY_LEN], *args[MAXBUF];

    int cmd_count = 0;
    while (1)
    {
        printf("dsh> ");
        fgets(cmd, sizeof(cmd), stdin);
        cmd[strcspn(cmd, "\n")] = 0; // remove new line character
        if (cmd == NULL)
        {
            ;
        }
        else
        {
            update_history(cmd, history_list, cmd_count); // update history
            cmd_count++;                                  // update history size
            if (!is_absolute(cmd) && !is_builtin(cmd))    // if not absolute or builtin run relative
            {
                relative(cmd);
            }
            else
            {
                parse_command(cmd, args); // else parse the command
                if (is_builtin(args[0]))
                {
                    if (strcmp(args[0], "exit") == 0)
                    {
                        return 0; // check for exit
                    }
                    else if (strcmp(args[0], "history") == 0) // check for history
                    {
                        for (int i = 0; i < cmd_count; i++)
                        {
                            printf("%s\n", history_list[i]);
                        }
                    }
                    builtin_handler(args); // else use builtin handler
                }
                else if (is_absolute(args[0])) // else its an absolute path
                {
                    absolute(args); // run it
                }
            }
        }
    }
}