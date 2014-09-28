### ECE 404 Spring 2014 , HW 5 help script 
### This script will be used to test your RC4 encryption. 
### If you are coding in perl, an appropriate script will be used.
### Author - Bharath Kumar Comandur
###	    Purdue University
	    

### Note - You need to change the script appropriately to make it executable. 
### This is just to help you write your scripts in a suitable syntax


import wave 

from hw05 import *

# Your RC4 class should be named as RC4

rc4Cipher = RC4('VUp9UJKPjfBENCGi') 
originalp1 = wave.open('input.wav','r')
nframe_original = originalp1.getnframes()

# the following command reads and returns at most nframe_original 
# frames of audio, as a string of bytes.
originalAudio = originalp1.readframes(nframe_original)

# define your rc4Cipher.encrypt and decrypt functions appropriately
# obviously the encryptedAudio and decryptedAudio 
# will be also be strings of bytes , just like originalAudio

encryptedAudio = rc4Cipher.encrypt(originalAudio)
decryptedAudio = rc4Cipher.decrypt(encryptedAudio)

if originalAudio == decryptedAudio:
    print('RC4 is awesome')
else:
    print('Hmm, something seems fishy!')

originalp1.close()
