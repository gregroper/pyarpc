import time
from arduino import *
import json_rpc

json_rpc.open_port('/dev/ttyACM0', 115200, None)

pinMode(2, 1)
tone(2, 100, 20000)
