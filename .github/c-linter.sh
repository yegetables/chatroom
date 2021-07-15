#!/bin/bash

if [[ -z "$GITHUB_TOKEN" ]]; then
    echo "The GITHUB_TOKEN is required."
    exit 1
fi

clang-tidy --version

SRC=$(git ls-tree --full-tree -r HEAD | grep -e "\.\(c\|h\|hpp\|cpp\)\$" | cut -f 2)
echo $SRC >list_reporters
cp ./build/compile_commands.json ./compile_commands.json

while read line; do
    clang-tidy $SRC >>clang-tidy-report.txt
    # echo $line
done <list_reporters
while read line; do
    # clang-tidy $SRC >clang-tidy-report.txt
    # echo $line
    clang-format $SRC >>clang-format-report.txt
done <list_reporters

# cppcheck --enable=all --std=c++11 --language=c++ --output-file=cppcheck-report.txt *

PAYLOAD_TIDY=$(cat clang-tidy-report.txt)
PAYLOAD_FORMAT=$(cat clang-format-report.txt)
# PAYLOAD_CPPCHECK=`cat cppcheck-report.txt`
COMMENTS_URL=$(cat $GITHUB_EVENT_PATH | jq -r .pull_request.comments_url)

echo $COMMENTS_URL
echo "Clang-tidy errors:"
echo $PAYLOAD_TIDY
echo "Clang-format errors:"
echo $PAYLOAD_FORMAT
# echo "Cppcheck errors:"
# echo $PAYLOAD_CPPCHECK

if [ ${#PAYLOAD_TIDY} -ne 0]; then
    OUTPUT=$'**CLANG-TIDY WARNINGS**:\n'
    OUTPUT+=$'\n```\n'
    OUTPUT+="$PAYLOAD_TIDY"
    OUTPUT+=$'\n```\n'
fi

if [ ${#PAYLOAD_FORMAT} -ne 0]; then
    OUTPUT=$'**CLANG-FORMAT WARNINGS**:\n'
    OUTPUT+=$'\n```\n'
    OUTPUT+="$PAYLOAD_FORMAT"eCTF20/mb/drm_audio_fw/src on
    OUTPUT+=$'\n```\n'
fi
if [ ${#PAYLOAD_CPPCHECK} -ne 0]; then
    OUTPUT=$'\n**CPPCHECK WARNINGS**:\n'
    OUTPUT+=$'\n```\n'
    OUTPUT+="$PAYLOAD_CPPCHECK"
    OUTPUT+=$'\n```\n'
fi
# PAYLOAD=$(echo '{}' | jq --arg body "$OUTPUT" '.body = $body')

# curl -s -S -H "Authorization: token $GITHUB_TOKEN" --header "Content-Type: application/vnd.github.VERSION.text+json" --data "$PAYLOAD" "$COMMENTS_URL"
