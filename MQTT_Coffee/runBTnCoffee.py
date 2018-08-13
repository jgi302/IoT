#!/usr/bin/python3

import socket
import RPi.GPIO as GPIO
import time
import os

def main():

  coffeePin = 14
  GPIO.setmode(GPIO.BCM)
  GPIO.setup(coffeePin, GPIO.OUT)
  GPIO.output(coffeePin, GPIO.HIGH)
  coffee_status = b'OFF'
  hostMACAddress = 'B8:27:EB:BD:1F:E4'
  port = 3
  backlog = 1
  size = 1024
  s = socket.socket(socket.AF_BLUETOOTH, socket.SOCK_STREAM, socket.BTPROTO_RFCOMM)
  s.bind((hostMACAddress,port))
  s.listen(backlog)


  while 1:
    try:
      client, address = s.accept()
      while 1:
        data = client.recv(size)
        if data == b'ON':
          coffee_status = data
          GPIO.output(coffeePin, GPIO.LOW)
          print(data)
          client.send(data)
        elif data == b'OFF':
          coffee_status = data
          GPIO.output(coffeePin, GPIO.HIGH)
          print(data)
          client.send(data)
        elif data == b'STATUS':
          print(coffee_status)
          client.send(coffee_status)

    except KeyboardInterrupt:
      print("Closing socket")
      GPIO.cleanup()
      s.close()
      client.close()
      os._exit(0)
    except:
      continue
# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
if __name__ == "__main__":
    main()
