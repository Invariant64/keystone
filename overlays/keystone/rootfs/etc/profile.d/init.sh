#!/bin/sh
echo "==== Keystone custom init ===="
modprobe keystone-driver
/usr/share/keystone/examples/hello.ke