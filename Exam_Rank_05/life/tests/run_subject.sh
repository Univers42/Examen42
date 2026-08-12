#!/bin/sh
# Replays the five invocations printed in ../subject.txt and writes the same
# transcript expected_subject.txt holds, so the two can be diffed.
#
#   usage: ./run_subject.sh path/to/life

set -u
LIFE=${1:-../life}

emit() {
    printf "\$> echo '%s' | ./life %s\n" "$1" "$2"
    printf '%s\n' "$1" | "$LIFE" $2
}

emit 'sdxddssaaww'                 '5 5 0'
emit 'sdxssdswdxddddsxaadwxwdxwaa' '10 6 0'
emit 'dxss'                        '3 3 0'
emit 'dxss'                        '3 3 1'
emit 'dxss'                        '3 3 2'
