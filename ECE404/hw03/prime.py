#! /usr/bin/env python

import re
import subprocess
import sys

if len(sys.argv) < 2:
    print "Usage: prime.py <primes-check> "
    sys.exit(1)

# From FindMI.py
# Faster than calling the file
## The code shown below uses ordinary integer arithmetic implementation of
## the Extended Euclid's Algorithm to find the MI of the first-arg integer
## vis-a-vis the second-arg integer.
def MI(num, mod):
    '''
    The function returns the multiplicative inverse (MI) of num modulo mod
    '''
    NUM = num; MOD = mod
    x, x_old = 0L, 1L
    y, y_old = 1L, 0L
    while mod:
        q = num // mod
        num, mod = mod, num % mod
        x, x_old = x_old - q * x, x
        y, y_old = y_old - q * y, y
    if num != 1:
        return False
    else:
        return True

# Method of calling FindMI file to get prime number, was very slow for large 
# numbers
# is_prime = True
# n = int(sys.argv[1])
# for i in xrange(1, n):
#     subp = subprocess.Popen(["./FindMI.py %d %d" % (i, n,)], 
#                               stdout=subprocess.PIPE, shell=True)
#     (response, error) = subp.communicate() 
#     if response.find("NO MI") > 0:
#         is_prime = False
#         break


outf = open("prime.txt", "w")
is_prime = True

mod = int(sys.argv[1])
for i in xrange(1, mod):
    if not MI(i, mod):
        is_prime = False
        break

if is_prime:
    outf.write("%d is a prime\n" % mod)
else:
    outf.write("%d is not a prime\n" % mod)

outf.close()
