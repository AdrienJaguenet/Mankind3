#!/usr/bin/env bash

for f in *.c *.h
do
    indent -kr -ci2 -cli2 -i2 -l80 -nut $f &
done

wait
rm ./*~
