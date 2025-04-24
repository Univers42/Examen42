#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#################################### UTILS FOR EXERCICES ######################################
# define WRITE_CHAR(c, size) (write(1,&(c),size))
# define NEWLINE() (WRITE_CHAR("\n", 1))
# define IS_SPACE(x) ((x ^ ' ') == 0 || (x ^'\t') == 0 || (x ^'\n') == 0)
# define ISUPPER(c) (((c) & ~0x1F) == 0x40)
# define ISLOWER(c) (((c) & ~0x1F) == 0x60)
# define ISALPHA(c) (ISUPPER(c) || ISLOWER(c))
# define ISDIGIT(c) ((c) >= '0' && (c) <= '9')
# define IS_PRINTABLE(x) (x >= 32 && x <= 126)
# define IS_ALNUM(x) (IS_ALPHA(x) || IS_DIGIT(x))

//################################## DEFINES FOR SPEFICIC EXERCICE ##############################

int ft_strlen(char *str)
{
    char *temp = str;
    while(*str++);
    return (str - temp - 1);
}
void ft_epur_str(char *str)
{
    char *start;
    char *end;
    int size;
    int printed = 1;
    size = ft_strlen(str);
    end = str + size -1;
    start = str;
    while(IS_SPACE(*start))
        start++;
    while(IS_SPACE(*end))
        end--;
    while(start <= end)
    {
        if(IS_SPACE(*start) && !printed)
        {
            WRITE_CHAR("   ", 3);
            printed = 1;
        }
        else if (!IS_SPACE(*start))
        {
            WRITE_CHAR(*start, 1);
            printed = 0;
        }
        start++;
    }
    NEWLINE();
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return(NEWLINE(), 1);
    char *str = argv[1];
    ft_epur_str(str);
    return (0);
}