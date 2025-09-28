#include <unistd.h>

# define BIT_SEQ 8
# define SET_MAP(bitmap, c) (bitmap[(unsigned char)(c) / 32] |= (1U << (unsigned char)(c) % 32))
# define CHECK_MAP(bitmap, c) (bitmap[(unsigned char)(c) / 32] & (1U << (unsigned char)(c) % 32))
# define NEWLINE() (write(1, "\n", 1))

typedef struct s_tables
{
    unsigned int seen[BIT_SEQ];
}t_tables;

void ft_union(char *str1, char *str2)
{
    t_tables map = {{0}};
    
    while(*str1)
    {
        if(!CHECK_MAP(map.seen, *str1))
        {
            write(1, str1, 1);
            SET_MAP(map.seen, *str1);
        }
        str1++;
    }
    
    while(*str2)
    {
        if(!CHECK_MAP(map.seen, *str2))
        {
            write(1, str2, 1);
            SET_MAP(map.seen, *str2);
        }
        str2++;
    }
}

int main(int argc, char **argv)
{
    if(argc != 3)
    {
        NEWLINE();
        return 0;
    }
    
    ft_union(argv[1], argv[2]);
    NEWLINE();
    return 0;
}
