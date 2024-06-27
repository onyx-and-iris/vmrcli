#include <string.h>

void replace_multiple_space_with_one(char *s)
{
    int j = 0;
    int count = 0;
    int len = strlen(s);

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

    return;
}