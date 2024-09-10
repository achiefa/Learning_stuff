#!/bin/bash

run_or_fail() {
  local explanation=$1 # local scope
  shift 1 # shift args to the left by 1
  "$@"
  if [ $? != 0 ]; then # `$?` holds the exit status of the last command executed.
    echo $explanation 1>&2 # redirect the standard output (stdout) to the standard error (stderr)
    # 1 is the file descriptor for standard output (stdput)
    # 2 is the file descriptor for standard error (stderr). The `&` before 
    # indicates that `2` is a file file descriptor and not a file name
    exit 1
  fi
}