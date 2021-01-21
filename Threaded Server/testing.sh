#!/bin/bash
#   Make the necessary c files 
make 

# Scripted Test #1: Single instruction
echo 
echo "Single Instruction Test"
(sleep 2; echo quit) | ./dbserver 5000 || echo FAILED&

sleep 1
./dbtest --port 5000 --set='key0' helloworld
./dbtest --port 5000 --get='key0'
./dbtest --port 5000 --delete='key0'
./dbtest --port 5000 --set='key1' goworld
./dbtest --port 5000 --get='key1'
./dbtest --port 5000 --delete='key1'
wait
echo "SINGLE INSTR TEST: PASSED"
echo ""

# Scripted Test #2: Overload multithreaded
echo 
echo "Overload, 5 Threaded Test"
(sleep 3; echo quit) | ./dbserver 5002 || echo FAILED&

sleep 1
./dbtest --port 5002 --overload --threads 5
wait
echo "OVERLOAD MULTITHREAD TEST: PASSED"
echo ""

# Scripted Test #3: Overload multithreaded with Stats
echo 
echo "Overload, 5 Threaded Test 2 with Stats Before/After"
(sleep 1; echo stats; sleep 4; echo stats; echo quit) | ./dbserver 5004 || echo FAILED&

sleep 2
./dbtest --port 5004 --overload --threads 5
wait
echo "OVERLOAD MULTITHREAD TEST 2: PASSED"
echo ""

# Scripted Test #4: 10000 Requests multithreaded
echo 
echo "10000 Requests, 5 Threaded Test"
(sleep 3; echo quit) | ./dbserver 5006 || echo FAILED&

sleep 1
./dbtest --port 5006 --count 10000 --threads 5
wait
echo "10000 REQUESTS MULTITHREAD TEST: PASSED"
echo ""

# Scripted Test #5: 10000 Requests multithreaded with Stats
echo 
echo "10000 Requests, 5 Threaded Test 2 with Stats Before/After"
(sleep 1; echo stats; sleep 4; echo stats; echo quit) | ./dbserver 5008 || echo FAILED&

sleep 2
./dbtest --port 5008 --count 10000 --threads 5
wait
echo "10000 REQUESTS MULTITHREAD TEST 2: PASSED"
echo ""

