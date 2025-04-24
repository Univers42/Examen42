#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#################################### UTILS FOR EXERCICES ######################################
# define WRITE_CHAR(c) (write(1,(c),1))
# define NEWLINE() (WRITE_CHAR("\n"))
# define IS_SPACE(x) ((x ^ ' ') == 0 || (x ^'\t') == 0 || (x ^'\n') == 0)
# define ISUPPER(c) (((c) & ~0x1F) == 0x40)
# define ISLOWER(c) (((c) & ~0x1F) == 0x60)
# define ISALPHA(c) (ISUPPER(c) || ISLOWER(c))
# define ISDIGIT(c) ((c) >= '0' && (c) <= '9')
# define IS_PRINTABLE(x) (x >= 32 && x <= 126)
# define IS_ALNUM(x) (ISALPHA(x) || ISDIGIT(x))

//################################## DEFINES FOR SPEFICIC EXERCICE ##############################
# define TOUPPER(c) ((c) & ~32)
# define TOLOWER(c) (((c)| 32))

void ft_str_capitalizer(char *str)
{
    while (*str)
    {
        if(ISALPHA(*str))
        {
            if(*(str + 1) == '\0' || IS_SPACE(*(str + 1)))
                *str = TOUPPER(*str);
            else
                *str = TOLOWER(*str);
        }
        WRITE_CHAR(str);
        str++;
    }
}

int main(int argc, char **argv)
{
    if (argc == 1)
        return(NEWLINE(), 1);
    argv++;
    while (*argv)
    {
        ft_str_capitalizer(*argv);
        NEWLINE();
        argv++;
    }
    return (0);
}
