#!/usr/bin/env bash

GLOBIGNORE=math_3d.h
for f in *.c *.h
do
    indent -ts4 -kr -ci2 -cli2 -l80 $f &
done

wait
rm ./*~
