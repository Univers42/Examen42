#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

static void compute_removal(const char *buffer, int n, int *lr, int *rr)
{
    int bal;
    int i;

    i = -1;
    bal = 0;
    *lr = 0;
    *rr = 0;
    while (++i < n)
    {
        if (buffer[i] == '(')
            bal++;
        else if (buffer[i] == ')')
        {
            if (bal > 0)        //The error was here
                bal--;
            else
                (*rr)++;
        }
    }
    *lr = bal;
}

void    build_dfs(char *buffer, char *cur, int n, int k, int bal, int lr, int rr);

static void rec_open(char *buffer, char *cur, int n, int k, int bal, int lr, int rr)
{
    cur[k] = '(';
    build_dfs(buffer, cur, n, k + 1, bal + 1, lr, rr);
    if (lr > 0)
    {
        cur[k] = ' ';
        build_dfs(buffer, cur, n, k + 1, bal, lr - 1, rr);
    }
}

static void rec_close(char *buffer, char *cur, int n, int k, int bal, int lr, int rr)
{
    if (bal > 0)
    {
        cur[k] = ')';
        build_dfs(buffer, cur, n, k + 1, bal - 1, lr, rr);
    }
    if(rr > 0)
    {
        cur[k] = ' ';
        build_dfs(buffer, cur, n, k + 1, bal, lr, rr - 1);
    }
}

void    build_dfs(char *buffer, char *cur, int n, int k, int bal, int lr, int rr)
{
    char    c;

    if (k == n)
    {
        if (bal == 0 && lr == 0 && rr == 0)
        {
            write(1, cur, n);
            write(1, "\n", 1);
        }
    }
    else
    {
        c = buffer[k];
        if (c == '(')
            rec_open(buffer, cur, n, k, bal, lr, rr);
        else if(c == ')')
            rec_close(buffer, cur, n, k, bal, lr, rr);
        else
        {
            cur[k] = c;
            build_dfs(buffer, cur, n, k + 1, bal, lr, rr);
        }
    }
}

int main(int argc, char **argv)
{
    char    *buffer;
    int     lr;
    int     rr;
    int     bal;
    int     k;
    int     n;
    int     i;

    if (argc != 2)
        return (1);
    buffer = argv[1];		//and here
    n = strlen(buffer);
    char    cur[n];
    i = -1;
    while (++i < n)
        cur[i] = buffer[i];
    compute_removal(buffer, n, &lr, &rr);
    bal = 0;
    k = 0;
    build_dfs(buffer, cur, n, k, bal, lr, rr);
    return (0);
}