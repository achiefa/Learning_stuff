"""
Example taken from `500 Lines or Less`
GitHub repo: https://github.com/aosabook/500lines/blob/master/ci/code/repo_observer.py
"""

import argparse
import subprocess
import os
import time
from socket import error as socket_err

import helpers

def poll():
  parser = argparse.ArgumentParser()
  parser.add_argument("--dispatch-server",
                      help="dispatcher host, " \
                      "by default it uses localhost:8888",
                      default="localhost:8888",
                      action="store")
  
  parser.add_argument("repo", metavar="REPO", type=str,
                      help="path to the repository this will observe")
  
  args = parser.parse_args()
  dispatcher_host, dispatcher_port = args.dispatch_server.split(":")

  # periodically check the repository for changes
  while True:
      try:
          # call the bash script that will update the repo and check
          # for changes. If there's a change, it will drop a .commit_id file
          # with the latest commit in the current working directory
          subprocess.check_output(["./update_repo.sh", args.repo])
      except subprocess.CalledProcessError as e:
         raise Exception("Could not update and check repository. " +
                         f" Reason : {e.output}")
  

      # Check the existence of the `.commit_id` file. If it exists, there is
      # a new commit and the observer needs to notify the dispatcher so that
      # it can starts the tests.
      if os.path.isfile(".commit_id"):
        try:
          response = helpers.communicate(dispatcher_host,
                                           int(dispatcher_port),
                                           "status")
        except socket_err as e:
           raise Exception(f"Could not communicate with dispatcher server: {e}")
        
        if response == "OK":
           commit = ""
           with open(".commit_id", "r") as f:
              commit = f.readline()
              # Send the commit ID to the dispatcher
              response = helpers.communicate(dispatcher_host,
                                             int(dispatcher_port),
                                             f"dispatch: {commit}")
              
              if response != "OK":
                 raise Exception(f"Could not dispatch the test: {response}")
              print("dispatched!")
        else:
           raise Exception(f"Could not dispatch the test: {response}")
        
      time.sleep(5)
         

if __name__ == "__main__":
  poll()