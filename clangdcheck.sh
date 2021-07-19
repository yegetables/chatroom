#!/bin/bash

SRC=$(git ls-tree --full-tree -r HEAD | grep -e "\.\(c\|h\|hpp\|cpp\)\$" | cut -f 2)
# echo $SRC >list_reporters
if [ ! -e "./compile_commands.json" ]; then
    cp build/compile_commands.json compile_commands.json
fi

clang-tidy $SRC >clang-tidy-report.yaml
clang-format $SRC -i >clang-format-report.yaml
