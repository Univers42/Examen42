#!/bin/bash
# filepath: /home/dlesieur/Documents/Examen42/Exam_Rank_03/powerset/tester.sh

# Run your powerset program and capture output
./a.out "$@" > output.txt

# Normalize whitespace (trim leading/trailing; collapse multiple spaces)
# MODE can be: by-line (default) or by-set (numbers-in-line sorted)
MODE="${MODE:-by-line}"

awk '
{
    if ($0 ~ /^[[:space:]]*$/) next;
    gsub(/^[[:space:]]+|[[:space:]]+$/, "", $0);  # trim
    gsub(/[[:space:]]+/, " ", $0);                # collapse spaces
    print $0;
}
' output.txt > normalized_raw.txt

if [ "$MODE" = "by-set" ]; then
    # Sort numbers within each subset to compare by value regardless of order
    awk '
    {
        split($0, arr, " ");
        n = length(arr);
        asort(arr);
        line = "";
        for (i = 1; i <= n; i++) {
            if (arr[i] != "") {
                if (line != "") line = line " ";
                line = line arr[i];
            }
        }
        print line;
    }
    ' normalized_raw.txt > normalized.txt
else
    mv normalized_raw.txt normalized.txt
fi

# Detect duplicates and show counts
LC_ALL=C sort normalized.txt | uniq -c | awk '
{
    cnt = $1; $1 = ""; sub(/^ +/, "");
    if (cnt > 1) { print "Duplicate detected x" cnt ": " $0; found = 1; }
}
END {
    if (!found) print "No duplicates found!";
}
'