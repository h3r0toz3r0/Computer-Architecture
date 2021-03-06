#!/bin/bash

# Note: This simulation prints to stderr
# Array of all benches to run
allBenches=('compress95' 'go' 'perl') # removed cc1 and anagram

# Set Associativity
allAssoc=(1 2 4)

# Iterate through each bench test
for bench in ${allBenches[@]};
do
    if [ $bench = "cc1" ]; then
        extra='-o /dev/null'                                
    elif [ $bench = 'compress95' ]; then
        extra='<simplesim-3.0/benchmarks/compress95.in'     
    elif [ $bench = 'go' ]; then 
        extra='50 9 simplesim-3.0/benchmarks/2stone9.in'    
    elif [ $bench = 'perl' ]; then
        extra='simplesim-3.0/benchmarks/perl-tests.pl'
    fi

    echo "Benchmark: " $bench

    # Title file
    file_name=results_$bench.csv

    # Add bench.alpha
    bench=$bench.alpha

    # Empty file
    > $file_name

    # Place headers
    { echo "title,cache_size,set_number,block_size,replacement_type,associativity,il1.miss_rate,il1.repl_rate,il2.miss_rate,il2.repl_rate,dl1.miss_rate,dl1.repl_rate,dl2.miss_rate,dl2.repl_rate"; } > $file_name
        
    for asso in ${allAssoc[@]};
    do
        echo "Associativity: " $asso

        # Replacement algorithm
        repl='f'

        # Base Case
        echo " "
        echo "Printing Base Case"
        echo "-------------------------"

        # Variables
        title=base_case
        cache_size=32
        set_number=1024
        bsize=32

        { printf "$title,$cache_size,$set_number,$bsize,$repl,$asso,"; } >> $file_name
        ./simplesim-3.0/sim-cache -cache:dl1 dl1:$set_number:$bsize:$asso:$repl -cache:dl2 dl2:$set_number:$bsize:$asso:$repl -cache:il1 il1:$set_number:$bsize:$asso:$repl -cache:il2 il2:$set_number:$bsize:$asso:$repl simplesim-3.0/benchmarks/$bench $extra 2> >( grep "il1.miss_rate\|il1.repl_rate\|il2.miss_rate\|il2.repl_rate\|dl1.miss_rate\|dl1.repl_rate\|dl2.miss_rate\|dl2.repl_rate" ) | awk "{print \$2}" | tr '\n' ',' | sed 's/,\?$/\n/' >> $file_name
 

        # Optimization: Larger Cache
        echo " "
        echo "Printing Optimization: Larger Cache"
        echo "-------------------------"

        # Variables
        title=cache_size
        set_number=512
        bsize=32

        for i in `seq 1 10`; 
        do
            set_number=$(($set_number<<1))
            cache_size=$(($(($set_number/1024))*32));
            { printf "$title,$cache_size,$set_number,$bsize,$repl,$asso,"; } >> $file_name
            ./simplesim-3.0/sim-cache -cache:dl1 dl1:$set_number:$bsize:$asso:$repl -cache:dl2 dl2:$set_number:$bsize:$asso:$repl -cache:il1 il1:$set_number:$bsize:$asso:$repl -cache:il2 il2:$set_number:$bsize:$asso:$repl simplesim-3.0/benchmarks/$bench $extra 2> >( grep "il1.miss_rate\|il1.repl_rate\|il2.miss_rate\|il2.repl_rate\|dl1.miss_rate\|dl1.repl_rate\|dl2.miss_rate\|dl2.repl_rate" ) | awk "{print \$2}" | tr '\n' ',' | sed 's/,\?$/\n/' >> $file_name
        done


        # Optimization: Larger Block Size
        echo " "
        echo "Printing Optimization: Larger Block Size"
        echo "-------------------------"

        # Variables
        title=block_size
        set_number=1024
        cache_size=32
        bsize=32

        for i in `seq 1 10`; 
        do
            set_number=$(($set_number>>1))
            block_size=$(($((32*1024))/$set_number));
            { printf "$title,$cache_size,$set_number,$bsize,$repl,$asso,"; } >> $file_name
            ./simplesim-3.0/sim-cache -cache:dl1 dl1:$set_number:$bsize:$asso:$repl -cache:dl2 dl2:$set_number:$bsize:$asso:$repl -cache:il1 il1:$set_number:$bsize:$asso:$repl -cache:il2 il2:$set_number:$bsize:$asso:$repl simplesim-3.0/benchmarks/$bench $extra 2> >( grep "il1.miss_rate\|il1.repl_rate\|il2.miss_rate\|il2.repl_rate\|dl1.miss_rate\|dl1.repl_rate\|dl2.miss_rate\|dl2.repl_rate" ) | awk "{print \$2}" | tr '\n' ',' | sed 's/,\?$/\n/' >> $file_name
        done


        # Replacement Algorithm Optimization
        # Replacement algorithm
        repl='f'

        # Base Case
        echo " "
        echo "Printing Replacement Algorithm Optimization: Random"
        echo "-------------------------"

        # Variables
        title=random_case
        cache_size=32
        set_number=1024
        bsize=32

        { printf "$title,$cache_size,$set_number,$bsize,$repl,$asso,"; } >> $file_name
        ./simplesim-3.0/sim-cache -cache:dl1 dl1:$set_number:$bsize:$asso:$repl -cache:dl2 dl2:$set_number:$bsize:$asso:$repl -cache:il1 il1:$set_number:$bsize:$asso:$repl -cache:il2 il2:$set_number:$bsize:$asso:$repl simplesim-3.0/benchmarks/$bench $extra 2> >( grep "il1.miss_rate\|il1.repl_rate\|il2.miss_rate\|il2.repl_rate\|dl1.miss_rate\|dl1.repl_rate\|dl2.miss_rate\|dl2.repl_rate" ) | awk "{print \$2}" | tr '\n' ',' | sed 's/,\?$/\n/' >> $file_name


        # Replacement Algorithm Optimization
        # Replacement algorithm
        repl='l'

        # Base Case
        echo " "
        echo "Printing Replacement Algorithm Optimization: LRU"
        echo "-------------------------"

        # Variables
        title=lru_case
        cache_size=32
        set_number=1024
        bsize=32

        { printf "$title,$cache_size,$set_number,$bsize,$repl,$asso,"; } >> $file_name
        ./simplesim-3.0/sim-cache -cache:dl1 dl1:$set_number:$bsize:$asso:$repl -cache:dl2 dl2:$set_number:$bsize:$asso:$repl -cache:il1 il1:$set_number:$bsize:$asso:$repl -cache:il2 il2:$set_number:$bsize:$asso:$repl simplesim-3.0/benchmarks/$bench $extra 2> >( grep "il1.miss_rate\|il1.repl_rate\|il2.miss_rate\|il2.repl_rate\|dl1.miss_rate\|dl1.repl_rate\|dl2.miss_rate\|dl2.repl_rate" ) | awk "{print \$2}" | tr '\n' ',' | sed 's/,\?$/\n/' >> $file_name
    done
done

# deubbing: ./simplesim-3.0/sim-cache -cache:dl1 dl1:1024:32:1:f -cache:dl2 ul2:1024:32:1:f -cache:il1 dl1:1024:32:1:f -cache:il2 dl2:1024:32:1:f simplesim-3.0/benchmarks/cc1.alpha -O simplesim-3.0/benchmarks/1stmt.i