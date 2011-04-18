#!/bin/bash
killall Node1
killall Node2
killall Node3
killall Node4
killall RT_Master
killall sleep
killall run_node.sh
killall run_master.sh
killall wait_for_file.sh
rm -f .script*
cat /proc/xenomai/sched
