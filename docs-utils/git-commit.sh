#!/usr/bin/env bash
#
# This script is used to commit changes to the git repository.

now=`date '+%Y/%m/%d %H:%M:%S'`
git add -A
git commit -S -m "${now}"
