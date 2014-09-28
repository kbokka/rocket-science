#!/usr/bin/env python

# Aurthor: Samodya Abeysiriwardane
# HW 05 RC4

import sys
import wave

class RC4:
    def __init__(self, key):
        self.key = key
    
    def encrypt(self, bytepacket):
        # pseudo random byte generator RC4
        prbgen = self._pseudo_random_byte_gen(self.key) 
        # For each byte in input bytepacket xor with random byte from the generator
        output = [chr(ord(pb) ^ prbgen.next()) for pb in bytepacket]
        return ''.join(output)

    def decrypt(self, bytepacket):
        # pseudo random byte generator RC4
        prbgen = self._pseudo_random_byte_gen(self.key) 
        # For each byte in input bytepacket xor with random byte from the generator
        output = [chr(ord(eb) ^ prbgen.next()) for eb in bytepacket]
        return ''.join(output)

    def _pseudo_random_byte_gen(self, key):
        """Returns a RC4 pseudo random byte generator object"""
        S = [i for i in xrange(0, 256)]
        keylen = len(key)
        T = [ord(key[i % keylen]) for i in xrange(0, 256)]
        
        j = 0
        for i in xrange(0, 256):
            j = (j + S[i] + T[i]) % 256
            S[i], S[j] = S[j], S[i]

        i = 0
        j = 0
        while True:
            i = (i + 1) % 256
            j = (j + S[i]) % 256
            S[i], S[j] = S[j], S[i]
            k = (S[i] + S[j]) % 256
            yield S[j]


def main():
    if len(sys.argv) < 5:
        print "usage: -[e|d] key infile outfile"
        sys.exit(1)

    rc4cipher = RC4(sys.argv[2])

    infile = wave.open(sys.argv[3], 'r')
    original_length = infile.getnframes()
    original = infile.readframes(original_length)
    
    outfile = wave.open(sys.argv[4], 'w')
    outfile.setparams(infile.getparams())
    
    if sys.argv[1] == "-e":
        output = rc4cipher.encrypt(original)
        outfile.writeframes(output)
    else:
        output = rc4cipher.decrypt(original)
        outfile.writeframes(output)
    
    infile.close()
    outfile.close()

if __name__ == "__main__":
    main()
