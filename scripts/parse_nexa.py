#!/usr/bin/python

#
# Parse a .csv containg the RF signal as exported by the saleae Logic analyzer.
#
# - Note! This is not intended as a "robust" parser, it's more a verifier
#   to make sure I've understood the protocol.
#

import csv
import sys
import os

class Sample(object):
    def __init__(self, time, val):
        self.time = float(time)
        self.val = int(val)

    def __repr__(self):
        return "%f %d" % (self.time, self.val)

HIGH=1
LOW=0

def parse_csv(fname):
    csvf = file(fname)
    reader = csv.reader(csvf)
    x = reader.next() # Skip header
    prev = None
    num_T = 0
    seq = []
    for a in reader:
        sample = Sample(a[0], a[1])

        # Fake 10ms pre pause for consistency...
        if prev == None:
            prev = Sample("%f"%(float(a[0])-0.01), "0")
            first = prev

        length_us = (sample.time - prev.time)*1000000
        Tus = 262
        length_T = int(round(length_us / Tus))
        dev_us = (length_us - length_T*Tus)
#        print "%dT % 4s (deviation % 3dus %3.2f%%)" % (length_T, "LOW" if sample.val else "HIGH", dev_us, 100.*dev_us/Tus)
        num_T += length_T
        prev = sample
        seq.append((length_T, LOW if sample.val else HIGH))

    return seq

#States
SCAN=0
ID=1
def parse_bit_single(seq, index):
    s0 = seq[index]
    s1 = seq[index+1]
    if s0 == (1, HIGH) and s1 == (1, LOW):
        return 1
    if s0 == (1, HIGH) and s1 == (5, LOW):
        return 0
    raise Exception("PARSE BIT i:%d %d:%d %d:%d" % (index,s0[0],s0[1],s1[0],s1[1]))

def parse_bit(seq, index):
    b0 = parse_bit_single(seq, index)
    b1 = parse_bit_single(seq, index + 2)
    if b0 == b1:
        raise Exception("%d==%d" % (b0, b1))
    return b0

def parse_bits(seq, index, bits):
    ret = 0
    for j in range(bits):
        ret = ret << 1
        ret |= parse_bit(seq, index)
        index += 4
    return (ret, index)

def parse_nexa(seq):
    state = SCAN
    i = 0
    skip = 0
    while i < len(seq) - 1:
        if state == SCAN:
            print "SCAN: %d (%d:%s)" % (i, seq[i][0], "HIGH" if seq[i][1] == HIGH else "LOW")
            if seq[i] == (1, HIGH) and seq[i+1] == (10, LOW):
                print "SYNC @ %d skip:%d" % (i, skip)
                state = ID
                i += 2
                continue
            i += 1
            skip += 1
            continue

        if state == ID:
            (trans_id, i) = parse_bits(seq, i, 26)
            (group, i)    = parse_bits(seq, i, 1)
            (onoff, i)    = parse_bits(seq, i, 1)
            (unit, i)     = parse_bits(seq, i, 2)
            (button, i)   = parse_bits(seq, i, 2)
            # FIXME: Parse single bit 1 + 40T LOW here
            print "DONE: % -20s id:0x%x g:%d o:%d u:%d b:%d" % (os.path.basename(sys.argv[1]), trans_id, group, onoff, unit, button)
            state = SCAN
            continue

        raise Exception("UNKNOWN STATE %d" % state)

if __name__ == "__main__":
    seq = parse_csv(sys.argv[1])
    parse_nexa(seq)
