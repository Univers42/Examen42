// two pass reverse  technique
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
int ft_strlen(char *s)
{
    int i = 0;
    while (s[i])
        i++;
    return i;
}

void ft_putstr(char *s)
{
    while (*s)
        write(1, s++, 1);
}

void reverse(char *s, int start, int end)
{
    while (start < end)
    {
        s[start] ^= s[end];
        s[end] ^= s[start];
        s[start] ^= s[end];
        start++;
        end--;
    }
}

void rev_wstr(char *s)
{
    int len = ft_strlen(s);
    char *copy = malloc(len + 1);
    int i = 0, start = 0, end = 0;
    if (!copy)
        return;
    while (s[i])
    {
        copy[i] = s[i];
        i++;
    }
    copy[i] = '\0';
    reverse(copy, 0, len - 1);
    i = 0;
    while (copy[i])
    {
        if (copy[i] == ' ')
        {
            reverse(copy, start, i - 1);
            start = i + 1;
        }
        else
            end = i + 1;
        i++;
    }
    (reverse(copy, start, end - 1), ft_putstr(copy), free(copy));
}

int main(int argc, char **argv)
{
    if (argc == 2)
        rev_wstr(argv[1]);
    write(1, "\n", 1);
    return 0;
}