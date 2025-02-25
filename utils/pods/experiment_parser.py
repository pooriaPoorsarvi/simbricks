import os
import json
import pandas as pd
import re

# Path to the directory containing the JSON files
input_directory = "../../out"  # Adjust the directory path as needed
output_file = "gathered_data/results.csv"

# Initialize a list to store extracted data
data = []

# Function to parse the experiment name
def parse_experiment_name(exp_name):
    """
    Parse the experiment name to extract:
    - Far-off memory size (GB)
    - Read size (e.g., 1G or 512M)
    - Mode (read/write)
    """
    pattern = r"_(\d+)G_(\d+[MG])_(read|write)"
    match = re.search(pattern, exp_name)
    if match:
        far_off_memory_size = int(match.group(1))  # Extract far-off memory size in GB
        read_size = match.group(2)  # Extract read size (e.g., '1G', '512M')
        mode = match.group(3)  # Extract mode: 'read' or 'write'
        return far_off_memory_size, read_size, mode
    return None, None, None
# Function to extract "elapsed time" from the stdout array
def get_elapsed_time(stdout):
    for line in stdout:
        if "Elapsed time:" in line:
            return int(line.split("Elapsed time:")[1].split("ms")[0].strip())
    return None

# Loop through all JSON files in the directory
for filename in os.listdir(input_directory):
    if filename.endswith(".json") and filename != "summary.json":
        print(f"Processing {os.path.join(input_directory, filename)}...")
        filepath = os.path.join(input_directory, filename)
        
        # Open and load JSON file
        with open(filepath, 'r') as file:
            json_data = json.load(file)
            
            # Extract experiment name
            exp_name = json_data.get("exp_name", "")
            
            # Parse the experiment name
            far_off_memory_size, read_size, mode = parse_experiment_name(exp_name)
            
            # Access "stdout" to get elapsed time
            elapsed_time = None
            sims_data = json_data.get("sims", {})
            for sim in sims_data.values():
                stdout_logs = sim.get("stdout", [])
                elapsed_time = get_elapsed_time(stdout_logs)
            
            if elapsed_time is not None:
                # Append the extracted information
                data.append({
                    "Experiment Name": exp_name,
                    "Far-off Memory Size (GB)": far_off_memory_size,
                    "Read Size": read_size,
                    "Mode": mode,
                    "Elapsed Time (ms)": elapsed_time
                })
            else:
                assert False, f"Elapsed time not found for {exp_name}"

# Convert data to a Pandas DataFrame
df = pd.DataFrame(data)

# Save the DataFrame to a CSV file
df.to_csv(output_file, index=False)

# Display the DataFrame
print(df)