import argparse
import socket
import socketserver
import time

import helpers

'''
  Custom class that adds threading ability to the default 
  SocketServer.
'''
class ThreadingTCPServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
  runners = [] # Test runner pool
  dead = False # Indicate to other threads that the server is not running
  dispathced_commits = {} # To keep track of dispatched commits
  pending_commits = [] # Keep track of commits we have not yet dispatched


def runner_checker(server):
  """
    Function to periodically ping each registered test runner to make
    sure they are still responsive. If they are not responsive, the
    runner is removed from the pool and its commit ID is dispatched
    to the next available runner.
  """
  def manage_commit_lists(runner):
    # Iterate over dispatched tests
    for commit, assigned_runner in server.dispatched_commits.items(): 
      if assigned_runner == runner:
        del server.dispatched_commits[commit]
        server.pending_commits.append(commit)
        break
    server.runners.remove(runner)

  while not server.dead:
    time.sleep(1)
    for runner in server.runners:
      s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
      try:
        response = helpers.communicate(runner['host'],
                                       runner['port'],
                                       "ping")
        if response != "pong":
          print(f"removing runner {runner}")
          manage_commit_lists(runner)
      except socket.error as e:
        manage_commit_lists(runner)

        

def serve():
  parser = argparse.ArgumentParser()
  parser.add_argument("--host",
                      help="dispatcher's hpst, by default is the localhost",
                      default="localhost",
                      action="store")
  parser.add_argument("--port",
                      help="dispatcher's port, by default it uses 8888",
                      default=8888,
                      action="store")
  args = parser.parse_args()
  
  # Start the dispatcher server
  server = ThreadingTCPServer((args.host, int(args.port)), DispatcherHandler)
  print(f"Serving on ({args.host}, {int(args.port)}")