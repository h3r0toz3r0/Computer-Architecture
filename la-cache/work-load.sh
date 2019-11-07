#!/bin/bash

## Study effects of prefetching policies on dineroIV

# Architecture Variables
split_cache_size='8K'
block_size='64'
cache_level='1'
assoco='4'

## Architecture general setup
## dineroIV -l${cache_level}-isize ${split_cache_size} -l${cache_level}-ibsize ${block_size} -l${cache_level}-dsize ${split_cache_size} -l${cache_level}-dbsize ${block_size} -l${cache_level}-iassoc ${assoco} -l${cache_level}-dassoc ${assoco} -l${cache_level}-ifetch $switch -l${cache_level}-dfetch $switch -informat d < trace | grep 'Demand miss rate' | awk '{print $4}'

## Option Key:
## C single character
## U unsigned decimal integer
## N cache level (1 <= N <= 5)
## T cache type (u=unified, i=instruction, d=data)

# Add variable switches
# -lN-Tfetch C      Fetch policy (d=demand, a=always, m=miss, t=tagged, l=load forward, s=subblock) (default d)
switch_array=( d a m t l s )
for switch in ${switch_array[@]}
do
    echo -l${cache_level}-ifetch $switch -l${cache_level}-dfetch $switch
    dineroIV -l${cache_level}-isize ${split_cache_size} -l${cache_level}-ibsize ${block_size} -l${cache_level}-dsize ${split_cache_size} -l${cache_level}-dbsize ${block_size} -l${cache_level}-iassoc ${assoco} -l${cache_level}-dassoc ${assoco} -l${cache_level}-ifetch $switch -l${cache_level}-dfetch $switch -informat d < trace | grep 'Demand miss rate' | awk '{print $4}'
done

# -lN-Tpfdist U     Prefetch distance (in sub-blocks) (default 1)
# Note: switch will NOT work for "d" fetch policy, so check with all other fetch policies
for ((switch=0;switch<=100;switch+=10)); 
do 
    switch_array2=( a m t )
    for switch2 in ${switch_array2[@]}
    do
        echo -l${cache_level}-ifetch $switch2 -l${cache_level}-dfetch $switch2 -l${cache_level}-ipfdist $switch -l${cache_level}-dpfdist $switch
        dineroIV -l${cache_level}-isize ${split_cache_size} -l${cache_level}-ibsize ${block_size} -l${cache_level}-dsize ${split_cache_size} -l${cache_level}-dbsize ${block_size} -l${cache_level}-iassoc ${assoco} -l${cache_level}-dassoc ${assoco} -l${cache_level}-ifetch $switch2 -l${cache_level}-dfetch $switch2 -l${cache_level}-ipfdist $switch -l${cache_level}-dpfdist $switch -informat d < trace | grep 'Demand miss rate' | awk '{print $4}'
    done
done

# -lN-Tpfabort U    Prefetch abort percentage (0-100) (default 0)
# Note: switch will NOT work for "d" fetch policy, so check with all other fetch policies
for ((i=0;i<=100;i+=10)); 
do 
    switch_array2=( a m t l s )
    for switch2 in ${switch_array2[@]}
    do
        echo -l${cache_level}-ifetch $switch2 -l${cache_level}-dfetch $switch2 -l${cache_level}-ipfabort $i -l${cache_level}-dpfabort $i
        dineroIV -l${cache_level}-isize ${split_cache_size} -l${cache_level}-ibsize ${block_size} -l${cache_level}-dsize ${split_cache_size} -l${cache_level}-dbsize ${block_size} -l${cache_level}-iassoc ${assoco} -l${cache_level}-dassoc ${assoco} -l${cache_level}-ifetch $switch2 -l${cache_level}-dfetch $switch2 -l${cache_level}-ipfabort $i -l${cache_level}-dpfabort $i -informat d < trace | grep 'Demand miss rate' | awk '{print $4}'
    done
done
