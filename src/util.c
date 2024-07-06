/**
 * @file util.c
 * @author Onyx and Iris (code@onyxandiris.online)
 * @brief Utility functions.
 * @version 0.5.0
 * @date 2024-07-06
 *
 * @copyright Copyright (c) 2024
 * https://github.com/onyx-and-iris/vmrcli/blob/main/LICENSE
 */

#include <stddef.h>
#include <stdio.h>
#include "wrapper.h"
#include "util.h"

/**
 * @brief Removes the last part of a path
 *
 * @param szPath Pointer to the path string
 */
void remove_name_in_path(char *szPath)
{
    char *p = szPath;

    while (*p++)
        ;
    while (p > szPath && *p != '\\')
        p--;
    if (*p == '\\')
        *p = '\0';
}

/**
 * @brief replaces multiple newlines and tabs with single spaces
 *
 * @param s The tring to be reduced
 * @param len Current length of the string
 * @return int New length of the string
 */
int replace_multiple_space_with_one(char *s, size_t len)
{
    int j = 0;
    int count = 0;

    if (len == 1 && (s[0] == ' ' || s[0] == '\t'))
    {
        s[0] = '\0';
        return len;
    }

    if (len < 2)
        return len;

    for (int i = 0; s[i] != '\0'; i++)
    {
        if (s[i] == ' ' || s[i] == '\t')
        {
            count++;
        }

        if (s[i] != ' ' && s[i] != '\t')
        {
            if (count >= 1)
            {
                count = 0;
                s[j++] = ' ';
            }
            s[j++] = s[i];
        }
    }
    s[j] = '\0';
    return j;
}

/**
 * @brief
 *
 * @param s Pointer to a character buffer
 * @param kind The kind of Voicemeeter.
 * @param n Maximum number of characters to be written to the buffer
 * @return char* The kind of Voicemeeter as a string
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
 * @brief returns Voicemeeter's version as a string
 *
 * @param s Pointer to a character buffer
 * @param v Unprocessed version as a long int
 * @param n Maximum number of characters to be written to the buffer
 * @return char* Pointer to a character buffer
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