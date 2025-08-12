#!/bin/sh
echo "==== Keystone custom init ===="
modprobe keystone-driver
cd /usr/share/keystone/examples/