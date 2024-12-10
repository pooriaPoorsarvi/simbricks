cd ../../

rm -r out

simbricks-run experiments/pyexps/pods/all_memory_configs.py --runs 20 --force 

cd utils/pods

python experiment_parser.py
python plot_experiment_result.py

