#!/bin/bash
TIME=6h
#TIME=10m
#mv config.h config.h.backup
CONFIGS=`find EvalScript/* -type d`
echo Configs: $CONFIGS
for EVAL_CONFIG in $CONFIGS ; do
    rm -f .script.start.*
    rm -f .script.stop.*
    EvalScript/wait_for_file.sh ".script.ready.Node1" RT_Master
    EvalScript/wait_for_file.sh ".script.ready.Node2" RT_Master
    EvalScript/wait_for_file.sh ".script.ready.Node3" RT_Master
    EvalScript/wait_for_file.sh ".script.ready.Node4" RT_Master
    rm -f .script.ready.*
    rm -f .script.nextround

    echo "== Config $EVAL_CONFIG ========================================"
    EVAL_DIR=$EVAL_CONFIG

    # BUILD CONFIG
    echo Building...
    echo cp $EVAL_DIR/config.h .
    cp $EVAL_DIR/config.h .
    make clean
    make eval || exit 1

    # RUN
    echo Running RT_Master and other nodes for ca. $TIME...
    ./RT_Master &
    sleep 1s
    echo $TIME > ".script.start.Node1"
    echo $TIME > ".script.start.Node2"
    sleep 2m
    echo $TIME > ".script.start.Node3"
    echo $TIME > ".script.start.Node4"

    echo Wait for other nodes
    EvalScript/wait_for_file.sh ".script.stop.Node1" RT_Master
    EvalScript/wait_for_file.sh ".script.stop.Node2" RT_Master
    EvalScript/wait_for_file.sh ".script.stop.Node3" RT_Master
    EvalScript/wait_for_file.sh ".script.stop.Node4" RT_Master
    echo Stopping RT_Master
    killall RT_Master
    touch .script.nextround

    # SAVE OUTPUT
    mv log* lat* $EVAL_DIR
done
rm -f .script.start.*
rm -f .script.stop.*
touch ".script.stop.script"
#mv config.h.backup config.h

