/**
 * Copyright (c) 2024 Onyx and Iris
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `util.c` for details.
 */

#ifndef __UTIL_H__
#define __UTIL_H__

struct quickcommand
{
    char *name;
    char *fullcommand;
};

void remove_last_part_of_path(char *fullpath);
char *kind_as_string(char *s, int kind, int n);
char *version_as_string(char *s, long v, int n);
bool is_comment(char *s);
struct quickcommand *command_in_quickcommands(const char *command, const struct quickcommand *quickcommands, int n);

#endif /* __UTIL_H__ */