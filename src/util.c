#include <stddef.h>
#include <stdio.h>
#include "vmr.h"
#include "util.h"

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
 * @param s the string to be reduced
 * @param len current length of the string
 * @return int new length of the string
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

char *kind_as_string(char *s, enum kind kind, int n)
{
    char *kinds[] = {
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
 * @param s string buffer the version will be written to
 * @param v unprocessed version as a long int
 * @param n maximum number of characters to be written to the buffer
 * @return char*
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