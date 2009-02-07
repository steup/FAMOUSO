#!/bin/sh

LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:`pwd`
export LD_LIBRARY_PATH
java -cp ./:./generated Subscriber
