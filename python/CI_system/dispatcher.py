import argparse
import socket
import socketserver
import time
import re
import logging
import os
import threading
import sys
import signal

import re._compiler

import helpers
import mylogging

#logging.basicConfig(level=logging.DEBUG, 
                   #format='[%(levelname)s] - %(funcName)s - %(threadName)s : %(message)s')

'''
  Custom class that adds threading ability to the default 
  SocketServer.
'''
class ThreadingTCPServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
  runners = [] # Test runner pool - registered test runners
  dead = False # Indicate to other threads that the server is not running
  dispathced_commits = {} # To keep track of dispatched commits
  pending_commits = [] # Keep track of commits we have not yet dispatched
  allow_reuse_address = True

  def signal_handler(self, sig, frame):
    self.logger.warning('You pressed Ctrl+C!')
    sys.exit(0)

  def __init__(self, server_address, handler_class):
    self.logger = logging.getLogger('ThreadingTCPServer')
    self.logger.propagate = False
    self.logger.addHandler(mylogging.ColoredHandler)
    self.logger.setLevel(logging.DEBUG)
    self.logger.debug('Initialising ThreadingTCPServer')
    socketserver.TCPServer.__init__(self, server_address, handler_class)
    socketserver.TCPServer.allow_reuse_port = True
    return
  
  def server_activate(self):
    self.logger.debug('server_activate')
    socketserver.TCPServer.server_activate(self)
    return

  def serve_forever(self):
    self.logger.debug('waiting for request')
    self.logger.info('Handling requests, press <Ctrl-C> to quit')
    while True:
        self.handle_request()
    return

  def handle_request(self):
    self.logger.debug('handle_request...')
    return socketserver.TCPServer.handle_request(self)

  def verify_request(self, request, client_address):
    self.logger.debug('verify_request(%s, %s)', request, client_address)
    return socketserver.TCPServer.verify_request(self, request, client_address)

  def process_request(self, request, client_address):
    self.logger.debug('process_request(%s, %s)', request, client_address)
    return socketserver.TCPServer.process_request(self, request, client_address)

  def server_close(self):
    self.logger.debug('server_close')
    return socketserver.TCPServer.server_close(self)

  def finish_request(self, request, client_address):
    self.logger.debug('finish_request(%s, %s)', request, client_address)
    return socketserver.TCPServer.finish_request(self, request, client_address)

  def close_request(self, request_address):
    self.logger.debug('close_request(%s)', request_address)
    return socketserver.TCPServer.close_request(self, request_address)

class DispatcherHandler(socketserver.BaseRequestHandler):
  command_re = re.compile(r"(\w+)(:.+)*")
  BUF_SIZE = 1024

  def __init__(self, request, client_address, server):
   self.logger = logging.getLogger('DispatcherHandler')
   self.logger.propagate = False
   self.logger.addHandler(mylogging.ColoredHandler)
   self.logger.setLevel(logging.DEBUG)
   self.logger.debug('Initialising DispatchHandler')
   socketserver.BaseRequestHandler.__init__(self, request, client_address, server)
  
  def setup(self):
   self.logger.debug("setup...")
   return socketserver.BaseRequestHandler.setup(self)
  
  def finish(self):
   self.logger.debug("finish...")
   return socketserver.BaseRequestHandler.finish(self)


  def handle(self):
    # Look at the incoming connection request
    self.data = self.request.recv(self.BUF_SIZE).strip()
    command_groups = self.command_re.match(self.data.decode('utf-8'))
    #self.logger.debug(f"Handling {command_groups}...")

    if not command_groups:
      self.request.sendall(b"Invalid command")
      return
    
    command = str(command_groups.group(1))

    if command == "status":
      self.logger.info("In status...")
      self.request.sendall(b"OK")
    
    elif command == "register":
      self.logger.info("Register")
      address = command_groups.group(2)
      host, port = '', ""
      try:
        host, port = re.findall(r":(\w*)", address)
      except TypeError as e:
        self.logger.warning(f"Client {self.client_address[0]} tried"\
                            " to register a new runner without providing"\
                            " an address.")
        self.request.sendall(b"[ERROR] The address of the runner must be "\
                             b"provided in the form register:host:port")
        return
      except ValueError as e:
        self.logger.warning(f"Client {self.client_address[0]} tried"\
                            " to register a new runner without providing"\
                            " either the host or the port.")
        self.request.sendall(b"[ERROR] The address of the runner must be "\
                             b"provided in the form register:host:port.")
        return
      
      runner = {"host" : host, "port" : port}
      self.server.runners.append(runner)
      self.request.sendall(b"OK")

    elif command == "dispatch":
      self.logger.info("Dispatching")
      commit_id = command_groups.group(2)[1:]
      if not self.server.runners:
        self.request.sendall(b"No runners registered")
      else:
        self.request.sendall(b"OK")
        dispatch_tests(self.server, commit_id)

    elif command == "results":
      self.logger.info(f"Received results command")
      results = command_groups.group(2)[1:] # command_group = results:<commit ID>:<length of results data in bytes>:<results>
      results = results.split(":")
      commit_id = results[0]
      length_msg = int(results[1])
      remaining_buffer = self.BUF_SIZE - len(command) + len(commit_id) + len(results[1]) + 3
      if length_msg > remaining_buffer:
        self.data += self.request.recv(length_msg - remaining_buffer) # (length_msg - remaining_buffer) is the actual size of <results>
      del self.server.dispatched_commits[commit_id]

      if not os.path.exists("test_results"):
        os.makedirs("test_results")

      with open(f"test_results/{commit_id}", "w") as f:
                data = self.data.split(":")[3:]
                data = "\n".join(data)
                f.write(data)
      self.request.sendall(b"OK")

    else:
      self.logger.info(f"Received invalid command: {command}")
      self.request.sendall(b"Invalid command")
     

def manage_commit_lists(server, runner):
  """
  Function that removes the test runner `runner` from the
  dispatched commits in the server. This test runner is then
  dispatched to the next available runner (in the `pending_commits`
  list)
  """
  # Iterate over dispatched tests
  for commit, assigned_runner in server.dispatched_commits.items(): 
    if assigned_runner == runner:
      del server.dispatched_commits[commit]
      server.pending_commits.append(commit)
      break
  server.runners.remove(runner)

def runner_checker(server):
  """
    This function periodically pings the registered test runners
    to make sure they are still responsive. If a test runner is 
    unresponsive, it is remover from the pool and the respective
    commit ID is dispatched to the next available runner.
  """
  log = logging.getLogger("Server")
  log.debug("Starting runner_checker")
  while not server.dead:
    time.sleep(1)
    for runner in server.runners:
      # Do I need this?
      #s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
      try:
        response = helpers.communicate(runner['host'],
                                       runner['port'],
                                       "ping")
        if response != "pong":
          print(f"Removing runner {runner}")
          manage_commit_lists(server, runner)
      except socket.error as e:
        manage_commit_lists(server, runner)

def dispatch_tests(server, commit_id):
  """
  Find an available test runner from the pool of registered
  test runners. When `dispatch_tests` finds an available runner,
  it sends a runtest message to the runner with the commit ID. If
  none is available, it waits 2 seconds before repeating the process.
  
  The `dispatch_tests` functions logs which commit ID is being tested
  by which test runner in the `dispatched_commits` variable. If the
  commit ID dispatched by this function is in `pending_commits`, then
  the commit is removed from this variable.
  """
  log = logging.getLogger("server")
  while True:
    print("Trying to dispatch to runners")
    for runner in server.runners:
      response = helpers.communicate(runner["host"],
                                     int(runner["port"]),
                                     f"runtest:{commit_id}")
      if response == "OK":
        log.info("Adding id {commit_id}...")
        server.dispatched_commits[commit_id] = runner
        if commit_id in server.pending_commits:
          server.pending_commits.remove(commit_id)
        return
    time.sleep(2)

def redistribute(server):
  """
  Function that dispatches the commit IDs logged in `pending_commits`.
  Commit IDs are dispatched using the `dispatched_tests` function.
  """
  log = logging.getLogger("server")
  log.debug("starting redistribute")
  while not server.dead:
    for commit in server.pending_commits:
      print(f"Runnig redistribute...")
      print(server.pending_commits)
      dispatch_tests(server, commit)
      time.sleep(5)

def serve():
  log = logging.getLogger("server")
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
  log.info(f"Serving on ({args.host}, {int(args.port)})")

  # Thread to check runner pool
  runner_heartbeat = threading.Thread(target=runner_checker, args=(server,))
  redistributor = threading.Thread(target=redistribute, args=(server,))

  try:
    runner_heartbeat.start()
    redistributor.start()
    # Activate the server; this will keep running until you
    # interrupt the program with Ctrl+C or Cmd+C
    server.serve_forever()
  except KeyboardInterrupt:
    # if any exception occurs, kill the thread
    log.warning("Ctrl+C detected, shutting down the server...")
    server.dead = True
    runner_heartbeat.join()
    redistributor.join()
  except Exception:
    # if any exception occurs, kill the thread
    log.warning("Exception detected, shutting down the server...")
    server.dead = True
    runner_heartbeat.join()
    redistributor.join()


if __name__ == "__main__":
  log = logging.getLogger("server")
  log.propagate = False
  log.addHandler(mylogging.ColoredHandler)
  log.setLevel(logging.DEBUG)
  serve()