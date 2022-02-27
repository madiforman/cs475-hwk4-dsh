/* dsh.c Unix Shell in C
 * Author: Madison Forman | Version: 2/26/22 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "dsh.h"

int main(int argc, char **argv)
{
    find_motd(); // print motd
    char cmd[MAXBUF], *history_list[HISTORY_LEN];
    int cmd_count = 0;
    while (1)
    {
        printf("dsh> ");
        fgets(cmd, sizeof(cmd), stdin);
        cmd[strcspn(cmd, "\n")] = 0; // remove new line character

        update_history(cmd, history_list, cmd_count); // update history
        cmd_count++;                                  // update history size
        if (!is_absolute(cmd) && !is_builtin(cmd))
        {
            relative(cmd);
        }
        else
        {
            char **args = parse_command(cmd);
            if (is_builtin(args[0]))
            {
                if (strcmp(args[0], "exit") == 0)
                {
                    return 0;
                }
                else if (strcmp(args[0], "history") == 0)
                {
                    for (int i = 0; i < cmd_count; i++)
                    {
                        printf("%s\n", history_list[i]);
                    }
                }
                builtin_handler(args);
            }
            else if (is_absolute(args[0]))
            {
                absolute(args);
            }
        }
        // printf("\n");
    }
}