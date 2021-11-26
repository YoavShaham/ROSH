#include "string.h"

void memset(void *str, int c, size_t n)
{
	int i = 0;

	for (i = 0; i < n; i++)
	{
		*str = c;
	}
}

int strlen(const char* str)
{
	int i = 0;
	while (*str != 0)	// while not end
	{
		i++;	// continue
		str++;
	}
	return i;		// return length on end
}

void itoa(int num, char* res)
{
	int size = digits(num);
	for (int i = 0; i < size; i++)		// for size times
	{
		res[size - i - 1] = num % 10 + ASCII_NUM_OFFSET;		// add last digit
		num /= 10;		// remove last digit
	}
}

int digits(int num)
{
	int i = 0;
	while (num > 0)		// count num's digits
	{
		num /= 10;
		i++;
	}
	return i == 0 ? 1 : i;
}

int strncmp(const char* str1, const char* str2, int n)
{
	for (int i = 0; i < n; i++)		// n times (or one of strings over)
	{
		if (str1[i] == 0 || str2 == 0)
			return str1[i] == str2[i] ? 0 : str1[i] - str2[i];
		if (str1[i] != str2[i])
			return str1[i] - str2[i];
	}
	return 0;
}

int atoi(const char* str)
{
	int size = strlen(str);
	int num = 0;
	for (int i = 0; i < size; i++)		// for every const char*
	{
		if (str[i] >= ASCII_NUM_OFFSET && str[i] < ASCII_NUM_OFFSET + 10)		// if in range (0-9)
		{
			num *= 10;			// add to int
			num += str[i] - ASCII_NUM_OFFSET;
		}
	}
	return num;
}