cd ../../

# Backup and clean up previous results
rm -r ../results/backup-result 
mkdir ../results/backup-result

cp -r ./out ../results/backup-result
cp -r ./utils/pods/gathered_data ../results/backup-result

zip -r ../results/backup-result.zip ../results/backup-result

rm -r ../results/latest-result
rm -r out
rm -r utils/pods/gathered_data

mkdir utils/pods/gathered_data

# Create new results 

START=$(date +%s%3N)
simbricks-run experiments/pyexps/pods/all_memory_configs.py --runs 10 --force --cores 24 --parallel
END=$(date +%s%3N)
echo "Time for all experiments was: $((END - START)) ms"

START=$(date +%s%3N)
simbricks-run experiments/pyexps/pods/scaling_memory_size.py --runs 10 --force --cores 24 --parallel
END=$(date +%s%3N)
echo "Time for all experiments was: $((END - START)) ms"

cd utils/pods

./gather_and_plot.sh

cd ../../


# Backup current results
mkdir ../results/latest-result

cp -r ./out ../results/latest-result
cp -r ./utils/pods/gathered_data ../results/latest-result

zip -r ../results/latest-result.zip ../results/latest-result

cd utils/pods

