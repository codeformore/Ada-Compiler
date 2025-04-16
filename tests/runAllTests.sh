#!/bin/bash
echo "=====BEGIN TESTING====="
echo "===TEST 1: testScanner==="
./bin/testScanner
if [ $? -ne 0 ]; then
    echo "===TEST 1 FAILED==="
    exit 1
fi
echo "===TEST 1 PASSED==="
echo "===TEST 2: testRDP==="
./bin/testRDP
if [ $? -ne 0 ]; then
    echo "===TEST 2 FAILED==="
    exit 1
fi
echo "===TEST 2 PASSED==="
echo "===TEST 3: testSymTbl==="
./bin/testSymTbl
if [ $? -ne 0 ]; then
    echo "===TEST 3 FAILED==="
    exit 1
fi
echo "===TEST 3 PASSED==="
echo "===TEST 4: testTACGen==="
./bin/testTACGen
if [ $? -ne 0 ]; then
    echo "===TEST 4 FAILED==="
    exit 1
fi
echo "===TEST 4 PASSED==="
echo "=====END TESTING====="