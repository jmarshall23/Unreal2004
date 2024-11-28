#!/bin/sh

if [ -z $1 ]; then
  UNREAL_BIN="ut2004-bin"
else
  UNREAL_BIN="$1"
fi

gdb ./$UNREAL_BIN `ps ax |grep $UNREAL_BIN |perl -w -e '$_ = <STDIN>; chomp; s/\A\s*?(\d+).*?\Z/$1/; print("$_\n");'`


