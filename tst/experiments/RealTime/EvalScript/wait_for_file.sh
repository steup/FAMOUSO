#!/bin/bash
touch .script.$2.waiting.for.$1
echo Waiting for file $1
while [ ! -f $1 ] ; do
    sleep 2s
done
echo Done
rm .script.$2.waiting.for.$1
