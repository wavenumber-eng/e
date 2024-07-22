#include "string.h"
#include "stdbool.h"
#include "stdarg.h"
#include "e.h"

#ifndef CONFIG__E_SHELL_PRINTF_MAX_LEN
	#define CONFIG__E_SHELL_PRINTF_MAX_LEN      (256U)
#endif

#ifndef CONFIG__E_SHELL_USE_HISTORY
	#define CONFIG__E_SHELL_USE_HISTORY 		(0U)
#endif

#ifndef CONFIG__E_SHELL_SEARCH_IN_HIST
	#define CONFIG__E_SHELL_SEARCH_IN_HIST 		(1U)
#endif

#ifndef CONFIG__E_SHELL_AUTO_COMPLETE
	#define CONFIG__E_SHELL_AUTO_COMPLETE 		(1U)
#endif

#ifndef CONFIG__E_SHELL_BUFFER_SIZE
	#define CONFIG__E_SHELL_BUFFER_SIZE 		(64)
#endif

#ifndef CONFIG__E_SHELL_MAX_ARGS
	#define CONFIG__E_SHELL_MAX_ARGS 			(8U)
#endif

#ifndef CONFIG__E_SHELL_HIST_MAX
	#define CONFIG__E_SHELL_HIST_MAX 			(4U)
#endif

#ifndef CONFIG__E_SHELL_MAX_CMD
	#define CONFIG__E_SHELL_MAX_CMD (32)
#endif



#define KEY_ESC (0x1BU)
#define KET_DEL (0x7FU)


static int32_t parse_line(e_shell__context_t ThisShellContext, const char *cmd, uint32_t len, char *argv[CONFIG__E_SHELL_MAX_ARGS]); /*!< parse line command */

static int32_t str_cmp(const char *str1, const char *str2, int32_t count); /*!< compare string command */

static void process_cmd(e_shell__context_t ThisShellContext, const char *cmd); /*!< process a command */

static void get_history(e_shell__context_t ThisShellContext, uint8_t hist_pos); /*!< get commands history */

static void autocomplete(e_shell__context_t ThisShellContext); /*!< auto complete command */

static int32_t StrLen(const char *str); /*!< get string length */

static char *StrCopy(char *dest, const char *src, int32_t count); /*!< string copy */

int32_t e_shell__printf(const e_shell__context_struct * shell,const char *FormatString,...)
{

	if(shell->tx_buffer != NULL)
	{
		char string_buffer[CONFIG__E_SHELL_PRINTF_MAX_LEN];

		va_list argptr;
		va_start(argptr,FormatString);
		vsnprintf((char *)string_buffer,CONFIG__E_SHELL_PRINTF_MAX_LEN,FormatString,argptr);
		va_end(argptr);


		shell->tx_buffer((uint8_t *)string_buffer,strnlen(string_buffer,CONFIG__E_SHELL_PRINTF_MAX_LEN));
	}

	return 0;
}


void e_shell__crunch(e_shell__context_t shell)
{
    uint8_t ch;
    int32_t i;

    if (!shell)
    {
        return;
    }

    if(shell->read_char == NULL)
    {
    	return;
    }
        while(shell->read_char(&ch)>=0)
        {
                /* Special key */
                if (ch == KEY_ESC)
                {
                    shell->stat = kSHELL_Special;
                    continue;
                }
                else if (shell->stat == kSHELL_Special)
                {
                    /* Function key */
                    if (ch == '[')
                    {
                        shell->stat = kSHELL_Function;
                        continue;
                    }
                    shell->stat = kSHELL_Normal;
                }
                else if (shell->stat == kSHELL_Function)
                {
                    shell->stat = kSHELL_Normal;

                    switch ((uint8_t)ch)
                    {
                        /* History operation here */
                        case 'A': /* Up key */
                            get_history(shell, shell->hist_current);
                            if (shell->hist_current < (shell->hist_count - 1))
                            {
                                shell->hist_current++;
                            }
                            break;
                        case 'B': /* Down key */
                            get_history(shell, shell->hist_current);
                            if (shell->hist_current > 0)
                            {
                                shell->hist_current--;
                            }
                            break;
                        case 'D': /* Left key */
                            if (shell->c_pos)
                            {
                                e_shell__printf(shell,"\b");
                                shell->c_pos--;
                            }
                            break;
                        case 'C': /* Right key */
                            if (shell->c_pos < shell->l_pos)
                            {
                                if(shell->echo){e_shell__printf(shell,"%c", shell->line[shell->c_pos]);};
                                shell->c_pos++;
                            }
                            break;
                        default:
                            break;
                    }
                    continue;
                }
                /* Handle tab key */
                else if (ch == '\t')
                {
        #if CONFIG__E_SHELL_AUTO_COMPLETE
                    /* Move the cursor to the beginning of line */
                    for (i = 0; i < shell->c_pos; i++)
                    {
                        e_shell__printf(shell,"\b");
                    }
                    /* Do auto complete */
                    autocomplete(shell);
                    /* Move position to end */
                    shell->c_pos = shell->l_pos = StrLen(shell->line);
        #endif
                    continue;
                }
        #if CONFIG__E_SHELL_SEARCH_IN_HIST
                /* Search command in history */
                else if ((ch == '`') && (shell->l_pos == 0) && (shell->line[0] == 0x00))
                {
                }
        #endif
                /* Handle backspace key */
                else if ((ch == KET_DEL) || (ch == '\b'))
                {
                    /* There must be at last one char */
                    if (shell->c_pos == 0)
                    {
                        continue;
                    }

                    shell->l_pos--;
                    shell->c_pos--;

                    if (shell->l_pos > shell->c_pos)
                    {
                        memmove(&shell->line[shell->c_pos], &shell->line[shell->c_pos + 1],
                                shell->l_pos - shell->c_pos);
                        shell->line[shell->l_pos] = 0;

                        if(shell->echo){e_shell__printf(shell,"\b%s  \b", &shell->line[shell->c_pos]);};

                        /* Reset position */
                        for (i = shell->c_pos; i <= shell->l_pos; i++)
                        {
                            if(shell->echo){e_shell__printf(shell,"\b");};

                        }
                    }
                    else /* Normal backspace operation */
                    {
                        if(shell->echo){e_shell__printf(shell,"\b \b");};
                        shell->line[shell->l_pos] = 0;
                    }
                    continue;
                }
                else
                {
                }

                /* Input too long */
                if (shell->l_pos >= (CONFIG__E_SHELL_BUFFER_SIZE - 1))
                {
                    shell->l_pos = 0;
                }

                /* Handle end of line, break */
                if ((ch == '\r') || (ch == '\n'))
                {
                    if(shell->echo){e_shell__printf(shell,"\r\n");};
                    process_cmd(shell, shell->line);
                    /* Reset all params */
                    shell->c_pos = shell->l_pos = 0;
                    shell->hist_current = 0;
                    if(shell->echo){e_shell__printf(shell,shell->prompt);};
                    memset(shell->line, 0, sizeof(shell->line));
                    continue;
                }

                /* Normal character */
                if (shell->c_pos < shell->l_pos)
                {
                    memmove(&shell->line[shell->c_pos + 1], &shell->line[shell->c_pos],
                            shell->l_pos - shell->c_pos);
                    shell->line[shell->c_pos] = ch;

                    if(shell->echo){e_shell__printf(shell,"%s", &shell->line[shell->c_pos]);};
                    /* Move the cursor to new position */
                    for (i = shell->c_pos; i < shell->l_pos; i++)
                    {
                        if(shell->echo){e_shell__printf(shell,"\b");};
                    }
                }
                else
                {
                    shell->line[shell->l_pos] = ch;
                    if(shell->echo){e_shell__printf(shell,"%c", ch);}
                }

                ch = 0;
                shell->l_pos++;
                shell->c_pos++;
        }

}


static void process_cmd(e_shell__context_t ThisShellContext, const char *cmd)
{
    static const e_shell__command_context_t *tmpCommand = NULL;
    static const char *tmpCommandString;
    int32_t argc;
    char *argv[CONFIG__E_SHELL_BUFFER_SIZE];
    uint8_t flag = 0;
    uint8_t tmpCommandLen;
    uint8_t tmpLen;
    uint8_t i = 0;

    tmpLen = StrLen(cmd);
    argc = parse_line(ThisShellContext,cmd, tmpLen, argv);

    if ((tmpCommand == NULL) && (argc > 0))
    {
        for (i = 0; i < ThisShellContext->ShellCommands.numberOfCommandInList; i++)
        {
            tmpCommand = &(ThisShellContext->ShellCommands.CommandList[i]);
            tmpCommandString = tmpCommand->pcCommand;
            tmpCommandLen = StrLen(tmpCommandString);


            /* Compare with space or end of string */
            if ((cmd[tmpCommandLen] == ' ') || (cmd[tmpCommandLen] == 0x00))
            {
                if (str_cmp(tmpCommandString, argv[0], tmpCommandLen) == 0)
                {
                    if (argc > 0)
                    {
                        flag = 1;
                        break;
                    }
                }
            }
        }
    }
    else
    {
        return;  //no command
    }

    if(flag == 0)
    {
        if(ThisShellContext->QuietOnBadCommand==0)
        {
            e_shell__printf(ThisShellContext,
                    "\r\n\"%s\" Command not recognized.  Enter 'help' to view a list of available commands.\r\n\r\n",cmd);
               
        }

         tmpCommand = NULL;
    }

    if(tmpCommand != NULL)
    {
        tmpLen = StrLen(cmd);
        /* Compare with last command. Push back to history buffer if different */
        if (tmpLen != str_cmp(cmd, ThisShellContext->hist_buf[0], StrLen(cmd)))
        {
            for (i = CONFIG__E_SHELL_HIST_MAX - 1; i > 0; i--)
            {
                memset(ThisShellContext->hist_buf[i], '\0', CONFIG__E_SHELL_BUFFER_SIZE);
                tmpLen = StrLen(ThisShellContext->hist_buf[i - 1]);
                StrCopy(ThisShellContext->hist_buf[i], ThisShellContext->hist_buf[i - 1], tmpLen);
            }
            memset(ThisShellContext->hist_buf[0], '\0', CONFIG__E_SHELL_BUFFER_SIZE);
            tmpLen = StrLen(cmd);
            StrCopy(ThisShellContext->hist_buf[0], cmd, tmpLen);
            if (ThisShellContext->hist_count < CONFIG__E_SHELL_HIST_MAX)
            {
                ThisShellContext->hist_count++;
            }
        }

        if(tmpCommand->pFuncCallBack == 0)
        {
            e_shell__printf(ThisShellContext,"\r\nCommand %s has a null callback\r\n\r\n",tmpCommand->pcCommand);
        }
        else
        {

           if(ThisShellContext->CurrentPrivilegeLevel >= tmpCommand->MinPrivilegeLevel)
                tmpCommand->pFuncCallBack((void *)ThisShellContext, argc, argv);
            else
            {
                if(ThisShellContext->QuietOnBadCommand==0)
                    e_shell__printf(ThisShellContext,"\r\nYou do not have privilege to run %s\r\n\r\n",tmpCommand->pcCommand);
            }
            
        }

        tmpCommand = NULL;
    }

}

static void get_history(e_shell__context_t ThisShellContext, uint8_t hist_pos)
{
    uint8_t i;
    uint32_t tmp;

    if (ThisShellContext->hist_buf[0][0] == '\0')
    {
        ThisShellContext->hist_current = 0;
        return;
    }
    if (hist_pos > CONFIG__E_SHELL_HIST_MAX)
    {
        hist_pos = CONFIG__E_SHELL_HIST_MAX - 1;
    }
    tmp = StrLen(ThisShellContext->line);
    /* Clear current if have */
    if (tmp > 0)
    {
        memset(ThisShellContext->line, '\0', tmp);
        for (i = 0; i < tmp; i++)
        {
            e_shell__printf(ThisShellContext,"\b \b");
        }
    }

    ThisShellContext->l_pos = StrLen(ThisShellContext->hist_buf[hist_pos]);
    ThisShellContext->c_pos = ThisShellContext->l_pos;
    StrCopy(ThisShellContext->line, ThisShellContext->hist_buf[hist_pos], ThisShellContext->l_pos);

    if(ThisShellContext->echo){e_shell__printf(ThisShellContext,ThisShellContext->hist_buf[hist_pos]);};
}

static void autocomplete(e_shell__context_t ThisShellContext)
{
    int32_t len;
    int32_t minLen;
    uint8_t i = 0;
    const e_shell__command_context_t *tmpCommand = NULL;
    const char *namePtr;
    const char *cmdName;

    minLen = 0;
    namePtr = NULL;

    if (!StrLen(ThisShellContext->line))
    {
        return;
    }
    if(ThisShellContext->echo){e_shell__printf(ThisShellContext,"\r\n");};
    /* Empty tab, list all commands */
    if (ThisShellContext->line[0] == '\0')
    {
        shell__help_handler(ThisShellContext, 0, NULL);
        return;
    }
    /* Do auto complete */
    for (i = 0; i < ThisShellContext->ShellCommands.numberOfCommandInList; i++)
    {
        tmpCommand = &ThisShellContext->ShellCommands.CommandList[i];
        cmdName = tmpCommand->pcCommand;
        if (str_cmp(ThisShellContext->line, cmdName, StrLen(ThisShellContext->line)) == 0)
        {
            if (minLen == 0)
            {
                namePtr = cmdName;
                minLen = StrLen(namePtr);
                /* Show possible matches */
                if(ThisShellContext->echo){e_shell__printf(ThisShellContext,"%s\r\n", cmdName);};
                continue;
            }
            len = str_cmp(namePtr, cmdName, StrLen(namePtr));
            if (len < 0)
            {
                len = len * (-1);
            }
            if (len < minLen)
            {
                minLen = len;
            }
        }
    }
    /* Auto complete string */
    if (namePtr)
    {
        StrCopy(ThisShellContext->line, namePtr, minLen);
    }
    if(ThisShellContext->echo){e_shell__printf(ThisShellContext,"%s%s", ThisShellContext->prompt, ThisShellContext->line);};
    return;
}

static char *StrCopy(char *dest, const char *src, int32_t count)
{
    char *ret = dest;
    int32_t i = 0;

    for (i = 0; i < count; i++)
    {
        dest[i] = src[i];
    }

    return ret;
}

static int32_t StrLen(const char *str)
{
    int32_t i = 0;

    while (*str)
    {
        str++;
        i++;
    }
    return i;
}

static int32_t str_cmp(const char *str1, const char *str2, int32_t count)
{
    while (count--)
    {
        if (*str1++ != *str2++)
        {
            return *(unsigned char *)(str1 - 1) - *(unsigned char *)(str2 - 1);
        }
    }
    return 0;
}

static int32_t parse_line(e_shell__context_t ThisShellContext,const char *cmd, uint32_t len, char *argv[CONFIG__E_SHELL_MAX_ARGS])
{
    uint32_t argc;
    char *p;
    uint32_t position;

    /* Init params */
    memset(ThisShellContext->g_paramBuffer, '\0', len + 1);
    StrCopy(ThisShellContext->g_paramBuffer, cmd, len);

    p = ThisShellContext->g_paramBuffer;
    position = 0;
    argc = 0;

    while (position < len)
    {
        /* Skip all blanks */
        while (((char)(*p) == ' ') && (position < len))
        {
            *p = '\0';
            p++;
            position++;
        }
        /* Process begin of a string */
        if (*p == '"')
        {
            p++;
            position++;
            argv[argc] = p;
            argc++;
            /* Skip this string */
            while ((*p != '"') && (position < len))
            {
                p++;
                position++;
            }
            /* Skip '"' */
            *p = '\0';
            p++;
            position++;
        }
        else /* Normal char */
        {
            argv[argc] = p;
            argc++;
            while (((char)*p != ' ') && ((char)*p != '\t') && (position < len))
            {
                p++;
                position++;
            }
        }
    }
    return argc;
}

int32_t e_shell__register_cmd(e_shell__context_t ThisShellContext,
                              char *pcCommand, 
                  char *pcHelpString,   
                              cmd_function_t  pFuncCallBack, 
                              uint32_t MinPrivilegeLevel    
                       )
{
    int32_t result = 0;

    /* If have room  in command list */
    if (ThisShellContext->ShellCommands.numberOfCommandInList < CONFIG__E_SHELL_MAX_CMD)
    {
        ThisShellContext->ShellCommands.CommandList[ThisShellContext->ShellCommands.numberOfCommandInList].pcCommand = pcCommand;
        ThisShellContext->ShellCommands.CommandList[ThisShellContext->ShellCommands.numberOfCommandInList].pcHelpString = pcHelpString;
        ThisShellContext->ShellCommands.CommandList[ThisShellContext->ShellCommands.numberOfCommandInList].pFuncCallBack = pFuncCallBack;
        ThisShellContext->ShellCommands.CommandList[ThisShellContext->ShellCommands.numberOfCommandInList].MinPrivilegeLevel = MinPrivilegeLevel;
        ThisShellContext->ShellCommands.numberOfCommandInList++;
    }
    else
    {
        result = -1;
    }
    return result;
}


cmd_function_t shell__help_handler(e_shell__context_t shell, int32_t argc, char **argv)
{
    uint32_t i = 0;

    e_shell__printf(shell,"\r\nCommands:");
    e_shell__printf(shell,"\r\n------------------------\r\n\r\n");

    for (i = 0; i < shell->ShellCommands.numberOfCommandInList; i++)
    {
        e_shell__printf(shell,E_VT100__YELLOW);

        
        e_shell__printf(shell,"%s",
                    shell->ShellCommands.CommandList[i].pcCommand);


        e_shell__printf(shell,E_VT100__WHITE);

        e_shell__printf(shell," > ");

        e_shell__printf(shell,E_VT100__CYAN);

        e_shell__printf(shell,"%s",
                        shell->ShellCommands.CommandList[i].pcHelpString);

        e_shell__printf(shell,"\r\n");
    }

    e_shell__printf(shell,E_VT100__DEFAULT);
    e_shell__printf(shell,"\r\n");
    return 0;
}

