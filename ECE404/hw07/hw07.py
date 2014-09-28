#! /usr/bin/env python

# Samodya Abeysiriwardane
# HW 07
# Hashing SHA-512

import sys
from BitVector import BitVector
# Use BitVector 3.3.2 or higher to work with newlines

k512 = (
    0x428A2F98D728AE22,0x7137449123EF65CD,0xB5C0FBCFEC4D3B2F,0xE9B5DBA58189DBBC,
    0x3956C25BF348B538,0x59F111F1B605D019,0x923F82A4AF194F9B,0xAB1C5ED5DA6D8118,
    0xD807AA98A3030242,0x12835B0145706FBE,0x243185BE4EE4B28C,0x550C7DC3D5FFB4E2,
    0x72BE5D74F27B896F,0x80DEB1FE3B1696B1,0x9BDC06A725C71235,0xC19BF174CF692694,
    0xE49B69C19EF14AD2,0xEFBE4786384F25E3,0x0FC19DC68B8CD5B5,0x240CA1CC77AC9C65,
    0x2DE92C6F592B0275,0x4A7484AA6EA6E483,0x5CB0A9DCBD41FBD4,0x76F988DA831153B5,
    0x983E5152EE66DFAB,0xA831C66D2DB43210,0xB00327C898FB213F,0xBF597FC7BEEF0EE4,
    0xC6E00BF33DA88FC2,0xD5A79147930AA725,0x06CA6351E003826F,0x142929670A0E6E70,
    0x27B70A8546D22FFC,0x2E1B21385C26C926,0x4D2C6DFC5AC42AED,0x53380D139D95B3DF,
    0x650A73548BAF63DE,0x766A0ABB3C77B2A8,0x81C2C92E47EDAEE6,0x92722C851482353B,
    0xA2BFE8A14CF10364,0xA81A664BBC423001,0xC24B8B70D0F89791,0xC76C51A30654BE30,
    0xD192E819D6EF5218,0xD69906245565A910,0xF40E35855771202A,0x106AA07032BBD1B8,
    0x19A4C116B8D2D0C8,0x1E376C085141AB53,0x2748774CDF8EEB99,0x34B0BCB5E19B48A8,
    0x391C0CB3C5C95A63,0x4ED8AA4AE3418ACB,0x5B9CCA4F7763E373,0x682E6FF3D6B2B8A3,
    0x748F82EE5DEFB2FC,0x78A5636F43172F60,0x84C87814A1F0AB72,0x8CC702081A6439EC,
    0x90BEFFFA23631E28,0xA4506CEBDE82BDE9,0xBEF9A3F7B2C67915,0xC67178F2E372532B,
    0xCA273ECEEA26619C,0xD186B8C721C0C207,0xEADA7DD6CDE0EB1E,0xF57D4F7FEE6ED178,
    0x06F067AA72176FBA,0x0A637DC5A2C898A6,0x113F9804BEF90DAE,0x1B710B35131C471B,
    0x28DB77F523047D84,0x32CAAB7B40C72493,0x3C9EBE0A15C9BEBC,0x431D67C49C100D4C, 
    0x4CC5D4BECB3E42B6,0x597F299CFC657E2A,0x5FCB6FAB3AD6FAEC,0x6C44198C4A475817,
)

def sha512(msg):
    def sigma_0(x):
        return (x[:] >> 1) ^ (x[:] >> 8) ^ (x[:].shift_right(7))

    def sigma_1(x):
        return (x[:] >> 19) ^ (x[:] >> 61) ^ (x[:].shift_right(6))

    def gamma_0(x):
        return (x[:] >> 28) ^ (x[:] >> 34) ^ (x[:] >> 39)

    def gamma_1(x):
        return (x[:] >> 14) ^ (x[:] >> 18) ^ (x[:] >> 41)

    def Ch(e, f, g):
        return (e & f) ^ (~e & g)

    def Maj(a, b, c):
        return (a & b) ^ (a & c) ^ (b & c)

    def T1(h, ch, g1, w, k):
        return (int(h) + int(ch) + int(g1) + int(w) + int(k)) % 2**64

    def T2(a, maj):
        return (int(a) + int(maj)) %  2**64

    iv_hexstrings = ('6a09e667f3bcc908', 'bb67ae8584caa73b', '3c6ef372fe94f82b',
            'a54ff53a5f1d36f1', '510e527fade682d1', '9b05688c2b3e6c1f',
            '1f83d9abfb41bd6b', '5be0cd19137e2179')
    hb = [BitVector(hexstring=h) for h in iv_hexstrings] # hashbuffer

    msg_block_bv = BitVector(textstring=msg)
    msg_length = msg_block_bv.length()
    msg_length_bv = BitVector(intVal=msg_length, size=128)
    zero_num = (896 - (msg_length + 1)) % 1024 
    pad_string = '1' + zero_num * '0'
    msg_padding_bv = BitVector(bitstring=pad_string)
    msg_block = msg_block_bv + msg_padding_bv + msg_length_bv

    words = [None] * 80
    for n in xrange(0, msg_block.length(), 1024):
        # Word schedule 
        block = msg_block[n:n+1024]
        words[0:16] = [block[i:i+64] for i in xrange(0, 1024, 64)]
        for i in xrange(16, 80):
            val = (int(words[i-16]) + int(sigma_0(words[i-15])) + int(words[i-7]) + int(sigma_1(words[i-2]))) % (2**64)
            words[i] = BitVector(intVal=val, size=64)

        # Compression function
        a, b, c, d, e, f, g, h = hb[0], hb[1], hb[2], hb[3] ,hb[4], hb[5], hb[6], hb[7]
        for i in xrange(80):
            t1 = T1(h, Ch(e, f, g), gamma_1(e), words[i], k512[i])
            t2 = T2(gamma_0(a), Maj(a, b, c))
            h = g
            g = f
            f = e
            e = BitVector(intVal= (int(d) + t1) % (2**64), size=64)
            d = c
            c = b
            b = a
            a = BitVector(intVal= (t1 + t2) % (2**64), size=64)

        # Intermediate hash values
        hb[0] = BitVector(intVal = (int(hb[0]) + int(a)) % (2**64), size=64)
        hb[1] = BitVector(intVal = (int(hb[1]) + int(b)) % (2**64), size=64)
        hb[2] = BitVector(intVal = (int(hb[2]) + int(c)) % (2**64), size=64)
        hb[3] = BitVector(intVal = (int(hb[3]) + int(d)) % (2**64), size=64)
        hb[4] = BitVector(intVal = (int(hb[4]) + int(e)) % (2**64), size=64)
        hb[5] = BitVector(intVal = (int(hb[5]) + int(f)) % (2**64), size=64)
        hb[6] = BitVector(intVal = (int(hb[6]) + int(g)) % (2**64), size=64)
        hb[7] = BitVector(intVal = (int(hb[7]) + int(h)) % (2**64), size=64)
    
    # Final Hash values
    msg_hash = hb[0] + hb[1] + hb[2]+ hb[3]+ hb[4] + hb[5] + hb[6] + hb[7]
    hex_str = msg_hash.getHexStringFromBitVector()
    return hex_str

def test(input, digest):
    import hashlib
    hlib_digest = hashlib.sha512(input).hexdigest()
    print "Hashlib output:", hlib_digest
    print "Awsmhsh output:", digest
    print "Equal?:", (digest == hlib_digest) # test

if __name__ == "__main__":
    input_filename = sys.argv[1]
    output_filename = "output.txt"
    fin = open(input_filename, 'r')
    fout = open(output_filename, 'w')
    input = fin.read()
    digest = sha512(input)
    test(input, digest)
    print digest
    fout.write(digest)
    fin.close()
    fout.close()
