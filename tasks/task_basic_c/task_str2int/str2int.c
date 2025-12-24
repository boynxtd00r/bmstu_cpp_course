#include "str2int.h"
#include <stdio.h>
#include <limits.h>
#include <ctype.h>
#include <stdlib.h>

int str2int(const char* str)
{
    if (str == NULL || *str == '\0') {
        fprintf(stderr, "Empty string\n");
        abort();
    }
    
    int i = 0;
    int sign = 1;
    long long result = 0;
    
    while (isspace(str[i])) {
        i++;
    }
    
    if (str[i] == '-') {
        sign = -1;
        i++;
        if (str[i] == '\0') {
            fprintf(stderr, "Invalid format\n");
            abort();
        }
    } else if (str[i] == '+') {
        i++;
        if (str[i] == '\0') {
            fprintf(stderr, "Invalid format\n");
            abort();
        }
    }
    
    while (str[i] != '\0') {
        if (str[i] >= '0' && str[i] <= '9') {
            result = result * 10 + (str[i] - '0');
            
            if (sign == 1 && result > INT_MAX) {
                fprintf(stderr, "Overflow\n");
                abort();
            }
            if (sign == -1 && (-result) < INT_MIN) {
                fprintf(stderr, "Underflow\n");
                abort();
            }
        } else {
            fprintf(stderr, "Invalid character\n");
            abort();
        }
        i++;
    }
    
    result *= sign;
    
    if (result > INT_MAX || result < INT_MIN) {
        fprintf(stderr, "Out of range\n");
        abort();
    }
    
    return (int)result;
}