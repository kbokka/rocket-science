#!/usr/bin/env python

###  Modified for cryptBreak by: Samodya Abeysiriwardane

###  Edited Version of original File:
###  DecryptForFun.py
###  Avi Kak  (kak@purdue.edu)
###  January 14, 2014
###  Usage: cryptBreak.py crypted.txt decrypted.txt

PassPhrase = "Hopes and dreams of a million years"

import re
import sys
import binascii
from BitVector import *                                         #(A)

if len(sys.argv) is not 3:                                      #(B)
    sys.exit('''Needs two command-line arguments, one for '''
             '''the encrypted file and the other for the '''
             '''decrypted output file''')

BLOCKSIZE = 16                                                  #(C)
numbytes = BLOCKSIZE / 8                                        #(D)

# Reduce the passphrase to a bit array of size BLOCKSIZE:
bv_iv = BitVector(bitlist = [0]*BLOCKSIZE)                      #(E)
for i in range(0,len(PassPhrase) / numbytes):                   #(F)
    textstr = PassPhrase[i*numbytes:(i+1)*numbytes]             #(G)
    bv_iv ^= BitVector( textstring = textstr )                  #(H)

# Create a bitvector from the ciphertext hex string:
FILEIN = open(sys.argv[1])                                      #(I)
encrypted_bv = BitVector( hexstring = FILEIN.read() )           #(J)

###### MODIFICATIONS #########

marktwain = re.compile(r"mark twain", flags=re.IGNORECASE)
# Try all possible bit combinations
for bruteskey in xrange(0, 2**16): 
    # Reduce the key to a bit array of size BLOCKSIZE:
    key_bv = BitVector(intVal=bruteskey, size=BLOCKSIZE)        #(P)
    print key_bv

    # Create a bitvector for storing the output plaintext bit array:
    msg_decrypted_bv = BitVector( size = 0 )                    #(T)

    # Carry out differential XORing of bit blocks and decryption:
    previous_decrypted_block = bv_iv                            #(U)
    for i in range(0, len(encrypted_bv) / BLOCKSIZE):           #(V)
        bv = encrypted_bv[i*BLOCKSIZE:(i+1)*BLOCKSIZE]          #(W)
        temp = bv.deep_copy()                                   #(X)
        bv ^=  previous_decrypted_block                         #(Y)
        previous_decrypted_block = temp                         #(Z)
        bv ^=  key_bv                                           #(a)
        msg_decrypted_bv += bv                                  #(b)

    outputtext = msg_decrypted_bv.getTextFromBitVector()        #(c)
    if marktwain.search(outputtext):
        # Generate the ASCII version of Key
        ascii_key = binascii.unhexlify('%x' % key_bv.intValue()) 
	print "**** WOO HOO ****"
        print "Bit Key      : ", str(key_bv)
        print "Key in ASCII : ", ascii_key
	print "Text: \n", outputtext
	break

###### MODIFICATIONS END #########

# Write the plaintext to the output file:
FILEOUT = open(sys.argv[2], 'w')                                #(d)
FILEOUT.write(outputtext)                                       #(e)
FILEOUT.close()                                                 #(f)
