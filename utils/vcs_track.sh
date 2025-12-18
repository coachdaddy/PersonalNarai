#!/bin/bash
DIR="../include"
FILE="$DIR/vcs_track.h"
TEMP_FILE="$FILE.tmp"

if git rev-parse --git-dir > /dev/null 2>&1; then
    HASH=$(git rev-parse HEAD)
    SHORT_HASH=$(git rev-parse --short=8 HEAD)
    DATE=$(git show -s --format=%ci HEAD)
else
    HASH="UNKNOWN"
    SHORT_HASH="UNKNOWN"
    DATE="UNKNOWN"
fi

# 임시 파일 작성
echo "// THIS FILE IS AUTO GENERATED" > "$TEMP_FILE"
echo "// DO NOT TRACK THIS FILE WITH THE VCS" >> "$TEMP_FILE"
echo "#ifndef __VCS_TRACK_H__" >> "$TEMP_FILE"
echo "#define __VCS_TRACK_H__" >> "$TEMP_FILE"
echo "#define VCS_TRACK_HASH \"$HASH\"" >> "$TEMP_FILE"
echo "#define VCS_TRACK_SHORT_HASH \"$SHORT_HASH\"" >> "$TEMP_FILE"
echo "#define VCS_TRACK_DATE \"$DATE\"" >> "$TEMP_FILE"
echo "#endif" >> "$TEMP_FILE"

# 내용이 변경되었을 때만 실제 파일 업데이트
if [ -f "$FILE" ] && cmp -s "$TEMP_FILE" "$FILE"; then
    rm "$TEMP_FILE"
    echo "vcs_track.h is up to date."
else
    mv "$TEMP_FILE" "$FILE"
    echo "vcs_track.h updated."
fi
