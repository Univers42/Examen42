#!/bin/bash

# Colors
RED="$(tput setaf 1)$(tput bold)"
GREEN="$(tput setaf 2)$(tput bold)"
YELLOW="$(tput setaf 3)$(tput bold)"
RESET="$(tput sgr0)"

USR="argo.c"

echo "${YELLOW}Compiling your version...${RESET}"
if ! gcc -Wall -Wextra -Werror -o usr "$USR"; then
    echo "${RED}Your compilation failed!${RESET}"
    exit 1
fi

# Array of tests: "Input" followed by "Expected Output"
tests=(
    # --- 1. VALID CASES (Output must match Input exactly) ---
    '1' '1'
    '-42' '-42'
    '0' '0'
    '"bonjour"' '"bonjour"'
    '""' '""'
    '"escape! \" "' '"escape! \" "'
    '"slash \\ "' '"slash \\ "'
    '{"tomatoes":42,"potatoes":234}' '{"tomatoes":42,"potatoes":234}'
    '{}' '{}'
    '{"a":{}}' '{"a":{}}'
    '{"recursion":{"recursion":{"recursion":{"recursion":"recursion"}}}}' '{"recursion":{"recursion":{"recursion":{"recursion":"recursion"}}}}'
    '{"a":1,"b":"two","c":{"d":4}}' '{"a":1,"b":"two","c":{"d":4}}'
    '{"spaces inside string":"  yes  "}' '{"spaces inside string":"  yes  "}'

    # --- 2. INVALID CASES (Output must match Error Message) ---
    '"unfinished string' "Unexpected end of input"
    '"unfinished string 2\"' "Unexpected end of input"
    '{"no value?":}' "Unexpected token '}'"
    '{"a": 1}' "Unexpected token ' '"
    '{"a":1,}' "Unexpected token '}'"
    '{1:1}' "Unexpected token '1'"
    '{"a":1"b":2}' "Unexpected token '\"'"
    '' "Unexpected end of input"
    'abc' "Unexpected token 'a'"
    '{"missing_colon"1}' "Unexpected token '1'"
    '{' "Unexpected end of input"
    '{"key":"val"' "Unexpected end of input"
)

echo
echo "${YELLOW}Running Enforced Tests...${RESET}"
pass_count=0
total_tests=$((${#tests[@]} / 2))

# Iterate through the array by steps of 2
for (( i=0; i<${#tests[@]}; i+=2 )); do
    input="${tests[$i]}"
    expected="${tests[$i+1]}"
    
    # Run the program and capture output (redirect stderr to stdout just in case)
    out_usr=$(echo -n "$input" | ./usr /dev/stdin 2>&1)
    
    if [ "$out_usr" = "$expected" ]; then
        echo -e "${GREEN}PASS${RESET}  Input: [$input]"
        pass_count=$((pass_count + 1))
    else
        echo -e "${RED}FAIL${RESET}  Input: [$input]"
        echo -e "  ${GREEN}Expected:${RESET} \"$expected\""
        echo -e "  ${RED}Yours:   ${RESET} \"$out_usr\""
        echo "-------------------------------------------"
    fi
done

# Cleanup
rm -f usr

echo
if [ $pass_count -eq $total_tests ]; then
    echo -e "${GREEN}ALL $total_tests TESTS PASSED 🎉${RESET}"
else
    echo -e "${RED}$(( total_tests - pass_count )) / $total_tests TEST(S) FAILED ❌${RESET}"
fi