#include <stdint.h>
#include <stdbool.h>

int string_length(const char s[]) {
    int i = 0;
    while (s[i] != '\0') ++i;
    return i;
}

int atoi(const char *str) {
	int result = 0;
	int sign = 1;

	while (*str == ' ' || *str == '\t') {
		str++;
	}

	if (*str == '-') {
		sign = -1;
		str++;
	} else if (*str == '+') {
		str++;
	}

	while (*str >= '0' && *str <= '9') {
		result = result * 10 + (*str - '0');
		str++;
	}

	return sign * result;
}

void reverse(char s[]) {
    int c, i, j;
    for (i = 0, j = string_length(s)-1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

void int_to_string(int n, char str[]) {
    int i, sign;
    if ((sign = n) < 0) n = -n;
    i = 0;
    do {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0) str[i++] = '-';
    str[i] = '\0';

    reverse(str);
}

void append(char s[], char n) {
    int len = string_length(s);
    s[len] = n;
    s[len+1] = '\0';
}

bool backspace(char s[]) {
    int len = string_length(s);
    if (len > 0) {
        s[len - 1] = '\0';
        return true;
    } else {
        return false;
    }
}

int starts_with(const char* str, const char* prefix) {
	while (*prefix) {
		if (*str++ != *prefix++) {
			return 0;
		}
	}
	return 1;
}


/* K&R
 * Returns <0 if s1<s2, 0 if s1==s2, >0 if s1>s2 */
int compare_string(char s1[], char s2[]) {
    int i;
    for (i = 0; s1[i] == s2[i]; i++) {
        if (s1[i] == '\0') return 0;
    }
    return s1[i] - s2[i];
}
