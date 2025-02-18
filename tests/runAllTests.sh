#!/bin/bash
echo "=====BEGIN TESTING====="
echo "===TEST 1: testScanner==="
./bin/testScanner
if [ $? -ne 0 ]; then
    echo "===TEST 1 FAILED==="
    exit 1
else
    echo "===TEST 1 PASSED==="
    echo "===TEST 2: testRDP==="
    ./bin/testRDP
    if [ $? -ne 0 ]; then
        echo "===TEST 2 FAILED==="
        exit 1
    else
        echo "===TEST 2 PASSED==="
    fi
fi
echo "=====END TESTING====="