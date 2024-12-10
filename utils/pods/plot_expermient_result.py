import pandas as pd
import matplotlib.pyplot as plt

# Input CSV file
input_file = "results.csv"

# Load the data from the CSV file
df = pd.read_csv(input_file)

# Separate data into "read" and "write" modes
read_data = df[df["Mode"] == "read"]
write_data = df[df["Mode"] == "write"]

# Group data by Experiment Name and Elapsed Time for plotting
read_data = read_data.groupby("Experiment Name")["Elapsed Time (ms)"].mean().reset_index()
write_data = write_data.groupby("Experiment Name")["Elapsed Time (ms)"].mean().reset_index()

# Set up bar plot for READ mode
plt.figure(figsize=(10, 6))
plt.bar(read_data["Experiment Name"], read_data["Elapsed Time (ms)"], label="Read Mode", color="skyblue")
plt.title("Elapsed Time for READ Configurations")
plt.xlabel("Experiment Name")
plt.ylabel("Elapsed Time (ms)")
plt.xticks(rotation=45)
plt.legend()  # Add legend
plt.tight_layout()
plt.savefig("read_configurations.png")
plt.show()

# Set up bar plot for WRITE mode
plt.figure(figsize=(10, 6))
plt.bar(write_data["Experiment Name"], write_data["Elapsed Time (ms)"], label="Write Mode", color="lightcoral")
plt.title("Elapsed Time for WRITE Configurations")
plt.xlabel("Experiment Name")
plt.ylabel("Elapsed Time (ms)")
plt.xticks(rotation=45)
plt.legend()  # Add legend
plt.tight_layout()
plt.savefig("write_configurations.png")
plt.show()

# Combined comparison of READ and WRITE for all configurations
combined_data = df.groupby(["Experiment Name", "Mode"])["Elapsed Time (ms)"].mean().reset_index()
pivot_data = combined_data.pivot(index="Experiment Name", columns="Mode", values="Elapsed Time (ms)")

# Plot combined bar chart
pivot_data.plot(kind="bar", figsize=(10, 6), color={"read": "skyblue", "write": "lightcoral"})
plt.title("Comparison of READ and WRITE Configurations")
plt.xlabel("Experiment Name")
plt.ylabel("Elapsed Time (ms)")
plt.xticks(rotation=45)
plt.legend(title="Mode")  # Add legend for combined plot
plt.tight_layout()
plt.savefig("combined_comparison.png")
plt.show()
