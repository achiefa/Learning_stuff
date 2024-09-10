"""
Example taken from `500 Lines or Less`
GitHub repo: https://github.com/aosabook/500lines/blob/master/ci/code/repo_observer.py
"""

import argparse
import subprocess

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
         

if __name__ == "__main__":
  poll()