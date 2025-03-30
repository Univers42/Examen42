#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

// Function prototype
char *ft_strpbrk(const char *s1, const char *s2);

// For visualization
void print_bitmap(unsigned char *bitmap, int size) {
    printf("Bitmap representation (only showing relevant parts):\n");
    for (int byte = 0; byte < 16; byte++) {
        if (bitmap[byte] == 0) continue; // Skip empty bytes for clarity
        
        printf("Byte %2d [ASCII %3d-%3d]: ", byte, byte*8, byte*8+7);
        for (int bit = 7; bit >= 0; bit--) {
            printf("%d", (bitmap[byte] & (1 << bit)) ? 1 : 0);
            if (bit == 4) printf(" "); // For readability
        }
        
        printf(" - Characters: ");
        for (int bit = 0; bit < 8; bit++) {
            if (bitmap[byte] & (1 << bit)) {
                char c = byte*8 + bit;
                if (c >= 32 && c <= 126) // Printable ASCII
                    printf("'%c' ", c);
                else
                    printf("(%d) ", c);
            }
        }
        printf("\n");
    }
    printf("\n");
}

// For performance testing
void performance_test(int iterations) {
    char *long_str = malloc(10000);
    char *chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    
    // Create a large string
    for (int i = 0; i < 9999; i++) {
        long_str[i] = chars[rand() % 62];
    }
    long_str[9999] = '\0';
    
    // Create a string of characters to search for
    char *search = "XYZ";
    
    // Perform timing test
    clock_t start, end;
    
    // Measure standard strpbrk
    start = clock();
    for (int i = 0; i < iterations; i++) {
        strpbrk(long_str, search);
    }
    end = clock();
    double std_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    // Measure our ft_strpbrk
    start = clock();
    for (int i = 0; i < iterations; i++) {
        ft_strpbrk(long_str, search);
    }
    end = clock();
    double our_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("Performance Test (%d iterations on 10,000 character string):\n", iterations);
    printf("Standard strpbrk: %.6f seconds\n", std_time);
    printf("Our ft_strpbrk:   %.6f seconds\n", our_time);
    printf("Ratio: %.2fx %s\n\n", 
           (std_time > our_time) ? std_time/our_time : our_time/std_time,
           (std_time > our_time) ? "faster" : "slower");
           
    free(long_str);
}

int main(void) {
    printf("Current Date and Time: 2025-03-30 14:50:19\n");
    printf("User: LESdylan\n\n");
    printf("BITMAP-BASED FT_STRPBRK DEMONSTRATION\n");
    printf("=====================================\n\n");
    
    // Example 1: Basic usage
    const char *str1 = "Hello, world!";
    const char *str2 = "aeiou";
    char *result1 = ft_strpbrk(str1, str2);
    char *std_result1 = strpbrk(str1, str2);
    
    printf("Example 1:\n");
    printf("String: \"%s\"\n", str1);
    printf("Chars to find: \"%s\"\n", str2);
    
    // Visualize the bitmap (in a real program, we'd need to extract it from the function)
    unsigned char bitmap[32] = {0};
    const char *s2_temp = str2;
    while (*s2_temp) {
        bitmap[*s2_temp >> 3] |= (1 << (*s2_temp & 7));
        s2_temp++;
    }
    print_bitmap(bitmap, 32);
    
    printf("Result: %s\n", result1 ? result1 : "NULL");
    printf("Standard strpbrk: %s\n", std_result1 ? std_result1 : "NULL");
    printf("Match: %s\n\n", (result1 == std_result1) ? "✓" : "✗");
    
    // Example 2: Special characters
    const char *str3 = "This is a test with special chars: !@#$%^&*()";
    const char *str4 = "!@#$";
    char *result2 = ft_strpbrk(str3, str4);
    char *std_result2 = strpbrk(str3, str4);
    
    printf("Example 2:\n");
    printf("String: \"%s\"\n", str3);
    printf("Chars to find: \"%s\"\n", str4);
    
    // Reset and visualize the new bitmap
    for (int i = 0; i < 32; i++) bitmap[i] = 0;
    s2_temp = str4;
    while (*s2_temp) {
        bitmap[*s2_temp >> 3] |= (1 << (*s2_temp & 7));
        s2_temp++;
    }
    print_bitmap(bitmap, 32);
    
    printf("Result: %s\n", result2 ? result2 : "NULL");
    printf("Standard strpbrk: %s\n", std_result2 ? std_result2 : "NULL");
    printf("Match: %s\n\n", (result2 == std_result2) ? "✓" : "✗");
    
    // Example 3: No match
    const char *str5 = "Hello";
    const char *str6 = "xyz";
    char *result3 = ft_strpbrk(str5, str6);
    char *std_result3 = strpbrk(str5, str6);
    
    printf("Example 3:\n");
    printf("String: \"%s\"\n", str5);
    printf("Chars to find: \"%s\"\n", str6);
    printf("Result: %s\n", result3 ? result3 : "NULL");
    printf("Standard strpbrk: %s\n", std_result3 ? std_result3 : "NULL");
    printf("Match: %s\n\n", (result3 == std_result3) ? "✓" : "✗");
    
    // Performance testing
    performance_test(100000);
    
    printf("Bitmap Algorithm Advantages:\n");
    printf("1. O(1) lookups - regardless of how many characters we're searching for\n");
    printf("2. Only processes each character of s1 and s2 once\n");
    printf("3. Fixed memory footprint (32 bytes) regardless of input size\n");
    printf("4. Excellent cache locality for the bitmap\n");
    printf("5. Uses fast bitwise operations for character detection\n");
    
    return 0;
}