#include "str2int.h"
#include <assert.h>
#include <stdio.h>
#include <limits.h>
#include <ctype.h>

int str2int(const char* str)
{
    if (str == NULL || *str == '\0') {
        assert(0 && "Empty string");
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
            assert(0 && "Invalid format");
        }
    } else if (str[i] == '+') {
        i++;
        if (str[i] == '\0') {
            assert(0 && "Invalid format");
        }
    }
    
    while (str[i] != '\0') {
        if (str[i] >= '0' && str[i] <= '9') {
            result = result * 10 + (str[i] - '0');
            
            if (sign == 1 && result > INT_MAX) {
                assert(0 && "Overflow");
            }
            if (sign == -1 && (-result) < INT_MIN) {
                assert(0 && "Underflow");
            }
        } else {
            assert(0 && "Invalid character");
        }
        i++;
    }
    
    result *= sign;
    
    if (result > INT_MAX || result < INT_MIN) {
        assert(0 && "Out of range");
    }
    
    return (int)result;
}