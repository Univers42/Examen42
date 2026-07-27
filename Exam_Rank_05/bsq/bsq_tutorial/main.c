#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
we have to loop through input files. 
*/
int main(int argc, char **argv)
{
    if (argc > 1)
    {
        for (int i = 1; i < argc; ++i)
        {
            FILE *file = fopen(argv[i], "r");
            if (file == NULL)
                continue;
            bsq(file);
            fclose(file);
        }
    }
    else
    {
        // call the key function with stdin as the input strea
        bsq(stdin);
    }
        return (0);

}