import socket

class MySocket:
  '''
  Class to modify `send` and `recv` methods in socket class.
  '''
  def __init__(self, sock=None):
    if sock is None:
      self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    else:
      self.sock = sock

  def connect(self, host, port):
    self.sock.connect((host,port))

  # Make sure the request is sent
  def send(self, request):
    total_sent = 0
    while total_sent < len(request):
      sent = self.sock.send(request[total_sent:])
      if sent == 0:
        raise RuntimeError("Socket connection broken")
      total_sent += sent

  #def myreceive(self):
  #    chunks = []
  #    bytes_recd = 0
  #    while bytes_recd < MSGLEN:
  #        chunk = self.sock.recv(min(MSGLEN - bytes_recd, 2048))
  #        if chunk == b'':
  #            raise RuntimeError("socket connection broken")
  #        chunks.append(chunk)
  #        bytes_recd = bytes_recd + len(chunk)
  #    return b''.join(chunks)
  
def communicate(host, port, request):
  # Create an INET, STREAMing socket
  s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
  # Connect to `host` on port `port`.
  s.connect((host, port))

  # `send`` and `recv` operate on network buffers. They only handle
  # buffers, not necessarily all the bytes that are handed the server.
  # `send` and `recv` return only when network buffers have been filled
  # (send) or emptied (recv). They then tell you how many bytes they
  # handled.
  #
  # A returned empty bytes (0) indicates that the client has disconnected.
  # This means that the other side (the server) has closed (or is in the
  # process of closing) the connection. No more data will be received on
  # on this connection. However, data can be sent successfully.

  # In HTTP protocol, a socket is used only for one transfer. The client
  # sends the request, and then reads the reply. That's it, after that the
  # socket is discarded. And the client can detect the end of the the
  # reply by receiving 0 bytes.
  s.send(request)
  response = s.recv(1024)
  s.close()
  return response