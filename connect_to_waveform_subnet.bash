#!/bin/bash

if [ -z "$1" ]
then
    echo "Usage: connect <interface>"
    exit
fi

ifconfig $1 169.254.5.22 netmask 255.255.0.0 up
