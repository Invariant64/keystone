import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import pandas as pd
import os

def plot_stress_results(csv_dir, output_file, max_n=None):
    plt.figure(figsize=(12, 8))
    plt.xlabel('N')
    plt.ylabel('Time (s)')
    title_text = 'Stress Test Results'
    if max_n is not None:
        title_text += f' (N=0-{max_n} only)'
    plt.title(title_text)
    plt.grid(True)

    plot_data_dict = {}

    for filename in os.listdir(csv_dir):
        if filename.endswith('.csv'):
            filepath = os.path.join(csv_dir, filename)
            df = pd.read_csv(filepath)

            stress_type = filename.split('_')[0] # e.g., 'hdd' or 'cpu'
            program_name = df.iloc[0, 0] # First cell is the program name
            full_label = f'{stress_type.upper()}_{program_name}'

            n_values = []
            times = []
            for col_name in df.columns[1:]: 
                if 'N=' in col_name:
                    try:
                        n = int(col_name.split('=')[1].replace(' (Baseline)', '0'))
                        n_values.append(n)
                        time_val = df[col_name].iloc[0]
                        times.append(time_val)
                    except ValueError:
                        if col_name == 'N=0 (Baseline)':
                            n_values.append(0)
                            time_val = df[col_name].iloc[0]
                            times.append(time_val)
                        else:
                            print(f"Skipping column: {col_name} in {filename} due to unexpected format")
                            continue
            
            sorted_pairs = sorted(zip(n_values, times))
            n_values_sorted_filtered = []
            times_sorted_filtered = []
            for n, time_val in sorted_pairs:
                if max_n is None or n <= max_n:
                    n_values_sorted_filtered.append(n)
                    times_sorted_filtered.append(time_val)
            
            plot_data_dict[full_label] = (n_values_sorted_filtered, times_sorted_filtered)

    # Define the desired order for the legend
    desired_order = [
        'CPU_iozone.ke',
        'HDD_iozone.ke',
        'CPU_dhrystone.ke',
        'HDD_dhrystone.ke'
    ]

    for label in desired_order:
        if label in plot_data_dict:
            n_values, times = plot_data_dict[label]
            plt.plot(n_values, times, marker='o', label=label)

    plt.legend()
    plt.figtext(0.5, 0.01, 'N: Number of stress worker threads as configured in run_stress_tests.sh', ha='center', fontsize=10)
    plt.savefig(output_file)
    print(f"Plot saved to {output_file}")

if __name__ == '__main__':
    csv_directory = 'stress_results'
    
    # Full range plot
    output_image_file_full = 'stress_results/stress_test_results.png'
    plot_stress_results(csv_directory, output_image_file_full)