#ifndef __UTIL_H__
#define __UTIL_H__

void remove_name_in_path(char *szPath);
void replace_multiple_space_with_one(char *s, size_t len);
char *version_as_string(char *, long v, int n);

#endif /* __UTIL_H__ */