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

void* ft_calloc(size_t count, size_t size) {
    void *res = malloc(count * size);
    if (!res)
        return (NULL);
    for (size_t i = 0; i < count * size; i++)
        ((char*)res)[i] = 0;
    return (res);
}

