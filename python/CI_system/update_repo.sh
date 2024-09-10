#!/bin/bash

source run_or_fail.sh

# delete previous version id
rm -f .commit_id

run_or_fail "Repository folder not found!" pushd $1 1> /dev/null