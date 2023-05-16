#pragma once

#include <stddef.h>
#include <stdlib.h>

int ft_strlen(const char *s);
int ft_strcmp(const char *s1, const char *s2);
int ft_strncmp(const char *s1, const char *s2, size_t n);
void ft_strcpy(char *dst, const char *src);
void *ft_calloc(size_t count, size_t size);
void ft_bzero(void *s, size_t n);