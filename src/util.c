/**
 * @file util.c
 * @author Onyx and Iris (code@onyxandiris.online)
 * @brief Utility functions.
 * @version 0.13.0
 * @date 2024-07-06
 *
 * @copyright Copyright (c) 2024
 * https://github.com/onyx-and-iris/vmrcli/blob/main/LICENSE
 */

#include <stdio.h>
#include <string.h>
#include "util.h"
#include "log.h"

/**
 * @brief Removes the last part of a path
 *
 * @param fullpath
 */
void remove_last_part_of_path(char *fullpath)
{
    char *p;

    if ((p = strrchr(fullpath, '\\')) != NULL)
    {
        *p = '\0';
    }
}

/**
 * @brief Gets log level as int from string
 * @param level Log level as string
 * @return int Log level as int, or -1 if not found
 */
int log_level_from_string(const char *level)
{
    if (strcmp(level, "TRACE") == 0)
        return LOG_TRACE;
    else if (strcmp(level, "DEBUG") == 0)
        return LOG_DEBUG;
    else if (strcmp(level, "INFO") == 0)
        return LOG_INFO;
    else if (strcmp(level, "WARN") == 0)
        return LOG_WARN;
    else if (strcmp(level, "ERROR") == 0)
        return LOG_ERROR;
    else if (strcmp(level, "FATAL") == 0)
        return LOG_FATAL;
    else
        return -1;
}

/**
 * @brief Converts Voicemeeter's kind into a string.
 *
 * @param s Pointer to a character buffer receiving the kind
 * @param kind The kind of Voicemeeter.
 * @param n Maximum number of characters to be written to the buffer
 * @return char* String representation of the kind of Voicemeeter.
 */
char *kind_as_string(char *s, int kind, int n)
{
    static const char *kinds[] = {
        "Basic",
        "Banana",
        "Potato",
        "Basic x64",
        "Banana x64",
        "Potato x64",
    };
    snprintf(s, n, kinds[kind - 1]);
    return s;
}

/**
 * @brief Converts Voicemeeter's version into a string.
 *
 * @param s Pointer to a character buffer receiving the version
 * @param v Unprocessed version as a long int
 * @param n Maximum number of characters to be written to the buffer
 * @return char* String representation of the Voicemeeter version
 */
char *version_as_string(char *s, long v, int n)
{
    long v1 = (v & 0xFF000000) >> 24,
         v2 = (v & 0x00FF0000) >> 16,
         v3 = (v & 0x0000FF00) >> 8,
         v4 = (v & 0x000000FF);
    snprintf(s, n, "%i.%i.%i.%i", (int)v1, (int)v2, (int)v3, (int)v4);
    return s;
}

/**
 * @brief Is the current input a comment
 *
 * @param s Pointer to the current input
 * @return true
 * @return false
 */
bool is_comment(char *s)
{
    return s[0] == '#';
}

/**
 * @brief Searches the quickcommands array for a quickcommand
 * corresponding to the command_key.
 *
 * @param command_key The key used to search for the quickcommand
 * @param quickcommands Pointer to an array of quickcommands
 * @param n The number of quickcommands
 * @return struct quickcommand* Pointer to the found quickcommand
 * May return NULL if quickcommand not found.
 */
struct quickcommand *command_in_quickcommands(const char *command_key, const struct quickcommand *quickcommands, int n)
{
    for (int i = 0; i < n; ++i)
    {
        if (strcmp(command_key, quickcommands[i].name) == 0)
        {
            return (struct quickcommand *)(quickcommands + i);
        }
    }
    return NULL;
}

/**
 * @brief Adds quotes around the value part of a command if it contains spaces or tabs
 *
 * @param command The input command string (parameter=value format)
 * @param output Buffer to store the result
 * @param max_len Maximum length of the output buffer
 * @return true if quotes were added or command was copied successfully
 * @return false if the command is too long or invalid
 */
bool add_quotes_if_needed(const char *command, char *output, size_t max_len)
{
    const char *equals_pos = strchr(command, '=');
    
    // No '=' found, copy command as-is
    if (equals_pos == NULL) {
        if (strlen(command) >= max_len)
            return false;
        strcpy(output, command);
        return true;
    }

    const char *value = equals_pos + 1;
    
    // Value doesn't contain spaces or tabs, copy command as-is  
    if (strchr(value, ' ') == NULL && strchr(value, '\t') == NULL) {
        if (strlen(command) >= max_len)
            return false;
        strcpy(output, command);
        return true;
    }

    // Value needs quotes - calculate required buffer size
    size_t param_len = equals_pos - command;
    size_t value_len = strlen(value);
    size_t quotes_len = 2;
    size_t required_len = param_len + 1 + quotes_len + value_len + 1; // param + '=' + '"' + value + '"' + '\0'
    
    if (required_len > max_len)
        return false;
    
    /**
     * Construct the output string in the format: parameter="value"
     * - Copy the parameter part (up to the '=')
     * - Append '=' and opening quote
     * - Append the value
     * - Append closing quote and null terminator
     */
    char *pos = output;
    
    strncpy(pos, command, param_len);
    pos += param_len;
    
    *pos++ = '=';
    *pos++ = '"';
    
    strcpy(pos, value);
    pos += value_len;
    
    *pos++ = '"';
    *pos = '\0';
    
    return true;
}
