#include <unistd.h>
#define NEWLINE() (write(1,"\n",1))

void hidenp(char *str, char *set)
{
    while(*str && *set)
        str+=!(*str ^ *set++);
    return (write(1,(!*str ? "1\n" : "0\n"),2));
}

int main(int argc, char **argv)
{
    char *str;
    char *set;
    if(argc != 3)
        return (NEWLINE(), 1);
    str = argv[1];
    set = argv[2];
    hidenp(str, set);
    return (0);
}
