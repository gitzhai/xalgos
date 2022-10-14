# xalgos
data structures and algorithms implemented with C

header file foler :
    include

description to the whole library :
    include/xalgos.h

test file folder :
    test

test main function entry :
    test/test.c

make file usage :
    1. test the library
       a. change folder name xalgos to xalgos_bin
       b. make
       c. ./xalgos

    2. dynamic library
       a. delete test folder (or move it to other place)
       b. change folder name xalgos to xalgos_libso
       c. make
       d. libxalgos.so is created

    3. static library
       a. delete test folder (or move it to other place)
       b. change folder name xalgos to xalgos_liba
       c. make
       d. libxalgos.a is created
