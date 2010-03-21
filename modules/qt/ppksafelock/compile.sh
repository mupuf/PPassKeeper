#!/bin/bash

g++ -g -o crypt `pkg-config --libs --cflags openssl` -DHAVE_STDINT_H crypt.cpp sha512.c