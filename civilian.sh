#!/bin/bash
toolchain/civcc programs/$1.cvc
toolchain/civas $1.s -o civ.out
toolchain/civvm civ.out
