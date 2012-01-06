#!/bin/bash

if [ "`uname -s`" = "Linux" ]; then
	bochs-term -f build/bochsrc.txt
else
	if [ "`uname -s`" = "Darwin" ]; then
		bochs -f build/bochsrc.txt
	fi
fi
