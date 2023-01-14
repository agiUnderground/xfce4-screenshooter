#!/bin/sh
set -x
find . -name '*.po' -print -execdir ./clean_po.sh {} \;
