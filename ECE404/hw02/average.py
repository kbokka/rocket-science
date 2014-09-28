#! /usr/bin/env python

# Author: Samodya Abeysiriwardane
# Description: DES Encryption Decryption
# usage: %s <input> 

import sys
from BitVector import BitVector

if len(sys.argv) < 2:
    print "usage: %s <input>" % sys.argv[0]
    sys.exit(1)

key = raw_input("Enter the key: ")
if len(key) < 8:
    print "Key needs to be at least 8 chars"
    sys.exit(1)

permute_1 = [56, 48, 40, 32, 24, 16, 8, 0, 57, 49, 41, 33, 25, 17, 9, 1, 58, 
            50, 42, 34, 26, 18, 10, 2, 59, 51, 43, 35, 62, 54, 46, 38, 30, 
            22, 14, 6, 61, 53, 45, 37, 29, 21, 13, 5, 60, 52, 44, 36, 28, 20,
            12, 4, 27, 19, 11, 3] 

permute_2 = [13, 16, 10, 23, 0, 4, 2, 27, 14, 5, 20, 9, 22, 18, 11, 3, 25, 7, 
            15, 6, 26, 19, 12, 1, 40, 51, 30, 36, 46, 54, 29, 39, 50, 44, 32, 
            47, 43, 48, 38, 55, 33, 52, 45, 41, 49, 35, 28, 31]

leftshifts = [1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1]

sboxes = [[[14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7], 
          [0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8], 
          [4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0], 
          [15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13]], 

         [[15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10], 
          [3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5], 
          [0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15], 
          [13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9]], 

         [[10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8], 
          [13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1], 
          [13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7], 
          [1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12]], 

         [[7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15], 
          [13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9], 
          [10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4], 
          [3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14]], 

         [[2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9], 
          [14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6], 
          [4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14], 
          [11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3]], 

         [[12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11], 
          [10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8], 
          [9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6], 
          [4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13]], 

         [[4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1], 
          [13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6], 
          [1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2], 
          [6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12]], 

         [[13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7], 
          [1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2], 
          [7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8], 
          [2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11]]]

pbox = [15, 6, 19, 20, 28, 11, 27, 16, 0, 14, 22, 25, 4, 17, 30, 
        9, 1, 7, 23, 13, 31, 26, 2, 8, 18, 12, 29, 5, 21, 10, 3, 24]

def crypt(inputf_name, key):
    allblocks = []
    bvfile = BitVector(filename=inputf_name)
    round_keys = [get_round_key(key, rnum) for rnum in xrange(0, 16)]
    while(bvfile.more_to_read):
        bit_block = bvfile.read_bits_from_file(64)
        
        # pad if bit_block size is less than 64 bits 
        bit_block.pad_from_right(64 - len(bit_block))

        # Perform 16 rounds 
        for round in xrange(0, 16):
            bit_block = crypt_round(bit_block, round_keys[round])

        LE, RE = bit_block.divide_into_two()
        bit_block = RE + LE
        allblocks.append(bit_block)

    bvfile.close_file_object()
    return allblocks

def crypt_modified(inputf_name, key, which_block, which_bit):
    # Modified crypt for test with plain txt changes
    # which_block: to toggle `which_bit` in which block

    allblocks = []
    bvfile = BitVector(filename=inputf_name)
    round_keys = [get_round_key(key, rnum) for rnum in xrange(0, 16)]

    block = 0 # Keep count of 64 Bit Blocks
    while(bvfile.more_to_read):
        bit_block = bvfile.read_bits_from_file(64)
        if block ==  which_block:
            bit_block[which_bit] = bit_block[which_bit] ^ 1
        # pad if bit_block size is less than 64 bits 
        bit_block.pad_from_right(64 - len(bit_block))

        # Perform 16 rounds 
        for round in xrange(0, 16):
            bit_block = crypt_round(bit_block, round_keys[round])

        LE, RE = bit_block.divide_into_two()
        bit_block = RE + LE
        allblocks.append(bit_block)
        block += 1

    bvfile.close_file_object()
    return allblocks

def crypt_round(bvblock, round_key):
    LE, RE = bvblock.divide_into_two()
    new_LE = RE.deep_copy() # L = R without changing
    new_RE = LE ^ feistal(RE, round_key) 
    return new_LE + new_RE

def feistal(bitblock, key):
    # Expansion Permutaion
    bits = []
    for i in xrange(0, 32, 4):
        bits.append(bitblock[i - 1])
        bits.extend(bitblock[i + j] for j in xrange(0, 4))
        bits.append(bitblock[(i + 4) % 32])
    bitblock = BitVector(bitlist=bits)

    # XOR with Round key 
    bitblock = bitblock ^ key

    # Sub with S-Box 
    bits = ''
    for sbnum in xrange(0, 8):
        word  = bitblock[(sbnum * 6):(sbnum * 6) + 6]
        row = BitVector(bitlist=[word[-1], word[0]]).intValue()
        col = word[1:-1].intValue()
        bits += str(BitVector(intVal=sboxes[sbnum][row][col], size=4))
    bitblock = BitVector(bitstring=bits)

    bitblock = bitblock.permute(pbox)
    return bitblock

def get_round_key(key, round):
    bitkey = key.permute(permute_1) # initial permutation
    
    # divide key into two parts and do circular left shift  
    lbitkey, rbitkey = bitkey.divide_into_two()
    rbitkey << leftshifts[round]
    lbitkey << leftshifts[round]
    bitkey = lbitkey + rbitkey 

    bitkey = bitkey.permute(permute_2) # contraction permutation
    return bitkey

def count_bit_changes(bvlist1, bvlist2):
    count = 0
    for i, bv1 in enumerate(bvlist1):
        bv2 = bvlist2[i]
        for j, b1 in enumerate(bv1):
            b2 = bv2[j]
            if b1 != b2:
                count += 1
    return count

def testw_keybits_changed(crypted):
    # crypted has BitVector lists from the original try 
    print "\nTest with Keybits Changed ----"
    bitchanges = [0, 0, 0, 0]
    print "Change MSB bit in key:"
    keybv = BitVector(textstring=key)
    keybv[0] = keybv[0] ^ 1 # Flip MSB Bit
    print "Testing with: ", keybv
    crypted_t = crypt(sys.argv[1], keybv)
    bitchanges[0] = count_bit_changes(crypted, crypted_t)
    print "%d bits changed in cipher text" % bitchanges[0] 

    print "\nChange 2nd LSB bit in key:"
    keybv = BitVector(textstring=key)
    keybv[-2] = keybv[-2] ^ 1 # Flip LSB Bit
    print "Testing with: ", keybv
    crypted_t = crypt(sys.argv[1], keybv)
    bitchanges[1] = count_bit_changes(crypted, crypted_t)
    print "%d bits changed in cipher text" % bitchanges[1] 

    print "\nChange 5th MSB in key:"
    keybv = BitVector(textstring=key)
    keybv[5] = keybv[5] ^ 1 # Flip Bit
    print "Testing with: ", keybv
    crypted_t = crypt(sys.argv[1], keybv)
    bitchanges[2] = count_bit_changes(crypted, crypted_t)
    print "%d bits changed in cipher text" % bitchanges[2] 

    print "\nChange 3rd MSB bit in key:"
    keybv = BitVector(textstring=key)
    keybv[3] = keybv[3] ^ 1 # Flip MSB Bit
    print "Testing with: ", keybv
    crypted_t = crypt(sys.argv[1], keybv)
    bitchanges[3] = count_bit_changes(crypted, crypted_t)
    print "%d bits changed in cipher text" % bitchanges[3] 

    avg =  sum(bitchanges) / float(len(bitchanges))
    print "\nAverage number of changes: ", avg
    
def testw_plaintxt_changed(crypted):
    # crypted has BitVector lists from the original try 
    print "\nTest with Plain txt bits Changed ----"
    print "with key: ", key
    bitchanges = [0, 0, 0]
    keybv = BitVector(textstring=key)

    print "Change 32nd bit in 3rd 64 block of text:"
    crypted_t = crypt_modified(sys.argv[1], keybv, 3, 32)
    bitchanges[0] = count_bit_changes(crypted, crypted_t)
    print "%d bits changed in cipher text" % bitchanges[0] 

    print "\nChange 10th bit in 5th 64 block of text:"
    crypted_t = crypt_modified(sys.argv[1], keybv, 5, 10)
    bitchanges[1] = count_bit_changes(crypted, crypted_t)
    print "%d bits changed in cipher text" % bitchanges[1] 

    print "\nChange 15th bit in 9th 64 block of text:"
    crypted_t = crypt_modified(sys.argv[1], keybv, 9, 15)
    bitchanges[2] = count_bit_changes(crypted, crypted_t)
    print "%d bits changed in cipher text" % bitchanges[2] 

    avg =  sum(bitchanges) / float(len(bitchanges))
    print "\nAverage number of changes: ", avg
    
def main():
    bitkey = BitVector(textstring=key)
    print "Testing with: ", BitVector(textstring=key)
    crypted = crypt(sys.argv[1], bitkey)

    testw_keybits_changed(crypted) # test and print out stats when bits in key changes
    testw_plaintxt_changed(crypted)
    sys.exit(0)

if __name__ == "__main__":
    main()
