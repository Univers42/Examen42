 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 #include "exam.h"
 
 # define GREEN  "\x1b[32m"
 # define RED    "\x1b[31m"
 # define YELLOW "\x1b[33m"
 # define RESET  "\x1b[0m"
 
 typedef struct s_filter
 {
     char *name;
     char *(*fn)(char *);
 }   t_filter;
 
 typedef struct s_test
 {
     char *(*filter)(char *haystack, char *needle);
     char *pattern;
     char *input;
     char *expected;
 } t_test;
 
 
 int total_test = 0;
 int total_pass = 0;
 
 t_test test_list[] =
 {
     {filter, "hello", "hello world", "***** world"},
     {filter, "world", "hello world", "hello *****"},
     {filter, "test", "this is a test", "this is a ****"},
     {filter, "abc", "abcdefg", "***defg"},
     {filter, "xyz", "no match here", "no match here"},
     {filter, "longpattern", "short", "short"},
     {filter, "abc", "abcabcab", "******ab"},
     {filter, "aaaa", "aaa", "aaa"},
     {filter, "hello", "hello\nworld\nhello","*****\nworld\n*****"},
 
     {NULL, NULL, NULL, NULL}
 };
 
 void assert_equal(t_test *test)
 {
     total_test++;
     // Create a writable copy of the input string
     char *input_copy = malloc(strlen(test->input) + 1);
     strcpy(input_copy, test->input);
 
     char *result = test->filter(input_copy, test->pattern);
     if (strcmp(test->expected, result) == 0)
     {
         total_pass++;
         printf(GREEN "Test passed: " RESET "%s\n", test->input);
     }
     else
     {
         printf(RED "Test failed: " RESET "%s\n", test->input);
         printf(YELLOW "Expected: " RESET "%s\n", test->expected);
         printf(YELLOW "Got: " RESET "%s\n", result);
     }
 
     free(input_copy);
 }
 
 int main(int argc, char **argv)
 {
     int i = 0;
     while (test_list[i].filter != NULL)
     {
         assert_equal(&test_list[i]);
         i++;
     }
     printf("\nTotal tests: %d\n", total_test);
     printf("Total passed: %d\n", total_pass);
     printf("Total failed: %d\n", total_test - total_pass);
     return (0);
 }
