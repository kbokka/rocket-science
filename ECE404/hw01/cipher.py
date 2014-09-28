#!/usr/bin/env python

# Samodya Abeysiriwardane
# ECE 404 HW 01 Prob 1


import itertools
import re
import sys

letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
txt = raw_input("Please enter the Text: \n")
key = raw_input("Please enter the Key Phrase: \n")

# Clean the input
txt = re.sub(r'[^A-Za-z]+', '', txt)

# Makes the circular list from key
cycled_key =  itertools.cycle(key)

def encrypt(char):
    key_char = cycled_key.next()
    letters_index = (letters.index(char) + letters.index(key_char)) % 52
    return letters[letters_index]

# Encrypt letter by letter and join them
encrypted_text = ''.join(encrypt(c) for c in txt)

print "\nEncrypted Text:"
print encrypted_text
