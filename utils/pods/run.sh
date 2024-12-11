cd ../../

rm -r out

START=$(date +%s%3N)

simbricks-run experiments/pyexps/pods/all_memory_configs.py --runs 1 --force 

END=$(date +%s%3N)

echo "Time for all experiments was: $((END - START)) ms"

cd utils/pods

python experiment_parser.py
python plot_experiment_result.py

