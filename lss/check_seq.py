import sys

tnum = 0
fi = open(sys.argv[1])
for line in fi:
    num = int(line)
    if num < tnum:
        print "Error", tnum, num
        break
    tnum = num
