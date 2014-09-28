#!/usr/bin/env python

##  GCD.py

def gcd(a,b):
    while b:
        a,b = b, a%b
    return a

arg1 = 321451443876
arg2 = 12555473728888

gcdval = gcd( arg1, arg2 )
print "GCD is: ", gcdval
