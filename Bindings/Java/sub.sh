#!/bin/sh

LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:`pwd`
java -cp ./:./generated Subscriber
