#!/bin/sh

[ -d tmp ] || mkdir tmp
                                > tmp/genLayout.sh
cat genLayout_prefix.sh        >> tmp/genLayout.sh
awk -f makeLayout.awk < layout >> tmp/genLayout.sh
cat genLayout_suffix.sh        >> tmp/genLayout.sh

sh tmp/genLayout.sh
