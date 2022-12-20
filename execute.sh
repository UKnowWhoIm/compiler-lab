#!/bin/sh
dir=$1
output=${2:-$1/a.out}
cd $dir
bison *.y 
flex *.l
gcc lex.yy.c
cd ..
./$dir/a.out
