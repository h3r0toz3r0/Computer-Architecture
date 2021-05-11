#!/bin/bash

# configuration
reclen=1000 #64
sizes=( 4000000 ) #( 64 512 4096 32768)

# intro title
echo ""
echo "IOzone Benchmark"
echo ""

# docker build --build-arg SIZE=512 -t iozone . && docker run iozone > tmp.out && cat tmp.out | awk 'FNR == 29 {print $3}'

# remove csv files
rm results_iozone_docker.csv

# run 10 iterations of benchmark
echo ""
echo "running iterations..."
echo "size,reclen,write,read" > results_iozone_docker.csv
num=10
for j in  "${sizes[@]}"   
do
    # build container instance 
    docker build --build-arg SIZE=$j --build-arg RECLEN=$reclen -t iozone .

    for i in  $( seq 1 $num )
    do
        # run instance
        docker run iozone > tmp.out
        write=$(cat tmp.out | awk 'FNR == 30 {print $3}')
        read=$(cat tmp.out | awk 'FNR == 30 {print $5}')
        #rm tmp.out 

        echo "$j,$reclen,$write,$read" >> results_iozone_docker.csv

    done
done 

# done
echo ""
echo "exiting..."
echo ""