/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcspn.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dyl-syzygy <dyl-syzygy@student.42.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 16:31:59 by dyl-syzygy        #+#    #+#             */
/*   Updated: 2025/03/30 16:38:54 by dyl-syzygy       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/**
 *       Each byte in bitmap has 8 bits
        // To find which byte: character / 8 = character >> 3
        // To find which bit within that byte: character % 8
 */
size_t ft_strcspn(const char *s, const char *reject)
{
    unsigned char bitmap[32] = {0};
    const char *start = s;
    
    while (*reject) 
    {
        bitmap[*reject >> 3] |= (1 << (*reject & 7));
        reject++;
    }
    while (*s) 
    {
        if (bitmap[*s >> 3] & (1 << (*s & 7)))
            break;
        s++;
    }
    return s - start;
}


int main(void)
{
    printf("Current Date and Time: 2025-03-30 14:36:57\n");
    printf("User: LESdylan\n\n");
    printf("Testing ft_strcspn:\n");
    printf("===================\n\n");

    // Test case structure: description, string, reject set, expected result
    typedef struct {
        char *desc;
        char *str;
        char *reject;
        size_t expected;
    } TestCase;

    TestCase tests[] = {
        {"Basic test", "Hello, world!", ",.!?", 5},
        {"No match", "Hello", "xyz", 5},
        {"Empty string", "", "abc", 0},
        {"Empty reject", "Hello", "", 5},
        {"First character match", "Hello", "H", 0},
        {"Last character match", "Hello", "o", 4},
        {"Special characters", "a!b@c#d$", "!@#$", 1},
        {"Alphanumeric search", "abc123", "0123456789", 3},
        {"Duplicates in reject", "Hello", "llll", 2},
        {"Long string", "This is a rather long string to test the function", " ", 4},
        {"ASCII range test", "abcdefghijklmnopqrstuvwxyz", "0123456789", 26},
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    for (int i = 0; i < num_tests; i++) {
        size_t result = ft_strcspn(tests[i].str, tests[i].reject);
        size_t std_result = strcspn(tests[i].str, tests[i].reject);
        
        printf("Test %d: %s\n", i + 1, tests[i].desc);
        printf("  String: \"%s\"\n", tests[i].str);
        printf("  Reject: \"%s\"\n", tests[i].reject);
        printf("  ft_strcspn result: %zu\n", result);
        printf("  Expected result:   %zu\n", std_result);
        
        if (result == std_result) {
            printf("  ✅ PASS\n");
            passed++;
        } else {
            printf("  ❌ FAIL\n");
        }
        printf("\n");
    }

    printf("Summary: %d/%d tests passed\n", passed, num_tests);
    
    return 0;
}

/*
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

// Macro for setting a bit in the bitmap for a character
#define BIT_SET(bitmap, c) (bitmap[(unsigned char)(c) / 32] |= 1U << ((unsigned char)(c) % 32))
// Macro for checking if a bit is set in the bitmap for a character
#define BIT_CHECK(bitmap, c) (bitmap[(unsigned char)(c) / 32] & (1U << ((unsigned char)(c) % 32)))

int ft_strcspn(const char *str1, const char *str2)
{
    int index = 0;
    uint32_t bitmap[8] = {0};
    while(*str2)
    {
        BIT_SET(bitmap, *str2);
        str2++;
    }
    while(*str1)
    {
        if(BIT_CHECK(bitmap, *str1))
            return index;
        index++;
        str1++;
    }
    return index;
}

int main(void)
{
    char string[46] = "This is the source string";
    char *substring = "axlie";
    printf("%c\n%d\n", string[ft_strcspn(string, substring)], ft_strcspn(string, substring));	
}
*/
