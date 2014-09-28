#!/usr/bin/env python

## FindMI.py

## It is meant to be called as
##
##      FindMI.py  17  119
##
## if you want to find the multiplicative inverse of 17 modulo 120

## This is for finding the multiplicative invers of the first arg integer
## in the set of remainders Z_n formed by the second arg integer.

outf = open("mitest_fmi.txt", "w")

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
        outf.write("%d %d no\n" % (NUM, MOD,))
    else:
        MI = (x_old + MOD) % MOD
        outf.write("%d %d %d\n" % (NUM, MOD, MI))
        return MI

for a in xrange(1, 1000):
    for p in xrange(1, 1000):
        x = MI(a, p)

outf.close()
