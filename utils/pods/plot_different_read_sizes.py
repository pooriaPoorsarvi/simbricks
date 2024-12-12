import pandas as pd
import matplotlib.pyplot as plt
from natsort import natsorted
import seaborn as sns

# Input CSV file
input_file = "gathered_data/results.csv"

# Load the data from the CSV file
df = pd.read_csv(input_file)

# Filter out experiments containing "scale"
df = df[df['Experiment Name'].str.contains("scale")]

df["opSize"] = df["Read Size"].apply(lambda x: int(x[:-1]))

# Create the main plot with mean lines
plt.figure(figsize=(10, 6))

# First create the mean lines
sns.lineplot(data=df, x="opSize", y="Elapsed Time (ms)", 
             hue="Mode", palette=['red','blue'], 
             hue_order=["read", "write"],
             ci=None)  # Remove confidence intervals

# Then add scatter points with some transparency
sns.scatterplot(data=df, x="opSize", y="Elapsed Time (ms)", 
                hue="Mode", palette=['red', 'blue'], 
                hue_order=["read", "write"],
                alpha=0.3,  # Make points semi-transparent
                legend=False)  # Don't create a second legend

plt.xlabel("Operation Size (MB)")
plt.ylabel("Elapsed Time (ms)")
plt.legend(title="Mode")

plt.savefig("gathered_data/rw_scale.png")