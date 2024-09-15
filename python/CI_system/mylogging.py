import logging
import blessings

COLORS = {
    'BOLD': '\033[1m',      # Bold
    'DEBUG': '\033[94m',    # Blue
    'INFO': '\033[92m',     # Green
    'WARNING': '\033[93m',  # Yellow
    'ERROR': '\033[91m',    # Red
    'CRITICAL': '\033[95m', # Magenta
    'RESET': '\033[0m'      # Reset color
}


t = blessings.Terminal()

class ColoredFormatter(logging.Formatter):
  def format(self, record):
      log_color = COLORS.get(record.levelname, COLORS['RESET'])
      reset_color = COLORS['RESET']
      
      # Adding color to the log level
      record.levelname = f"{log_color}{record.levelname}{reset_color}"
      record.name = f"{COLORS['BOLD']}{record.name}{reset_color}"
      
      return super().format(record)
  
formatter = ColoredFormatter(
   '[%(levelname)s][%(name)s:%(threadName)s:%(funcName)s]: %(message)s'
)
ColoredHandler = logging.StreamHandler()  # Output to the console
ColoredHandler.setFormatter(formatter)