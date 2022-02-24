/*
 * main.c
 *
 *  Created on: Mar 17 2017
 *      Author: david
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "dsh.h"

int main(int argc, char **argv)
{
   // printf("lol\n");
    return dsh_loop();
    //char cmd[] = "ls -l";
   // relative(cmd);
   // my_pwd();
//    if(access(cmd, F_OK|X_OK)==0) {
//        printf("yay\n");
//    }
    //absolute(cmd);
    //relative("ls -a");
    //char *path = "..";
   // my_cd(path);
    // char **agrs = get_paths(cmd);
    // for(int i = 0; i < sizeof(agrs);i++){
    //     printf("main: %s\n", agrs[i]);
    // }

/*
while(1){
    if .dsh_motd exists {
        print
    } else {
        get line of input;
        if(input[0]=='/)
        {
             if path to file exists
            {
                 dsh_launch();
            } else
            {
                print error;
                restart loop;
            }
        }else 
        {
          if(input is builtin)
          {
              run command
              restart loop
          }  else {
              split $PATH on ':'
              while(more paths)
                concatenate next path with command
              if(pathto file exists)
              {
                  dsh_launch
                  restart loop
              }
          }
        }
    }


}
*/

   

}