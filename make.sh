#!/bin/bash

clang++ -I Includes -std=c++17 Sources/* -o daemon
printf "done\n"
