/**
 * @file util.c
 * @author Onyx and Iris (code@onyxandiris.online)
 * @brief Utility functions.
 * @version 0.7.0
 * @date 2024-07-06
 *
 * @copyright Copyright (c) 2024
 * https://github.com/onyx-and-iris/vmrcli/blob/main/LICENSE
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "wrapper.h"
#include "util.h"

/**
 * @brief Removes the last part of a path
 *
 * @param fullpath The entire path
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
 * @brief Converts Voicemeeter's kind into a string.
 *
 * @param s Pointer to a character buffer
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
 * @param s Pointer to a character buffer
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