#!/usr/bin/env python
import signal
import sys



def signal_handler(sig, frame):
    print("I've been killed !", file=sys.stderr)
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

header = "Content-Type: text/html\r\n"
print(header,end='\r\n')

while True:
	pass
