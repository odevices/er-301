#!/bin/bash

ip link add name am335x type bridge
ip link set dev am335x up
ip addr add 192.168.4.1/24 dev am335x
dnsmasq --conf-file=./dnsmasq.conf