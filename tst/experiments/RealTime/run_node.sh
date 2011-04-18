#!/bin/bash
while [ ! -f ".script.stop.script" ] ; do
    touch ".script.ready.$1"
    EvalScript/wait_for_file.sh ".script.start.$1" $1
    TIME=`cat .script.start.$1`
    echo Running $1 for $TIME secs
    ./$1 &
    sleep $TIME
    echo Stopping $1
    killall $1
    touch ".script.stop.$1"
    EvalScript/wait_for_file.sh .script.nextround $1
done

