#ifdef __cplusplus
    extern "C" {
#endif
#ifndef _UTILS_H_
#define _UTILS_H_

void str_remove_junk_char(char *);

char *str_replace(char *search , char *replace , char *subject);

char** str_split(char* a_str, const char a_delim);

char *url_decode(const char *str);

#endif
#ifdef __cplusplus
    }
#endif
