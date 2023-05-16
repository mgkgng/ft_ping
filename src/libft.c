#include "libft.h"

int ft_strlen(const char *s) {
    int i = 0;
    while (s[i])
        i++;
    return (i);
}

int ft_strcmp(const char *s1, const char *s2) {
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return (*s1 - *s2);
}

int ft_strncmp(const char *s1, const char *s2, size_t n) {
    while (n-- && *s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return (*s1 - *s2);
}

void ft_strcpy(char *dst, const char *src) {
    while (*src)
        *dst++ = *src++;
    *dst = 0;
}

void *ft_calloc(size_t count, size_t size) {
    void *res = malloc(count * size);
    if (!res)
        return (NULL);
    for (size_t i = 0; i < count * size; i++)
        ((char*)res)[i] = 0;
    return (res);
}

void *ft_memset(void *b, int c, size_t len)
{
	char	*p;

	p = b;
	while (len-- > 0)
		*p++ = c;
	return (b);
}

void ft_bzero(void *s, size_t n) {
	ft_memset(s, 0, n);
}