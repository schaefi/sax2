#!/bin/sh

export LD_LIBRARY_PATH=../../

if [ ! -f test.exe ];then
	mcs test.cs -r:SaX.dll
fi
mono test.exe
