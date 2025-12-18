#!/bin/bash
f="../include/vcs_track.h"
echo "// THIS FILE IS AUTO GENERATED" > $f
echo "// DO NOT TRACK THIS FILE WITH THE VCS" >> $f
echo "#ifndef __VCS_TRACK_H__" >> $f
echo "#define __VCS_TRACK_H__" >> $f
hash=$(git rev-parse HEAD)
echo "#define VCS_TRACK_HASH \"$hash\"" >> $f
hash=$(git rev-parse --short=8 HEAD)
echo "#define VCS_TRACK_SHORT_HASH \"$hash\"" >> $f
date=$(git show -s --format=%ci)
echo "#define VCS_TRACK_DATE \"$date\"" >> $f
echo "#endif" >> $f

