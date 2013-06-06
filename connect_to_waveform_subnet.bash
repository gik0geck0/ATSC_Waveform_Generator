#!/bin/bash

if [ -z "$!" ]
then
    "Usage: connect <interface>"
    exit
fi

ifconfig $1 169.254.5.22 netmask 255.255.0.0 up
