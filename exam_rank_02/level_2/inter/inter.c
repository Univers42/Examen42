#include <unistd.h>
#include <stdint.h>
# define WRITE_CHAR(c) (write(1,&(c),1))
# define WRITE_NEW_LINE(c) (write(1,"\n",1))
# define BIT_SET(bitmap,c) (bitmap[(unsigned char)(c)/32] |= 1U << (unsigned char)(c) % 32)
# define BIT_CHECK(bitmap, c) (bitmap[(unsigned char)(c) / 32] & (1U << ((unsigned char)(c) % 32)))
# define BIT_BUFFER 8

typedef struct s_tables
{
    uint32_t seen[BIT_BUFFER];
    uint32_t printed[BIT_BUFFER];
}t_tables;

void inter(const char *str1, const char *str2)
{
    t_tables table = {{0}, {0}};
    while(*str2)
    {
        BIT_SET(table.seen, *str2);
        str2++;
    }
    while(*str1)
    {
        if(BIT_CHECK(table.seen, *str1) && ! BIT_CHECK(table.printed, *str1))
        {
            WRITE_CHAR(*str1);
            BIT_SET(table.printed, *str1);
        }
        str1++;
    }
}

int main(int argc, char **argv)
{
    if (argc == 3)
        inter(argv[1], argv[2]);
    WRITE_NEW_LINE();
    return (0);
}
