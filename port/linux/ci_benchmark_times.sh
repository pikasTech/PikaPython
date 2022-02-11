cd Release && make -j
cd ..

RUN_TIMES=$1
touch performance_data.json
echo [] > performance_data.json

for ((i=1; i<=RUN_TIMES;i++))
do
    OUT_FILE=benchmark_result_$i.json
    # main
    bash ci_benchmark_run.sh $OUT_FILE > /dev/null && \
    python3 ci_benchmark_calibration.py $OUT_FILE
done
