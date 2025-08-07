#pragma once

#include <stdint.h>
#include <stdbool.h>

#define low_16(address) (uint16_t)((address) & 0xFFFF)
#define high_16(address) (uint16_t)(((address) >> 16) & 0xFFFF)

int string_length(char s[]);

int atoi(const char *str);

void reverse(char s[]);

void int_to_string(int n, char str[]);

bool backspace(char s[]);

void append(char s[], char n);

int compare_string(char s1[], char s2[]);

int starts_with(const char* str, const char* prefix);
