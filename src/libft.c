#include "ft_ping.h"

int ft_strlen(const char *s) {
    int i = 0;
    while (s[i])
        i++;
    return (i);
}

void ft_strcpy(char *dst, const char *src) {
    while (*src)
        *dst++ = *src++;
    *dst = 0;
}

static void *ft_memset(void *b, int c, size_t len) {
	char *p = b;

	while (len-- > 0)
		*p++ = c;
	return (b);
}

void ft_bzero(void *s, size_t n) {
	ft_memset(s, 0, n);
}