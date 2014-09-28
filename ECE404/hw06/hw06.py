#! /usr/bin/env python

# Samodya Abeysiriwardane
# ECE 404 HW #6
# RSA block cipher

import sys
from BitVector import BitVector


# P, Q generated using modified PrimeGenerator.py
# PrimeGenerator.py was modified to generate prime-1 coprime e
p = 332302551650268460897954734536272420319
q = 258222644969751264151459428110328891689
e = 65537
# d = 51045946630242965074234865575892110109352763218080182137347987188415402400657

bv_p = BitVector(intVal=p)
bv_q = BitVector(intVal=q)
bv_pi = bv_p.multiplicative_inverse(bv_q)
bv_qi = bv_q.multiplicative_inverse(bv_p)
pi = int(bv_pi) # p inverse mod q
qi = int(bv_qi) # q inverse mod p

n = p * q
bv_n = BitVector(intVal=n)
tn = (p - 1) * (q - 1)
bv_tn = BitVector(intVal=tn)
bv_e = BitVector(intVal=e)
bv_d = bv_e.multiplicative_inverse(bv_tn)
d = int(bv_d)

def mod_exp(a, b, n):
    result = 1
    while(b > 0):
        if(b & 1):
            result = (result * a) % n
        b = b >> 1
        a = (a * a) % n
    return result

# This function can only be used by Decryption party because it's the only one 
# that knows prime factors p, q of n
# Faster than normal power using Chinese Remender Theorem
def super_power(a, b, n, primep, primeq):
    vp = mod_exp(a, b, primep)
    vq = mod_exp(a, b, primeq)
    xp = q * qi
    xq = p * pi
    return ((vp * xp) + (vp * xq)) % n

def encrypt(inputf_name, outputf_name):
    bvfile = BitVector(filename=inputf_name)
    outf = open(outputf_name, "wb")

    while(bvfile.more_to_read):
        bit_block = bvfile.read_bits_from_file(128)
        
        # pad if bit_block size is less than 128 bits 
        if len(bit_block) != 128:
            b_newline = '00001010'
            b_string = str(bit_block) + b_newline * ((128 - len(bit_block)) / 8)
            bit_block = BitVector(bitstring=b_string)
            assert (len(bit_block) == 128), "Bitvec bitstring error"

        # prepend with 0s
        bit_block.pad_from_left(128)
        out = mod_exp(int(bit_block), e, n) 
        bit_out = BitVector(intVal=out, size=256)
        bit_out.write_to_file(outf)

    outf.close()
    bvfile.close_file_object()

def decrypt(inputf_name, outputf_name):
    bvfile = BitVector(filename=inputf_name)
    outf = open(outputf_name, "wb")

    while(bvfile.more_to_read):
        bit_block = bvfile.read_bits_from_file(256)
        assert (len(bit_block) == 256), "Bitblock is not long enough"
        out = super_power(int(bit_block), d, n, p, q)
        bit_out = BitVector(intVal=out, size=256)
        # remove prepended 0s
        bit_out = bit_out[128:]
        bit_out.write_to_file(outf)

    outf.close()
    bvfile.close_file_object()

if len(sys.argv) < 4:
    print "usage: %s -[e|d] input output" % sys.argv[0]
    sys.exit(1)

if sys.argv[1] == '-e':
    encrypt(sys.argv[2], sys.argv[3])
elif sys.argv[1] == '-d':
    decrypt(sys.argv[2], sys.argv[3])
