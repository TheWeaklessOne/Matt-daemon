#!/bin/bash

clang++ -I Includes -std=c++17 Sources/* -o Matt_daemon
printf "done\n"
