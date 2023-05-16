#include "ft_ping.h"

int ft_strlen(const char *s) {
    int i = 0;
    while (s[i])
        i++;
    return (i);
}

static int ft_isdigit(int c) {
    return (c >= '0' && c <= '9');
}

int ft_atoi(const char *str, int strict) {
	int	res;
	int	i;
	int	neg;

    if (strict) {
        for (i = 0; str[i]; i++) {
            if (!ft_isdigit(str[i]))
                return (-1);
        }
    }
	res = 0;
	i = 0;
	while ((str[i] >= 9 && str[i] <= 13) || str[i] == ' ')
		i++;
	neg = 1;
	if (str[i] == '-' && ++i)
		neg *= -1;
	else if (str[i] == '+')
		i++;
	while (str[i] && ft_isdigit(str[i]))
		res = res * 10 + (str[i++] - '0');
	return (res * neg);
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

int ft_isfloat(char *str) {
    char* ptr;
    errno = 0;
    strtof(str, &ptr);
    
    if ((ptr == str) || (*ptr != '\0') || (errno != 0))
        return false;
    return true;
}
