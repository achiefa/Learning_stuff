#!/bin/bash

run_or_fail() {
  local explanation=$1 # local scope
  shift 1
  "$@"
  if [ $? != 0 ]; then # `$?` holds the exit status of the last command executed.
    echo $explanation 1>&2 # redirect the standard output (stdout) to the standard error (stderr)
    exit 1
  fi
}