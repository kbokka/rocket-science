#! /usr/bin/env python
from BitVector import BitVector

RCON = [
    0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40,
    0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a,
    0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a,
    0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39,
    0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25,
    0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a,
    0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08,
    0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8,
    0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6,
    0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef,
    0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61,
    0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc,
    0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01,
    0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b,
    0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e,
    0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3,
    0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4,
    0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94,
    0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8,
    0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20,
    0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d,
    0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35,
    0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91,
    0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f,
    0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d,
    0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04,
    0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c,
    0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63,
    0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa,
    0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd,
    0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66,
    0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d]

SBOX = [
    [0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76],
    [0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0],
    [0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15],
    [0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75],
    [0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84],
    [0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf],
    [0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8],
    [0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2],
    [0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73],
    [0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb],
    [0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79],
    [0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08],
    [0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a],
    [0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e],
    [0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf],
    [0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16]
]

INVSBOX = [
    [0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb],
    [0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb],
    [0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e],
    [0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25],
    [0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92],
    [0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84],
    [0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06],
    [0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b],
    [0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73],
    [0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e],
    [0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b],
    [0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4],
    [0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f],
    [0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef],
    [0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61],
    [0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d]
]

plaintxt = "plaintext.txt"
encout = "encrypted.out"
enctxt = "encryptedtext.txt"
dectxt = "decryptedtext.txt"
default_key = "anunexaminedlifeisnotworthliving"

def encrypt(keyschedule):
    # place to store state array
    rstate = [[0 for i in xrange(0, 4)] for i in xrange(0, 4)] 

    bvfile = BitVector(filename=plaintxt)
    enctxtout = open(enctxt, "w")
    fout = open(encout, "wb")

    while (bvfile.more_to_read):
        bit_block = bvfile.read_bits_from_file(128)
        bit_block.pad_from_right(128 - len(bit_block))

        # init state array
        for i in xrange(0, 4):
            for j in xrange(0, 4):
                sp = (i * 32) + (j * 8)
                rstate[j][i] = bit_block[sp:sp + 8]

        add_round_key(rstate, keyschedule[0:4])

        for i in xrange(1, 14):
            sub_bytes(rstate)
            shift_rows(rstate)
            mix_cols(rstate)
            add_round_key(rstate, keyschedule[i*4:((i+1)*4)])

        sub_bytes(rstate)
        shift_rows(rstate)
        add_round_key(rstate, keyschedule[56:60])

        for i in xrange(0, 4):
            for j in xrange(0, 4):
                enctxtout.write(rstate[i][j].get_hex_string_from_bitvector())
                rstate[i][j].write_to_file(fout)

    fout.close()
    enctxtout.close()

def sub_word(word, sbox):
    newword = ''
    for i in xrange(0, 32, 8):
        [row, col] = word[i:i+8].divide_into_two()
        row = row.int_val()
        col = col.int_val()
        newword += str(BitVector(intVal=sbox[row][col]))

    return BitVector(bitstring=newword)

def sub_bytes(state):
    for i in xrange(0, 4):
        for j in xrange(0, 4):
            [row, col] = state[i][j].divide_into_two()
            row = row.int_val()
            col = col.int_val()
            return BitVector(intVal=SBOX[row][col])

def shift_rows(state):
    for i in xrange(1, 4): # to select row
        state[i] = state[i][i:4] + state[i][0:i]

def mix_cols(state):
    mod = BitVector(bitstring='100011011')
    bv2 = BitVector(hexstring='2')
    bv3 = BitVector(hexstring='3')
    for j in xrange(0, 4): # to select col
        s0 = state[0][j]
        s1 = state[1][j]
        s2 = state[2][j]
        s3 = state[3][j]

        sp0 = bv2.gf_multiply_modular(s0, mod, 8) ^ bv3.gf_multiply_modular(s1, mod, 8) ^ s2 ^ s3 
        sp1 = s0 ^ bv2.gf_multiply_modular(s1, mod, 8) ^ bv3.gf_multiply_modular(s2, mod, 8) ^ s3
        sp2 = s0 ^ s1 ^ bv2.gf_multiply_modular(s2, mod, 8) ^ bv3.gf_multiply_modular(s3, mod, 8)
        sp3 = bv3.gf_multiply_modular(s0, mod, 8) ^ s1 ^ s2 ^ bv2.gf_multiply_modular(s3, mod, 8)

        state[0][j] = sp0
        state[1][j] = sp1
        state[2][j] = sp2
        state[3][j] = sp3

def rot_word(word):
    return word[8:32] + word[0:8]

def add_round_key(state, word):
    for j in xrange(0, 4):
        state[0][j] ^= word[j][0:8]
        state[1][j] ^= word[j][8:16]
        state[2][j] ^= word[j][16:24]
        state[3][j] ^= word[j][24:32]

def key_expansion(key):
    w = [0 for i in xrange(0, 64)]
    for i in xrange(0, 8):
        w[i] = BitVector(textstring=key[(i*4):(i*4+4)])

    i = 8
    while i < 60:
        temp = w[i-1]
        if (i % 8 == 0):
            temp = sub_word((rot_word(temp)) ^ BitVector(intVal=RCON[i//8]), SBOX)
        elif (i % 8 == 4):
            temp = sub_word(temp, SBOX)
        w[i] = w[i-8] ^ temp
        i = i + 1

    return w

def decrypt(keyschedule):
    # place to store state array
    rstate = [[0 for i in xrange(0, 4)] for i in xrange(0, 4)] 

    bvfile = BitVector(filename=encout)
    fout = open(dectxt, "w")

    while (bvfile.more_to_read):
        bit_block = bvfile.read_bits_from_file(128)
        bit_block.pad_from_right(128 - len(bit_block))

        # init state array
        for i in xrange(0, 4):
            for j in xrange(0, 4):
                sp = (i * 32) + (j * 8)
                rstate[j][i] = bit_block[sp:sp + 8]

        add_round_key(rstate, keyschedule[56:60])

        for i in xrange(1, 14):
            invshift_rows(rstate)
            invsub_bytes(rstate)
            add_round_key(rstate, keyschedule[i*4:((i+1)*4)])
            invmix_cols(rstate)

        invshift_rows(rstate)
        invsub_bytes(rstate)
        add_round_key(rstate, keyschedule[0:4])

        for i in xrange(0, 4):
            for j in xrange(0, 4):
                rstate[i][j].write_to_file(fout)

    fout.close()


def invsub_bytes(state):
    for i in xrange(0, 4):
        for j in xrange(0, 4):
            [row, col] = state[i][j].divide_into_two()
            row = row.int_val()
            col = col.int_val()
            return BitVector(intVal=INVSBOX[row][col])

def invshift_rows(state):
    for i in xrange(1, 4): # to select row
        state[i] = state[i][-i:] + state[i][:-i]

def invmix_cols(state):
    mod = BitVector(bitstring='100011011')

    brw0 = [BitVector(hexstring='0e'),
            BitVector(hexstring='0b'),
            BitVector(hexstring='0d'),
            BitVector(hexstring='09')]

    brw1 = [brw0[3]] + brw0[1:4] 
    brw2 = [brw1[3]] + brw1[1:4] 
    brw3 = [brw2[3]] + brw2[1:4] 

    for j in xrange(0, 4): # to select col
        s =[state[0][j],
            state[1][j],
            state[2][j],
            state[3][j]]

        sp0 = reduce(lambda x, y: x^y, [brw0[i].gf_multiply_modular(s[i], mod, 8) for i in xrange(0, 4)])
        sp1 = reduce(lambda x, y: x^y, [brw1[i].gf_multiply_modular(s[i], mod, 8) for i in xrange(0, 4)])
        sp2 = reduce(lambda x, y: x^y, [brw2[i].gf_multiply_modular(s[i], mod, 8) for i in xrange(0, 4)])
        sp3 = reduce(lambda x, y: x^y, [brw3[i].gf_multiply_modular(s[i], mod, 8) for i in xrange(0, 4)])

        state[0][j] = sp0
        state[1][j] = sp1
        state[2][j] = sp2
        state[3][j] = sp3

def main():
    #key = raw_input("Please enter the key (or empty to use default key):\n")
    #if not key:
    key = default_key
    keyschedule = key_expansion(key)

    encrypt(keyschedule)
    decrypt(keyschedule)

if __name__ == "__main__":
    main()
