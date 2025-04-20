int ft_atoi_base(const char *str, int str_base)
{
    int result = 0, is_negative = 0, digit;

    // Skip spaces
    while (*str == ' ' || (*str >= 9 && *str <= 13))
        str++;

    // Handle sign
    if (*str == '-') {
        is_negative = 1;
        str++;
    }

    // Parse digits
    while (*str)
    {
        if (*str >= '0' && *str <= '9')
            digit = *str - '0';
        else if (*str >= 'a' && *str <= 'f')
            digit = *str - 'a' + 10;
        else if (*str >= 'A' && *str <= 'F')
            digit = *str - 'A' + 10;
        else
            break;

        if (digit >= str_base)
            break;

        result = result * str_base + digit;
        str++;
    }
    return is_negative ? -result : result;
}
// Test main function
int main(void)
{
    // Helper function to print test results
    
    printf("\n=== TESTING ft_atoi_base ===\n\n");
    
    // Test binary base (base 2)
    printf("--- Binary (Base 2) ---\n");
    test_base("1010", 2);        // 10 in decimal
    test_base("11111111", 2);    // 255 in decimal
    test_base("-10101", 2);      // -21 in decimal
    
    // Test octal base (base 8)
    printf("\n--- Octal (Base 8) ---\n");
    test_base("123", 8);         // 83 in decimal
    test_base("777", 8);         // 511 in decimal
    test_base("-17", 8);         // -15 in decimal
    
    // Test decimal base (base 10)
    printf("\n--- Decimal (Base 10) ---\n");
    test_base("123", 10);        // 123 in decimal
    test_base("9876", 10);       // 9876 in decimal
    test_base("-42", 10);        // -42 in decimal
    
    // Test hexadecimal base (base 16)
    printf("\n--- Hexadecimal (Base 16) ---\n");
    test_base("1A", 16);         // 26 in decimal
    test_base("FF", 16);         // 255 in decimal
    test_base("abc", 16);        // 2748 in decimal
    test_base("FfFf", 16);       // 65535 in decimal (mixed case)
    test_base("-deadbeef", 16);  // -3735928559 in decimal
    
    // Test other bases
    printf("\n--- Other Bases ---\n");
    test_base("101", 3);         // Base 3: 10 in decimal
    test_base("12345", 6);       // Base 6
    test_base("9876", 11);       // Base 11
    test_base("DEFABC", 15);     // Base 15
    
    // Test edge cases
    printf("\n--- Edge Cases ---\n");
    test_base("0", 10);          // Zero
    test_base("", 10);           // Empty string
    test_base("123", 1);         // Invalid base (< 2)
    test_base("123", 17);        // Invalid base (> 16)
    test_base("12G", 16);        // Invalid character (G in hex)
    test_base("12345", 4);       // Invalid digit for base (5 in base 4)
    
    return 0;
}
