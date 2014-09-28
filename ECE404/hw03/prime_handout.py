#! /usr/bin/env python

# Author: Samodya Abeysiriwardane
# ECE 404
# HW 03

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

outf = open("prime_handout.txt", "w")
for mod in xrange(10001, 20000):
    is_prime = True
    for i in xrange(1, mod):
        if not MI(i, mod):
            is_prime = False
            break
        
    if is_prime:
        outf.write("%d is a prime\n" % mod)
    # else:
    #    outf.write("%d is not a prime\n" % mod)

outf.close()
