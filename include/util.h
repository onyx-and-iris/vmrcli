/**
 * Copyright (c) 2024 Onyx and Iris
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `util.c` for details.
 */

#ifndef __UTIL_H__
#define __UTIL_H__

void remove_name_in_path(char *szPath);
int replace_multiple_space_with_one(char *s, size_t len);
char *kind_as_string(char *s, int kind, int n);
char *version_as_string(char *, long v, int n);

#endif /* __UTIL_H__ */