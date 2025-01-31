import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv('lifetimes_bank_addr.csv')

bin_size = 50
max_lifetime = data['Lifetime'].max()
bins = range(0, max_lifetime + bin_size, bin_size)

num_banks = data['Bank'].nunique()
fig, axes = plt.subplots(nrows=num_banks, ncols=1, figsize=(10, 5 * num_banks))
fig.subplots_adjust(hspace=0.5)

for bank, ax in zip(data['Bank'].unique(), axes.flatten()):
    bank_data = data[data['Bank'] == bank]
    ax.hist(bank_data['Lifetime'], bins=bins, alpha=0.7, edgecolor='black')
    ax.set_title(f'Lifetime Histogram for Bank {bank}')
    ax.set_xlabel(f'Lifetime (Grouped by {bin_size}ns)')
    ax.set_ylabel('Frequency')

output_file = 'histograms.png'
plt.savefig(output_file)
plt.close()

print(f"Histograms grouped by {bin_size}ns saved to {output_file}")