// memory/necessities.h
// This module contains necessities needed by the compiler, like memcpy. It must
// be included somewhere, preferably the entry point, for the compiler to
// function properly.

#pragma once
#include <stdint.h>
#include <stddef.h>

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);