import pandas as pd
import matplotlib.pyplot as plt
from natsort import natsorted
import seaborn as sns

# Input CSV file
input_file = "gathered_data/results.csv"

# Load the data from the CSV file
df = pd.read_csv(input_file)


# Filter out experiments containing "scale"
df = df[~df['Experiment Name'].str.contains("scale")]


sort_order = [
    ['direct_access_memory_without_fast_path', 1],
    ['direct_access_memory', 0],
    ['configurable_pod_2_node', 2],
    ['configurable_pod_4_node', 3],
]

def name_mapping(exp_name):
    if 'direct_access_memory_without_fast_path' in exp_name:
        return 'QEMU slowed'
    elif 'direct_access_memory' in exp_name:
        return 'QEMU'
    elif 'configurable_pod_2_node' in exp_name:
        return '2 Nodes'
    elif 'configurable_pod_4_node' in exp_name:
        return '4 Nodes'
    return exp_name

def sort_key(exp_name):
    for name, order in sort_order:
        if name in exp_name:
            if 'read' in exp_name:
                return order * 2
            else:
                return (order * 2) + 1
    return len(sort_order)  # Place unmatched items at the end

def sort_df(df):
    
    df['sort_key'] = df['Experiment Name'].apply(lambda x: sort_key(x))
    df = df.sort_values(by='sort_key') # type: ignore

    return df


# Separate data into "read" and "write" modes
read_data = df[df["Mode"] == "read"]
write_data = df[df["Mode"] == "write"]
df = sort_df(df)
df["Final Name"] = df["Experiment Name"].apply(lambda x: name_mapping(x))


# Combined comparison of READ and WRITE for all configurations
# Do the same plots for read and write
plt.figure(figsize=(10, 6))
sns.barplot(data=df, x='Final Name', y='Elapsed Time (ms)', hue='Mode', palette=['red', 'blue'], hue_order=["read", "write"])

plt.title("Comparison of READ and WRITE Configurations")
plt.xlabel("Experiment")
plt.ylabel("Elapsed Time (ms)")
plt.xticks(rotation=45)
plt.legend(title="Mode")  # Add legend for combined plot
plt.tight_layout()
plt.savefig("gathered_data/combined_comparison.png")
plt.show()

