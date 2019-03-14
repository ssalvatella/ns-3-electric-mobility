
for nodes in 100 200 400 600 800 1000 1200 1400 1600 1800 2000
do
    for i in {1..$1}
    do
        ./waf --run "mobility-antwerp --traceFile=$HOME/maps/benchmarks/Antwerp_$nodes.tcl --vehicleAttributes=$HOME/maps/benchmarks/Antwerp_EVS_$nodes.xml --nodeNum=$nodes --duration=60" | tee -a "benchmarks_results/$nodes.mobility.txt"
        ./waf --run "electric-mobility-antwerp --traceFile=$HOME/maps/benchmarks/Antwerp_$nodes.tcl --vehicleAttributes=$HOME/maps/benchmarks/Antwerp_EVS_$nodes.xml --nodeNum=$nodes --duration=60" | tee -a "benchmarks_results/$nodes.EVS.txt"
    done
done