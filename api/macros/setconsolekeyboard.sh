#!/bin/sh

if [ $# -ne 1 -a $# -ne 2 ]; then
  echo "Usage: $(basename $0) <xkb_layout> [xkb_variant]"
  exit 1
fi

xkblayout="$1"
if [ "$1" == "cz" -o "$1" == "sk" ]; then
  if [ "$2" == "qwerty" ]; then
    xkblayout="$1_qwerty"
  fi
fi

layout=""
layout=$(grep -E \"${xkblayout}\" /usr/share/YaST2/data/xkblayout2keyboard.ycp | \
         cut -d ":" -f 2 | sed -e 's/\"//g' -e 's/,//g' -e 's/ //g')

if [ "$layout" == "" ]; then
  echo "No appropriate console keyboard layout found for $1($2)"
  exit 1
else
  echo "Setting console keyboard layout to $layout (xkb-layout: $1($2))"
  yast keyboard set layout=${layout}
fi
