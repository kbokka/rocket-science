#!/usr/bin/env python

# Samodya Abeysiriwardane
# ECE 404
# HW 3
# Stein algorithm implementation


## multiplyinv.py

## It is meant to be called as
##
##      multiplyinc.py 17 120
##
## if you want to find the multiplicative inverse of 17 modulo 120

## This is for finding the multiplicative invers of the first arg integer
## in the set of remainders Z_n formed by the second arg integer.

# From BGCD.py
def bgcd(a,b):
    if a == b: return a                                         #(A)
    if a == 0: return b                                         #(B)
    if b == 0: return a                                         #(C)
    if (~a & 1):                                                #(D)
        if (b &1):                                              #(E)
            return bgcd(a >> 1, b)                              #(F)
        else:                                                   #(G)
            return bgcd(a >> 1, b >> 1) << 1                    #(H)
    if (~b & 1):                                                #(I)
        return bgcd(a, b >> 1)                                  #(J)
    if (a > b):                                                 #(K)
        return bgcd( (a-b) >> 1, b)                             #(L)
    return bgcd( (b-a) >> 1, a )                                #(M)

# Stein algorithm implementation
def MI(num, mod):
    '''
    The function returns the multiplicative inverse (MI) of num modulo mod
    Assuming that it exists
    '''
    while num % 2 == 0 and mod % 2 == 0:
        num = num >> 1
        mod = mod >> 1

    if mod == 1 or mod == -1:
        return 0
    
    u1, u2, u3 = 1, 0, num
    v1, v2, v3 = mod, 1 - num, mod

    if num % 2 == 1:
        t1, t2, t3 = 0, -1 ,-mod
    else:
        t1, t2, t3 = 1, 0, num

    # dowhile = True
    while t3 is not 0:
        while t3 % 2 == 0:
            t3 = t3 >> 1
        
            if t1 % 2 == 0 and t2 % 2 == 0:
                t1 = t1 >> 1
                t2 = t2 >> 1
            else:
                t1 = (t1 + mod) >> 1
                t2 = (t2 - num) >> 1

        if t3 > 0:
            u1, u2, u3 = t1, t2, t3
        else:
            v1 = mod - t1
            v2 = -(num + t2)
            v3 = -t3

        t1 = u1 - v1
        t2 = u2 - v2
        t3 = u3 - v3

        if t1 < 0:
            t1 = t1 + mod
            t2 = t2 - num

        # dowhile = t3 is not 0

    return u1

outf = open("mi_test.txt", "w")

for a in xrange(1, 1000):
    for p in xrange(1, 1000):
        if bgcd(a,p) == 1:
            x = MI(a, p)
            outf.write("%d %d %d\n" % (a, p, x))
        else:
            outf.write("%d %d no\n" % (a, p,))

outf.close()
