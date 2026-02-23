#!/bin/bash

gcc -Wall -Wextra -Werror vbc.c -o vbc
[ $? -ne 0 ] && echo "Compilation failed!" && exit 1

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'
PASS=0
FAIL=0

check() {
    local input="$1"
    local expected="$2"
    local expected_exit=0
    
    # If the expected output contains "Unexpected", the expected exit code is 1
    if [[ "$expected" == *"Unexpected"* ]]; then
        expected_exit=1
    fi

    local output
    output=$(./vbc "$input" 2>&1)
    local exit_code=$?

    if [ "$output" = "$expected" ] && [ $exit_code -eq $expected_exit ]; then
        echo -e "${GREEN}PASS${NC} [$input] => $output"
        ((PASS++))
    else
        echo -e "${RED}FAIL${NC} [$input] => got: '$output' (exit: $exit_code) | expected: '$expected' (exit: $expected_exit)"
        ((FAIL++))
    fi
}

# Valid expressions
check '1'                   '1'
check '0'                   '0'
check '9'                   '9'
check '0*9'                 '0'
check '9+0'                 '9'
check '2+3'                 '5'
check '3*4+5'               '17'
check '3+4*5'               '23'
check '(3+4)*5'             '35'
check '1+2+3+4+5'           '15'
check '1+2+3+4+5+6+7+8+9+0' '45'
check '1*2*3*4*5*6*7*8*9*0' '0'
check '9*9*0*9*9'           '0'
check '(9+9)*0'             '0'
check '9*9*9*9*9*9*9*9*9*9' '3486784401'
check '0+0+0+0+0+0+0+0+0+0' '0'
check '(1)'                 '1'
check '((1+2))'             '3'
check '1+2+(3)'             '6'
check '(1+2)+(3+4)'         '10'
check '(1)*(2)*(3)'         '6'
check '(1+2)*3'             '9'
check '(((((((3)))))))'     '3'
check '(((((((((1+1)))))))))' '2'
check '(((((3+2)*2)+1)*2)+1)' '23'
check '((((1+2))*3)*4)*5'   '180'
check '(1+(2+(3+(4+(5)))))'  '15'
check '1*(2+3*(4+5*(6+7)))' '209'
check '((2+3)*((4+5)*(6+7)))' '585'
check '((6*6+7+5+8)*(1+0+4*8+7)+2)+4*(1+2)' '2254'
check '(((((2+2)*2+2)*2+2)*2+2)*2+2)*2'      '188'
check '2*4+9+3+2*1+5+1+6+6*1*1+8*0+0+5+0*4*9*5*8+9*7+5*1+3+1+4*5*7*3+0*3+4*8+8+8+4*0*5*3+5+4+5*7+9+6*6+7+9*2*6*9+2+1*3*7*1*1*5+1+2+7+4+3*4*2+0+4*4*2*2+6+7*5+9+0+8*4+6*7+5+4*4+2+5*5+1+6+3*5*9*9+7*4*3+7+4*9+3+0+1*8+1+2*9*4*5*1+0*1*9+5*3*5+9*6+5*4+5+5*8*6*4*9*2+0+0+1*5*3+6*8*0+0+2*3+7*5*6+8+6*6+9+3+7+0*0+5+2*8+2*7*2+3+9*1*4*8*7*9+2*0+1*6*4*2+8*8*3*1+8+2*4+8*3+8*3+9*5+2*3+9*5*6*4+3*6*6+7+4*8+0+2+9*8*0*6*8*1*2*7+0*5+6*5+0*2+7+2+3+8*7+6+1*3+5+4*5*4*6*1+4*7+9*0+4+9*8+7+5+6+2+6+1+1+1*6*0*9+7+6*2+4*4+1*6*2*9+3+0+0*1*8+4+6*2+6+2*7+7+0*9+6+2*1+6*5*2*3*5*2*6*4+2*9*2*4*5*2*2*3+8+8*3*2*3+0*5+9*6+8+3*1+6*9+8+9*2*0+2' '94305'

# Errors: unexpected token
check '1+2)'                "Unexpected token ')'"
check '1+2)('               "Unexpected token ')'"
check '((1+2)*3))'          "Unexpected token ')'"
check 'abc'                 "Unexpected token 'a'"
check '1++2'                "Unexpected token '+'"
check '1**2'                "Unexpected token '*'"
check '(*1+2)'              "Unexpected token '*'"
check '1+2+(+3)'            "Unexpected token '+'"
check '1+2+(-3)'            "Unexpected token '-'"
check '1+2()'               "Unexpected token '('"
check '1+2(+3)'             "Unexpected token '('"
check '+1'                  "Unexpected token '+'"
check '*1'                  "Unexpected token '*'"
check '()'                  "Unexpected token ')'"
check '1+()'                "Unexpected token ')'"
check '(()' 	            "Unexpected token ')'"
check '((1+3)*12+(3*(2+6))' "Unexpected token '2'"

# Errors: unexpected end of input
check ''                    'Unexpected end of input'
check '1+'                  'Unexpected end of input'
check '1+2*'                'Unexpected end of input'
check '1+2+'                'Unexpected end of input'
check '((1+2)*3'            'Unexpected end of input'
check '(1+2'                'Unexpected end of input'
check '1+(2+3'              'Unexpected end of input'
check '(((((2+2)*2+2)*2+2)*2+2)*2' 'Unexpected end of input'

echo ""
echo -e "Results: ${GREEN}$PASS passed${NC}, ${RED}$FAIL failed${NC} / $((PASS + FAIL)) total"