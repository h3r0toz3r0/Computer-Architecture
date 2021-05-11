#!/bin/bash

# intro title
echo ""
echo "Linpack Benchmark"

# build image
echo ""
echo "building executable..."
gcc -O3 linpack.c -o linpack

# remove previous csv
rm vm_average_linpack_output.csv
rm vm_linpack_output.csv

# run 10 iterations of benchmark
echo ""
echo "running iterations..."
echo "norm_resid,resid,machep,x1,xN,factor,solve,total,mflops,unit,cray" > results_linpack_vm.csv
num=10             
for i in $( seq 1 $num )    
do
    echo "  ... iteration: $i"
    # run program
    ./linpack > run.txt

    # get statistics
    a=$(cat run.txt | awk 'FNR == 14 {print $1}')
    b=$(cat run.txt | awk 'FNR == 14 {print $2}')
    c=$(cat run.txt | awk 'FNR == 14 {print $3}')
    d=$(cat run.txt | awk 'FNR == 14 {print $4}')
    e=$(cat run.txt | awk 'FNR == 14 {print $5}')
    f=$(cat run.txt | awk 'FNR == 18 {print $1}')
    g=$(cat run.txt | awk 'FNR == 18 {print $2}')
    h=$(cat run.txt | awk 'FNR == 18 {print $3}')
    i=$(cat run.txt | awk 'FNR == 18 {print $4}')
    j=$(cat run.txt | awk 'FNR == 18 {print $5}')
    k=$(cat run.txt | awk 'FNR == 18 {print $6}')

    # add values to respective csv file
    echo $a >> linpack_results_normresid.csv
    echo $b >> linpack_results_resid.csv
    echo $c >> linpack_results_machep.csv
    echo $d >> linpack_results_x1.csv
    echo $e >> linpack_results_xn.csv
    echo $f >> linpack_results_factor.csv
    echo $g >> linpack_results_solve.csv
    echo $h >> linpack_results_total.csv
    echo $i >> linpack_results_mflops.csv
    echo $j >> linpack_results_unit.csv
    echo $k >> linpack_results_cray.csv

	echo "$a,$b,$c,$d,$e,$f,$g,$h,$i,$j,$k" >> results_linpack_vm.csv
	
    # remove run file
    rm run.txt
done

# print csv results
echo ""
echo "benchmarks completed..."

# average results
total=0
norm_resid=$(awk -F' ' ' { total += $1 } END {print total/NR}' linpack_results_normresid.csv)
total=0
resid=$(awk -F' ' ' { total += $1 } END {print total/NR}' linpack_results_resid.csv)
total=0
machep=$(awk -F' ' ' { total += $1 } END {print total/NR}' linpack_results_machep.csv)
total=0
x1=$(awk -F' ' ' { total += $1 } END {print total/NR}' linpack_results_x1.csv)
total=0
xn=$(awk -F' ' ' { total += $1 } END {print total/NR}' linpack_results_xn.csv)
total=0
factor=$(awk -F' ' ' { total += $1 } END {print total/NR}' linpack_results_factor.csv)
total=0
solve=$(awk -F' ' ' { total += $1 } END {print total/NR}' linpack_results_solve.csv)
total=0
total=$(awk -F' ' ' { total += $1 } END {print total/NR}' linpack_results_total.csv)
total=0
mflops=$(awk -F' ' ' { total += $1 } END {print total/NR}' linpack_results_mflops.csv)
total=0
unit=$(awk -F' ' ' { total += $1 } END {print total/NR}' linpack_results_unit.csv)
total=0
cray=$(awk -F' ' ' { total += $1 } END {print total/NR}' linpack_results_cray.csv)

# print final averages
echo ""
echo "Averaged Results After $num Iterations"
echo "  Norm. Resid:        $norm_resid"
echo "  Resid:              $resid"
echo "  MACHEP:             $machep"
echo "  X[1]:               $x1"
echo "  X[N]:               $xn"
echo "  Factor:             $factor"
echo "  Solve:              $solve"
echo "  Total:              $total"
echo "  MFLOPS:             $mflops"
echo "  Unit:               $unit"
echo "  Cray-Ratio:         $cray"

# remove files
echo ""
echo "removing unnecessary files..."
rm -rf linpack_result*
rm linpack 

# done
echo ""
echo "exiting..."
echo ""