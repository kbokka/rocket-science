#! /usr/bin/sh

# firewall
iptables -A OUTPUT -j ACCEPT

# SSH accpet only from purdue.edu
iptables -A INPUT -p tcp --dport 22 -s purdue.edu -j ACCEPT

# http access only for 10.0.0.85
iptables -A INPUT -p tcp --dport 80 -s 10.0.0.85 -j ACCEPT

# Permit Auth/Ident port 113
iptables -A INPUT -p tcp --dport 113 -j ACCEPT

# Accept ICMP requests
iptables -A INPUT -p icmp --icmp-type echo-request -j ACCEPT

# Go easy on currently made connections
iptables -A INPUT -p all -m state --state ESTABLISHED,RELATED -j ACCEPT

# Block all other traffic with tcp reset
iptables -A INPUT -p all -j REJECT --reject-with tcp-reset
