char *ft_strcpy_alt(char *s1, char *s2)
{
    char *dest = s1;
    
    while ((*s1++ = *s2++))
        ;
        
    return (dest);
}
