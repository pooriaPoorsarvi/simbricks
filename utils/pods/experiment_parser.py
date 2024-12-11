import os
import json
import pandas as pd

# Path to the directory containing the JSON files
input_directory = "../../out"  # Adjust the directory path as needed
output_file = "results.csv"

# Initialize a list to store extracted data
data = []

# Function to extract "elapsed time" from the stdout array
def get_elapsed_time(stdout):
    for line in stdout:
        if "Elapsed time:" in line:
            return int(line.split("Elapsed time:")[1].split("ms")[0].strip())
    return None

# Loop through all JSON files in the directory
for filename in os.listdir(input_directory):
    if filename.endswith(".json"):
        if filename == "summary.json":
            continue
        print(f"Processing {os.path.join(input_directory, filename)}...")
        filepath = os.path.join(input_directory, filename)
        
        # Open and load JSON file
        with open(filepath, 'r') as file:
            json_data = json.load(file)
            
            # Extract experiment name
            exp_name = json_data.get("exp_name", "")
            
            # Determine mode (read/write) based on exp_name
            mode = "read" if "read" in exp_name else "write"
            
            # Access the "stdout" logs to find elapsed time
            sims_data = json_data.get("sims", {})
            for sim in sims_data.values():
                stdout_logs = sim.get("stdout", [])
                elapsed_time = get_elapsed_time(stdout_logs)
                if elapsed_time is not None:
                    # Append the extracted information
                    data.append({
                        "Experiment Name": exp_name.replace("_read", "").replace("_write", ""),
                        "Mode": mode,
                        "Elapsed Time (ms)": elapsed_time
                    })

# Convert data to a Pandas DataFrame
df = pd.DataFrame(data)

# Save the DataFrame to a CSV file
df.to_csv(output_file, index=False)

# Display the DataFrame
print(df)