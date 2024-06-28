#include <stddef.h>
#include <stdio.h>
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

void replace_multiple_space_with_one(char *s, size_t len)
{
    int j = 0;
    int count = 0;

    if (len == 1 && (s[0] == ' ' || s[0] == '\t'))
    {
        s[0] = '\0';
        return;
    }

    if (len < 2)
        return;

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
}

char *version_as_string(char *s, long v, int n)
{
    long v1 = (v & 0xFF000000) >> 24,
         v2 = (v & 0x00FF0000) >> 16,
         v3 = (v & 0x0000FF00) >> 8,
         v4 = (v & 0x000000FF);
    snprintf(s, n, "%i.%i.%i.%i", (int)v1, (int)v2, (int)v3, (int)v4);
    return s;
}