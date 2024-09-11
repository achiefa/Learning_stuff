#!/bin/bash


source run_or_fail.sh

# delete previous version id
rm -f .commit_id

# Check if the folder exists
run_or_fail "Repository folder not found!" pushd $1 1> /dev/null

# Reset the repository to the most recent (local) commit, in case
# anything got it out of sync
run_or_fail "Could not reset git!" git reset --hard HEAD

COMMIT=$(run_or_fail "Could not call 'git log' on repository" git log -n1)
if [ $? != 0 ]; then
  echo "Could not call 'git log' on repository"
  exit 1
fi

COMMIT_ID=`echo $COMMIT | awk '{ print $2 }'`

run_or_fail "Could not pull from repository" git pull
COMMIT=$(run_or_fail "Could not call 'git log' on repository" git log -n1)
if [ $? != 0 ]; then
  echo "Could not call 'git log' on repository"
  exit 1
fi

NEW_COMMIT_ID=`echo $COMMIT | awk '{ print $2 }'`

# Check if the new commit matches the previous one.
# If not, save the latest commit ID in the .commit_id file
if [ $NEW_COMMIT_ID != $COMMIT_ID ]; then
  popd >1 /dev/null # Returns to the previous directory
  echo $NEW_COMMIT_ID > .commit_id
fi

