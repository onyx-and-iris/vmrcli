#ifndef __UTIL_H__
#define __UTIL_H__

void remove_name_in_path(char *szPath);
int replace_multiple_space_with_one(char *s, size_t len);
char *kind_as_string(char *s, enum kind kind, int n);
char *version_as_string(char *, long v, int n);

#endif /* __UTIL_H__ */