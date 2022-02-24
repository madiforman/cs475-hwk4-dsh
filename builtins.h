typedef enum {
	CMD_EXT,	//external command
	CMD_CD,		//cd
	CMD_PWD,	//pwd
	CMD_HIST,	//history
	CMD_EXIT,	//exit
	CMD_ECHO	//echo
} cmd_t;

/**
 * Checks whether given command string is a builtin
 * @param cmd
 * @return positive code if true, CMD_EXT otherwise
 */
cmd_t chk_builtin(char *cmd);
