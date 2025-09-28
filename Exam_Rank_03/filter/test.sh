#!/usr/bin/env bash
set -Eeuo pipefail

# Move to script directory
cd -- "$(dirname -- "${BASH_SOURCE[0]}")"

# Build ./filter if needed
if [[ ! -x ./filter ]]; then
  echo "[build] compiling ./filter from solution.c"
  cc -Wall -Wextra -O2 solution.c -o filter
fi

# Escape a pattern for literal match in sed using '/' as delimiter (BRE-safe)
escape_for_sed_literal() {
  local s=$1
  # Order matters: backslash first, then delimiter, then BRE metas
  s=${s//\\/\\\\}
  s=${s//\//\\/}
  s=${s//./\\.}
  s=${s//\*/\\*}
  s=${s//\[/\\[}   # Escape [ for literal match
  s=${s//\]/\\]}   # Escape ] for literal match
  s=${s//\(/\\(}
  s=${s//\)/\\)}
  s=${s//\{/\\{}
  s=${s//\}/\\}}
  s=${s//^/\\^}
  s=${s//$/\\$}
  echo "$s"
}

# Make a string of N asterisks
stars() {
  local -i n=$1
  # printf width -> spaces, then tr to '*'
  printf "%*s" "$n" "" | tr ' ' '*'
}

# Random string from a given charset
rand_str() {
  local -i len=$1
  local chars=$2
  local -i clen=${#chars}
  local out=""
  for ((i=0; i<len; i++)); do
    out+="${chars: RANDOM % clen:1}"
  done
  printf "%s" "$out"
}

# Charsets (include many regex/metacharacters)
CS_ALNUM='abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'
CS_ALNUM_SP=$'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 '
CS_PUNCT=$'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 .,*[](){}?+^$|\\/=_-!~:;\'"<>'
# Keep NEWLINE out of datasets intentionally.

total=0
pass=0

tmpdir="$(mktemp -d)"
cleanup() { rm -rf "$tmpdir"; }
trap cleanup EXIT

run_case() {
  local input="$1"
  local pattern="$2"
  local -i idx="$3"

  # Skip empty pattern (invalid per subject)
  if [[ -z "$pattern" ]]; then
    return 0
  fi

  local escaped
  escaped="$(escape_for_sed_literal "$pattern")"
  local repl
  repl="$(stars "${#pattern}")"

  # Use files to preserve exact bytes
  printf "%s" "$input" | sed -e "s/$escaped/$repl/g" > "$tmpdir/sed_$idx.out"
  printf "%s" "$input" | ./filter "$pattern" > "$tmpdir/filter_$idx.out"

  if ! cmp -s "$tmpdir/sed_$idx.out" "$tmpdir/filter_$idx.out"; then
    echo "Mismatch on case #$idx"
    echo "Pattern (len=${#pattern}): [$pattern]"
    echo "Input   (len=${#input})"
    echo "---- sed output (first 256 bytes) ----"
    head -c 256 "$tmpdir/sed_$idx.out" | hexdump -C
    echo
    echo "---- filter output (first 256 bytes) ----"
    head -c 256 "$tmpdir/filter_$idx.out" | hexdump -C
    echo
    echo "Full outputs kept in: $tmpdir/sed_$idx.out and $tmpdir/filter_$idx.out"
    return 1
  fi
  return 0
}

# Some fixed sanity cases
fixed_cases() {
  local i=1
  run_case 'abcdefgaaaabcdefabc' 'abc' $((i++)) || return 1
  run_case 'aaaaaa' 'aa' $((i++)) || return 1         # overlapping pattern
  run_case 'hello world' ' ' $((i++)) || return 1     # space pattern
  run_case 'abc.def[ghi](jkl){mno}+pqr^stu$|vwx\\yz' '.' $((i++)) || return 1
  run_case 'slash/pipe|backslash\\' '|' $((i++)) || return 1
  run_case 'slash/pipe|backslash\\' '/' $((i++)) || return 1
  run_case 'endswithpatternXYZ' 'XYZ' $((i++)) || return 1
  run_case 'XYZstartswith' 'XYZ' $((i++)) || return 1
  run_case 'nomatchhere' 'longpattern' $((i++)) || return 1
  pass=$((pass + i - 1))
  total=$((total + i - 1))
}

# Randomized fuzzing
fuzz_cases() {
  local -i cases=${1:-300}
  local sets=("$CS_ALNUM" "$CS_ALNUM_SP" "$CS_PUNCT")
  local -i idx=1
  for ((t=0; t<cases; t++)); do
    # pick charset
    cs="${sets[ RANDOM % ${#sets[@]} ]}"

    # input length 0..400 (kept moderate)
    in_len=$(( RANDOM % 401 ))
    input="$(rand_str "$in_len" "$cs")"

    # choose pattern:
    # 60%: substring from input (if not empty), with len 1..min(12, remain)
    # 40%: random pattern of len 1..8
    if (( in_len > 0 && RANDOM % 100 < 60 )); then
      if (( in_len == 1 )); then
        p_len=1
        pos=0
      else
        p_len=$(( 1 + RANDOM % (in_len < 12 ? in_len : 12) ))
        pos=$(( RANDOM % (in_len - p_len + 1) ))
      fi
      pattern="${input:pos:p_len}"
    else
      p_len=$(( 1 + RANDOM % 8 ))
      pattern="$(rand_str "$p_len" "$cs")"
    fi

    if ! run_case "$input" "$pattern" "$((t + 1))"; then
      total=$((total + 1))
      return 1
    fi
    pass=$((pass + 1))
    total=$((total + 1))
    # Optionally show progress every 50 cases
    if (( (t + 1) % 50 == 0 )); then
      echo "[progress] $((t + 1)) randomized cases passed..."
    fi
  done
}

echo "[run] fixed cases..."
fixed_cases

echo "[run] randomized fuzzing..."
fuzz_cases 400

echo "OK: $pass/$total cases passed"
fuzz_cases 400

echo "OK: $pass/$total cases passed"
