#!/usr/bin/python

import sys
import os

maxlen = 8
#ALPHABET = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ *@#!$%^"
ALPHABET = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789*@#!$%^"

# Convert a number into a password
def i2p(num, alphabet=ALPHABET):
    if (num == 0):
        return alphabet[0]
    arr = []
    base = len(alphabet)
    while num:
        rem = num % base
        num = num // base
        arr.append(alphabet[rem])
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
def dump_ranges(minlen, maxlen, rangesize):
    pw1 = ( ALPHABET[1] * minlen )
    countr = p2i(pw1)
    while ( True ):
        countr = countr + rangesize
        pw2 = i2p(countr)
        if len(pw2) > maxlen:
            print "(%s) - (     END)" % pw1
            break
        print "(%8s) - (%8s)" % (pw1, pw2)
        pw1 = pw2

def dump_nums(minlen, maxlen, rangesize):
    pw1 = ( ALPHABET[1] * minlen )
    countr = p2i(pw1)
    stretch = 1
    while ( True ):
        if len(i2p(countr + rangesize)) > maxlen:
            print "%d:%d:%d:" % (stretch, countr, p2i(( ALPHABET[len(ALPHABET) - 1] * maxlen)))
            break
        print "%d:%d:%d:" % (stretch, countr, countr + rangesize)
        countr = countr + rangesize
        stretch = stretch + 1

iam = os.path.basename(sys.argv[0])

if iam == 'i2p':
    print i2p(int(sys.argv[1]))
elif iam == 'p2i':
    print p2i(sys.argv[1])
elif iam == 'pipr':
    i=int(sys.argv[1])
    end=int(sys.argv[2])
    while i <= end:
        print i2p(i)
        i = i +1
elif iam == 'dump_ranges':
    dump_ranges(1, 8, 2136000000)
else:
    dump_nums(1, 8, 2136000000)
