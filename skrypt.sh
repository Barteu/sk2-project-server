#!/bin/bash

gcc -Wall server.c structs.h init.c init.h fun_serv.c fun_serv.h -o serv

gcc -Wall client.c -o cli

gcc -Wall test.c -o test

