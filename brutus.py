#!/usr/bin/python

import sys
import os


#ALPHABET = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ *@#!$%^"
ALPHABET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789*@#!$%^"

# Convert a number into a password
def i2p(num, maxlen, alphabet=ALPHABET):
    arr = []
    base = len(alphabet)
    while num:
        rem = num % base
        num = num // base
        arr.append(alphabet[rem])
    if len(arr) > maxlen:
        return pass_max
    while len(arr) < maxlen:
        arr.append(alphabet[0])
    arr.reverse()
    return ''.join(arr)

# Convert a password into a number
def p2i(string, alphabet=ALPHABET):
    base = len(alphabet)
    strlen = len(string)
    num = 0
    idx = 0
    for char in string:
        power = (strlen - (idx + 1))
        num += alphabet.index(char) * (base ** power)
        idx += 1
    return num

# Print ranges in (startplain) - (endplain) format
def dump_ranges(maxlen, rangesize):
    pw1 = ( ALPHABET[0] * maxlen )
    countr = p2i(pw1)
    while ( True ):
        countr = countr + rangesize
        pw2 = i2p(countr, maxlen)
        print "(%8s) - (%8s)" % (pw1, pw2)
        if pw2 == pass_max:
                break
        pw1 = pw2

def dump_nums(maxlen, rangesize):
    pw1 = ( ALPHABET[0] * maxlen )
    countr = p2i(pw1)
    stretch = 1
    while ( True ):
        if i2p((countr + rangesize), maxlen) == pass_max:
            print "%d:%d:%d:" % (stretch, countr, p2i(( ALPHABET[len(ALPHABET) - 1] * maxlen)))
            break
        print "%d:%d:%d:" % (stretch, countr, countr + rangesize)
        countr = countr + rangesize
        stretch = stretch + 1

iam = os.path.basename(sys.argv[0])


pass_max = ( ALPHABET[len(ALPHABET)-1] * 8 )
if iam == 'i2p':
    print i2p(int(sys.argv[2]), int(sys.argv[1]))
elif iam == 'p2i':
    print p2i(sys.argv[1])
elif iam == 'pipr':
    i=int(sys.argv[1])
    end=int(sys.argv[2])
    while i <= end:
        print i2p(i)
        i = i +1
elif iam == 'dump_ranges':
    dump_ranges(8, 2136000000)
else:
    dump_nums(8, 2136000000)
