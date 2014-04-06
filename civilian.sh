#!/bin/bash
bin/civicc programs/$1.cvc -o progsrc/$1.s
toolchain/civas progsrc/$1.s -o civ.out
toolchain/civvm civ.out
