#include "int2str.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* int2str(int number)
{
	char* str = (char*)malloc(12 * sizeof(char));
	if (str == NULL)
	{
		return NULL;
	}

	int i = 0;
	int is_negative = 0;

	if (number == 0)
	{
		str[0] = '0';
		str[1] = '\0';
		return str;
	}

	if (number < 0)
	{
		is_negative = 1;
		if (number == INT_MIN)
		{
			strcpy(str, "-2147483648");
			return str;
		}
		number = -number;
	}

	while (number > 0)
	{
		str[i++] = (number % 10) + '0';
		number /= 10;
	}

	if (is_negative)
	{
		str[i++] = '-';
	}

	str[i] = '\0';

	int start = 0;
	int end = i - 1;
	while (start < end)
	{
		char temp = str[start];
		str[start] = str[end];
		str[end] = temp;
		start++;
		end--;
	}

	return str;
}