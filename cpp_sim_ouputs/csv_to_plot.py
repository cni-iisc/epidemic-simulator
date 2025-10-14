#!/usr/bin/env python3
"""
Enhanced Epidemic City Data Analyzer - JavaScript Style + Comparative Analysis
Mirrors the plotting strategies from sim.js for consistent visualization
Works with the output_modified directory structure containing multiple city simulations
Includes both individual analysis (original functionality) and comparative analysis (0-100% scale)
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os
import re
from collections import defaultdict
import argparse
from matplotlib.patches import Rectangle
import matplotlib.patches as mpatches

def setup_plot_style():
    """Configure matplotlib to match JavaScript styling"""
    plt.rcParams.update({
        'figure.figsize': (16, 10),
        'font.size': 12,
        'axes.titlesize': 16,
        'axes.labelsize': 14,
        'xtick.labelsize': 12,
        'ytick.labelsize': 12,
        'legend.fontsize': 12,
        'figure.titlesize': 18,
        'axes.grid': True,
        'grid.alpha': 0.3,
        'grid.linestyle': '-',
        'grid.linewidth': 0.5,
        'lines.linewidth': 3,
        'lines.markersize': 6,
        'axes.spines.top': False,
        'axes.spines.right': False,
        'figure.facecolor': 'white',
        'axes.facecolor': '#fafafa',
        'axes.edgecolor': '#666666',
        'xtick.color': '#333333',
        'ytick.color': '#333333',
        'text.color': '#333333'
    })

def discover_city_directories(base_dir="/data/tarunks/epidemic-simulator-master/output_modified"):
    """Discover all available city simulation directories - Universal for any number of cities"""
    
    print(f"Scanning directory: {base_dir}")
    
    if not os.path.exists(base_dir):
        print(f"Error: Directory {base_dir} does not exist!")
        return []
    
    # Find all subdirectories that contain simulation data
    city_dirs = []
    print("Discovering city directories...")
    
    for item in os.listdir(base_dir):
        item_path = os.path.join(base_dir, item)
        
        # Skip output directories and hidden folders
        if (os.path.isdir(item_path) and 
            not item.startswith('comparative_') and 
            not item.startswith('individual_') and
            not item.startswith('output_images_') and
            not item.startswith('.')):
            
            # Check if it contains CSV files (simulation data)
            csv_files = [f for f in os.listdir(item_path) if f.endswith('.csv')]
            
            if csv_files:
                city_info = parse_directory_name(item)
                city_info['path'] = item_path
                city_info['csv_count'] = len(csv_files)
                city_dirs.append(city_info)
                print(f"  ✓ Found: {city_info['full_name']} ({len(csv_files)} CSV files)")
    
    # Sort cities by population for better organization
    city_dirs.sort(key=lambda x: x['population_num'])
    
    print(f"\nTotal cities discovered: {len(city_dirs)}")
    return city_dirs

def parse_directory_name(dir_name):
    """Parse directory name to extract city and population info - Universal parser"""
    
    # Extract city name (before the first underscore or number)
    parts = dir_name.split('_')
    
    # Handle various naming conventions
    if len(parts) >= 1:
        city = parts[0].title()
    else:
        city = dir_name.title()
    
    # Extract population with flexible parsing
    population = 'Unknown'
    population_num = 0
    
    # Look for population indicators in all parts
    for part in parts[1:]:
        if any(char.isdigit() for char in part):
            # Handle different formats: 100k, 1000k, 12400k, 1M, 500000, etc.
            part_lower = part.lower()
            
            try:
                if 'k' in part_lower:
                    pop_num = float(part_lower.replace('k', ''))
                    if pop_num >= 1000:
                        population = f"{pop_num/1000:.1f}M"
                        population_num = int(pop_num * 1000)
                    else:
                        population = f"{int(pop_num)}K"
                        population_num = int(pop_num * 1000)
                        
                elif 'm' in part_lower:
                    pop_num = float(part_lower.replace('m', ''))
                    population = f"{pop_num:.1f}M"
                    population_num = int(pop_num * 1000000)
                    
                elif part.isdigit():
                    # Direct number (assume it's in thousands or actual population)
                    pop_num = int(part)
                    if pop_num > 10000:  # Likely actual population
                        if pop_num >= 1000000:
                            population = f"{pop_num/1000000:.1f}M"
                        else:
                            population = f"{pop_num/1000:.0f}K"
                        population_num = pop_num
                    else:  # Likely in thousands
                        if pop_num >= 1000:
                            population = f"{pop_num/1000:.1f}M"
                            population_num = pop_num * 1000
                        else:
                            population = f"{pop_num}K"
                            population_num = pop_num * 1000
                
                break  # Use first valid population found
                
            except (ValueError, IndexError):
                continue  # Try next part
    
    return {
        'city': city,
        'population': population,
        'population_num': population_num,
        'dir_name': dir_name,
        'full_name': f"{city} ({population})"
    }

def select_city_directory(city_dirs):
    """Allow user to select which city directory to analyze - Universal for any number of cities"""
    
    if not city_dirs:
        print("No city directories with CSV files found!")
        return None
    
    print(f"\nFound {len(city_dirs)} city simulation directories:")
    print("="*70)
    
    # Display all cities dynamically
    for i, city_info in enumerate(city_dirs):
        print(f"{i+1:3d}. {city_info['full_name']:<25} ({city_info['csv_count']} CSV files)")
    
    print("="*70)
    print(f"{len(city_dirs)+1:3d}. Analyze ALL cities (Individual)")
    print(f"{len(city_dirs)+2:3d}. Create COMPARATIVE analysis (0-100% scale)")
    
    while True:
        try:
            choice = input(f"\nSelect option (1-{len(city_dirs)+2}): ").strip()
            
            if choice == str(len(city_dirs)+1):
                return 'ALL'
            elif choice == str(len(city_dirs)+2):
                return 'COMPARATIVE'
            
            choice_num = int(choice)
            if 1 <= choice_num <= len(city_dirs):
                return city_dirs[choice_num - 1]
            else:
                print(f"Invalid choice. Please enter a number between 1 and {len(city_dirs)+2}.")
        
        except (ValueError, KeyboardInterrupt):
            print("Invalid input. Please enter a number.")
            if input("Press Enter to continue or 'q' to quit: ").lower() == 'q':
                return None

def setup_output_directory(city_info):
    """Create output directory for images"""
    if city_info == 'ALL':
        # Store individual results in separate folder at base path
        base_path = "/data/tarunks/epidemic-simulator-master/output_modified"
        output_dir = os.path.join(base_path, 'individual_analysis_all_cities')
    elif city_info == 'COMPARATIVE':
        base_path = "/data/tarunks/epidemic-simulator-master/output_modified"
        output_dir = os.path.join(base_path, 'comparative_analysis_all_cities')
    else:
        safe_name = city_info['dir_name'].replace(' ', '_').lower()
        output_dir = f'output_images_{safe_name}'
    
    # Create the directory
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
        print(f"Created directory: {output_dir}")
    else:
        print(f"Using existing directory: {output_dir}")
    
    return os.path.abspath(output_dir)

def get_colorblind_friendly_palette():
    """Enhanced colorblind-friendly palette - scientifically tested for all types of color vision deficiency"""
    # Based on IBM's accessible color palette and Wong's Nature recommendations
    # Tested for Protanopia, Deuteranopia, Tritanopia, and Monochromacy
    return {
        'primary_blue': '#648FFF',      # IBM Blue 60 - excellent for all CVD types
        'orange': '#FE6100',            # Vivid orange - high contrast, CVD safe
        'green': '#00B4A6',             # Teal - distinguishable from red for red-green CVD
        'red': '#DC267F',               # Magenta-pink - safe alternative to pure red
        'purple': '#785EF0',            # Purple-blue - distinct from other colors
        'brown': '#FFB000',             # Amber yellow - high visibility
        'pink': '#A7A7A7',              # Medium gray - neutral reference
        'gray': '#1F77B4',              # Standard blue - backup
        'olive': '#7F7F7F',             # Dark gray - high contrast
        'cyan': '#17BECF'               # Light cyan - additional option
    }

def get_enhanced_colorblind_palette():
    """Extended palette with more color options for complex plots"""
    # Paul Tol's vibrant scheme + Cynthia Brewer's ColorBrewer + IBM accessibility colors
    return [
        '#0173B2',  # Strong blue - universally safe
        '#DE8F05',  # Orange - red-green CVD safe  
        '#029E73',  # Teal green - distinct from red
        '#CC78BC',  # Light magenta - safe red alternative
        '#CA9161',  # Brown - high contrast
        '#949494',  # Gray - neutral
        '#56B4E9',  # Sky blue - distinct from primary blue
        '#F0E442',  # Yellow - maximum visibility
        '#009E73',  # Dark teal - backup green
        '#E69F00',  # Dark orange - backup orange
        '#999999',  # Light gray - additional neutral
        '#D55E00'   # Vermillion - final option
    ]

def assign_colors_safely(num_needed):
    """Assign colors ensuring maximum distinction for colorblind users"""
    base_palette = get_enhanced_colorblind_palette()
    
    if num_needed <= len(base_palette):
        return base_palette[:num_needed]
    else:
        # If we need more colors, cycle through with different alphas
        colors = []
        for i in range(num_needed):
            base_color = base_palette[i % len(base_palette)]
            colors.append(base_color)
        return colors

def get_js_color_palette():
    """Use colorblind-friendly palette instead of standard JavaScript colors"""
    return get_colorblind_friendly_palette()

def normalize_to_percentage(data, population):
    """Normalize data to percentage of population (0-100 scale)"""
    if population <= 0:
        return data
    return (data / population) * 100

def load_city_data_for_comparison(city_dirs, metric_file):
    """Load specific metric data from all cities for comparison"""
    city_data = {}
    
    for city_info in city_dirs:
        file_path = os.path.join(city_info['path'], metric_file)
        if os.path.exists(file_path):
            try:
                df = pd.read_csv(file_path)
                if len(df.columns) >= 2:
                    # Normalize to percentage of population
                    normalized_data = normalize_to_percentage(
                        df.iloc[:, 1].values, 
                        city_info['population_num']
                    )
                    
                    city_data[city_info['full_name']] = {
                        'time': df.iloc[:, 0].values,
                        'data': normalized_data,
                        'population': city_info['population_num'],
                        'raw_data': df.iloc[:, 1].values
                    }
            except Exception as e:
                print(f"Error loading {metric_file} for {city_info['full_name']}: {e}")
    
    return city_data

def create_javascript_style_plots(output_dir, city_info, case_detection_ratio=0.8):
    """Create plots matching JavaScript sim.js styling and structure"""
    
    colors = get_js_color_palette()
    city_name = city_info['full_name'] if city_info != 'ALL' else 'Multi-City'
    
    # Define plot configurations matching JavaScript structure
    plot_configs = {
        'Infections': {
            'files': ['num_infected.csv'],
            'colors': [colors['red']],
            'labels': ['Currently Infected'],
            'show_reported': True,
            'reported_color': colors['cyan'],
            'reported_style': '--'
        },
        'Cases': {
            'files': ['num_cases.csv'],
            'colors': [colors['primary_blue']],
            'labels': ['Total Cases'],
            'show_reported': False
        },
        'Hospital System': {
            'files': ['num_hospitalised.csv', 'num_critical.csv'],
            'colors': [colors['orange'], colors['red']],
            'labels': ['Hospitalized', 'Critical Cases'],
            'show_reported': False
        },
        'Outcomes': {
            'files': ['num_recovered.csv', 'num_fatalities.csv'],
            'colors': [colors['green'], colors['brown']],
            'labels': ['Recovered', 'Fatalities'],
            'show_reported': False
        }
    }
    
    # Create dashboard with 6 subplots (2x3 grid like JavaScript)
    fig, axes = plt.subplots(2, 3, figsize=(20, 16))
    fig.suptitle(f'{city_name} Epidemic Simulation Dashboard\n'
                f'JavaScript-Style Visualization (Case Detection Ratio: {case_detection_ratio})', 
                fontsize=20, fontweight='bold', y=0.95)
    
    plot_idx = 0
    
    # Plot each category
    for category, config in plot_configs.items():
        if plot_idx >= 6:
            break
            
        row = plot_idx // 3
        col = plot_idx % 3
        ax = axes[row, col]
        
        max_val = 0
        
        for i, filename in enumerate(config['files']):
            if os.path.exists(filename):
                try:
                    df = pd.read_csv(filename)
                    if len(df.columns) >= 2:
                        time_col = df.columns[0]
                        metric_col = df.columns[1]
                        
                        # Main line plot
                        ax.plot(df[time_col], df[metric_col], 
                               color=config['colors'][i], 
                               linewidth=3, 
                               alpha=0.8,
                               label=config['labels'][i])
                        
                        # Fill area under curve
                        ax.fill_between(df[time_col], df[metric_col], 
                                       alpha=0.2, color=config['colors'][i])
                        
                        max_val = max(max_val, df[metric_col].max())
                        
                        # Add reported cases line for infections plot
                        if config.get('show_reported', False):
                            reported_cases = df[metric_col] * case_detection_ratio
                            ax.plot(df[time_col], reported_cases,
                                   color=config['reported_color'],
                                   linestyle=config['reported_style'],
                                   linewidth=2,
                                   alpha=0.8,
                                   label='Reported Cases')
                        
                        # Add peak annotation
                        if max_val > 0:
                            max_day = df.loc[df[metric_col].idxmax(), time_col]
                            ax.annotate(f'Peak: {max_val:,.0f}\nDay {max_day:.0f}', 
                                       xy=(max_day, max_val), 
                                       xytext=(0.7, 0.8), 
                                       textcoords='axes fraction',
                                       fontsize=11, 
                                       fontweight='bold',
                                       bbox=dict(boxstyle='round,pad=0.5', 
                                               facecolor='white', 
                                               edgecolor=config['colors'][i],
                                               alpha=0.9),
                                       arrowprops=dict(arrowstyle='->', 
                                                     connectionstyle='arc3,rad=0.1',
                                                     color=config['colors'][i], 
                                                     lw=1.5))
                        
                except Exception as e:
                    print(f"Error plotting {filename}: {e}")
        
        # Format axis
        ax.set_title(category, fontsize=16, fontweight='bold', pad=15)
        ax.set_xlabel('Days', fontsize=12, fontweight='bold')
        ax.set_ylabel('Count', fontsize=12, fontweight='bold')
        ax.grid(True, alpha=0.3, linestyle='-', linewidth=0.5)
        ax.set_facecolor('#fafafa')
        
        # Format y-axis with commas
        ax.yaxis.set_major_formatter(plt.FuncFormatter(lambda x, p: f'{x:,.0f}'))
        
        # Set y-limits
        if max_val > 0:
            ax.set_ylim(0, max_val * 1.1)
        
        # Legend
        if len(config['files']) > 1 or config.get('show_reported', False):
            ax.legend(loc='best', fontsize=10, frameon=True, 
                     fancybox=True, shadow=True, framealpha=0.9)
        
        plot_idx += 1
    
    # Add transmission analysis in remaining subplots
    if plot_idx < 6:
        create_transmission_subplot(axes, plot_idx, colors)
    
    plt.tight_layout()
    
    safe_city = city_info['dir_name'] if city_info != 'ALL' else 'all_cities'
    filename = f'{output_dir}/{safe_city}_javascript_style_dashboard.png'
    plt.savefig(filename, dpi=200, bbox_inches='tight', 
               facecolor='white', edgecolor='none')
    print(f"Saved: {filename}")
    plt.show()
    plt.close()

def create_transmission_subplot(axes, start_idx, colors):
    """Create transmission analysis subplot matching JavaScript lambda evolution"""
    
    if start_idx >= 6:
        return
    
    # Use remaining subplots for transmission analysis
    remaining_plots = 6 - start_idx
    
    if remaining_plots >= 2:
        # Transmission by location
        row = start_idx // 3
        col = start_idx % 3
        ax = axes[row, col]
        
        transmission_files = [
            ('susceptible_lambda_H.csv', 'Home', colors['green']),
            ('susceptible_lambda_W.csv', 'Workplace', colors['red']),
            ('susceptible_lambda_C.csv', 'Community', colors['primary_blue']),
            ('susceptible_lambda_T.csv', 'Transport', colors['orange'])
        ]
        
        for filename, label, color in transmission_files:
            if os.path.exists(filename):
                try:
                    df = pd.read_csv(filename)
                    if len(df.columns) >= 2:
                        ax.plot(df.iloc[:, 0], df.iloc[:, 1], 
                               label=label, color=color, linewidth=3, alpha=0.8)
                except Exception as e:
                    print(f"Error reading {filename}: {e}")
        
        ax.set_title('Transmission by Location', fontsize=16, fontweight='bold', pad=15)
        ax.set_xlabel('Days', fontsize=12, fontweight='bold')
        ax.set_ylabel('Susceptible Lambda', fontsize=12, fontweight='bold')
        ax.legend(loc='best', fontsize=10, frameon=True, fancybox=True, shadow=True)
        ax.grid(True, alpha=0.3)
        ax.set_facecolor('#fafafa')

def smooth_data(x, y, method='savgol', window_length=5):
    """Apply smoothing to data similar to JavaScript smoothing techniques"""
    
    from scipy import signal
    from scipy.interpolate import interp1d
    import numpy as np
    
    if len(y) < window_length:
        return x, y
    
    try:
        if method == 'savgol':
            # Savitzky-Golay filter (similar to JavaScript smoothing libraries)
            # Ensure window_length is odd and reasonable
            if window_length % 2 == 0:
                window_length += 1
            if window_length > len(y):
                window_length = len(y) if len(y) % 2 == 1 else len(y) - 1
            if window_length < 3:
                window_length = 3
            
            smoothed_y = signal.savgol_filter(y, window_length, 2)  # 2nd order polynomial
            return x, smoothed_y
            
        elif method == 'moving_average':
            # Simple moving average
            smoothed_y = []
            half_window = window_length // 2
            for i in range(len(y)):
                start = max(0, i - half_window)
                end = min(len(y), i + half_window + 1)
                smoothed_y.append(np.mean(y[start:end]))
            return x, np.array(smoothed_y)
            
        elif method == 'interpolation':
            # Cubic spline interpolation for super smooth curves
            if len(x) > 3:  # Need at least 4 points for cubic
                # Create more points for smoother curve
                x_new = np.linspace(x.min(), x.max(), len(x) * 3)
                f = interp1d(x, y, kind='cubic', bounds_error=False, fill_value='extrapolate')
                y_new = f(x_new)
                return x_new, y_new
            else:
                return x, y
                
    except Exception as e:
        print(f"Smoothing failed: {e}, using original data")
        return x, y
    
    return x, y

def create_daily_plots(output_dir, city_info):
    """Create separate daily plots with JavaScript-style smoothing (default)"""
    
    colors = get_js_color_palette()
    city_name = city_info['full_name'] if city_info != 'ALL' else 'Multi-City'
    
    # Define metrics for daily analysis
    daily_metrics = {
        'New Exposures': {
            'file': 'daily_new_exposed.csv',
            'color': colors['purple'],
            'description': 'Daily New Exposures'
        },
        'New Symptomatic': {
            'file': 'daily_new_symptomatic.csv', 
            'color': colors['orange'],
            'description': 'Daily New Symptomatic'
        },
        'New Hospitalizations': {
            'file': 'daily_new_hospitalised.csv',
            'color': colors['red'],
            'description': 'Daily New Hospitalizations'
        },
        'New Fatalities': {
            'file': 'daily_new_deaths.csv',
            'color': colors['brown'],
            'description': 'Daily New Fatalities'
        },
        'New Recoveries': {
            'file': 'daily_new_recovered.csv',
            'color': colors['green'],
            'description': 'Daily New Recoveries'
        },
        'New Critical': {
            'file': 'daily_new_critical.csv',
            'color': colors['gray'],
            'description': 'Daily New Critical Cases'
        }
    }
    
    fig, axes = plt.subplots(2, 3, figsize=(20, 14))
    
    # Fix title positioning - move main title higher and reduce subplot title size
    fig.suptitle(f'{city_name} - Daily New Cases Analysis', 
                fontsize=18, fontweight='bold', y=0.98)  # Moved higher
    
    plot_idx = 0
    for metric_name, config in daily_metrics.items():
        if plot_idx >= 6:
            break
            
        row = plot_idx // 3
        col = plot_idx % 3
        ax = axes[row, col]
        
        if os.path.exists(config['file']):
            try:
                df = pd.read_csv(config['file'])
                if len(df.columns) >= 2:
                    time_col = df.columns[0]
                    metric_col = df.columns[1]
                    
                    x_data = df[time_col].values
                    y_data = df[metric_col].values
                    
                    # Apply smoothing by default (no user choice)
                    if len(y_data) > 5:
                        # Apply Savitzky-Golay smoothing
                        x_smooth, y_smooth = smooth_data(x_data, y_data, method='savgol', window_length=7)
                        
                        # Plot smoothed line (primary visualization)
                        ax.plot(x_smooth, y_smooth,
                               color=config['color'], 
                               linewidth=3,
                               alpha=0.9,
                               linestyle='-')
                        
                        # Fill area under smoothed curve
                        ax.fill_between(x_smooth, y_smooth, 
                                       alpha=0.2, color=config['color'])
                        
                        # Use original data for peak finding
                        max_val = y_data.max()
                        max_day = x_data[np.argmax(y_data)]
                        
                    else:
                        # No smoothing for short datasets
                        ax.plot(x_data, y_data,
                               color=config['color'], 
                               linewidth=3,
                               alpha=0.8,
                               marker='o',
                               markersize=3,
                               markerfacecolor=config['color'],
                               markeredgecolor='white',
                               markeredgewidth=0.5,
                               linestyle='-')
                        
                        # Fill area under curve
                        ax.fill_between(x_data, y_data, 
                                       alpha=0.2, color=config['color'])
                        
                        max_val = y_data.max()
                        max_day = x_data[np.argmax(y_data)]
                    
                    # Annotate peak - fix positioning to point to actual peak
                    if max_val > 0:
                        # Use smoothed data for annotation if available, otherwise original
                        if 'x_smooth' in locals() and 'y_smooth' in locals():
                            # Find peak in smoothed data
                            smooth_max_idx = np.argmax(y_smooth)
                            smooth_max_day = x_smooth[smooth_max_idx]
                            smooth_max_val = y_smooth[smooth_max_idx]
                            
                            ax.annotate(f'Peak: {max_val:,.0f}\nDay {max_day:.0f}', 
                                       xy=(smooth_max_day, smooth_max_val),  # Point to smoothed peak
                                       xytext=(smooth_max_day + 20, smooth_max_val * 1.2),  # Position text above and right
                                       fontsize=9, 
                                       fontweight='bold',
                                       bbox=dict(boxstyle='round,pad=0.4', 
                                               facecolor='white', 
                                               edgecolor=config['color'],
                                               alpha=0.9),
                                       arrowprops=dict(arrowstyle='->', 
                                                     connectionstyle='arc3,rad=0.1',
                                                     color=config['color'], 
                                                     lw=2))  # Thicker arrow
                        else:
                            ax.annotate(f'Peak: {max_val:,.0f}\nDay {max_day:.0f}', 
                                       xy=(max_day, max_val),  # Point to actual peak coordinates
                                       xytext=(max_day + 20, max_val * 1.2),  # Position text above and right
                                       fontsize=9, 
                                       fontweight='bold',
                                       bbox=dict(boxstyle='round,pad=0.4', 
                                               facecolor='white', 
                                               edgecolor=config['color'],
                                               alpha=0.9),
                                       arrowprops=dict(arrowstyle='->', 
                                                     connectionstyle='arc3,rad=0.1',
                                                     color=config['color'], 
                                                     lw=2))  # Thicker arrow
                    
            except Exception as e:
                print(f"Error reading {config['file']}: {e}")
        
        # Smaller subplot titles to avoid overlap
        ax.set_title(config['description'], fontsize=12, fontweight='bold', pad=10)
        ax.set_xlabel('Days', fontsize=11, fontweight='bold')
        ax.set_ylabel('Daily Count', fontsize=11, fontweight='bold')
        
        # JavaScript-style grid and background
        ax.grid(True, alpha=0.3, linestyle='-', linewidth=0.5)
        ax.set_facecolor('#fafafa')
        
        # Format y-axis with commas
        ax.yaxis.set_major_formatter(plt.FuncFormatter(lambda x, p: f'{x:,.0f}'))
        
        # Set reasonable y-limits
        if 'max_val' in locals() and max_val > 0:
            ax.set_ylim(0, max_val * 1.1)
        
        plot_idx += 1
    
    # Adjust layout with more space for titles
    plt.tight_layout(rect=[0, 0, 1, 0.96])  # Leave space at top for main title
    
    safe_city = city_info['dir_name'] if city_info != 'ALL' else 'all_cities'
    filename = f'{output_dir}/{safe_city}_daily_new_cases.png'
    plt.savefig(filename, dpi=250, bbox_inches='tight',
               facecolor='white', edgecolor='none')
    print(f"Saved: {filename}")
    plt.show()
    plt.close()

def create_cumulative_plots(output_dir, city_info):
    """Create separate cumulative plots matching JavaScript cumulative view"""
    
    colors = get_js_color_palette()
    city_name = city_info['full_name'] if city_info != 'ALL' else 'Multi-City'
    
    # Define metrics for cumulative analysis
    cumulative_metrics = {
        'Total Exposures': {
            'file': 'cumulative_new_exposed.csv',
            'color': colors['purple'],
            'description': 'Cumulative Exposures'
        },
        'Total Symptomatic': {
            'file': 'cumulative_new_symptomatic.csv', 
            'color': colors['orange'],
            'description': 'Cumulative Symptomatic'
        },
        'Total Hospitalizations': {
            'file': 'cumulative_new_hospitalised.csv',
            'color': colors['red'],
            'description': 'Cumulative Hospitalizations'
        },
        'Total Fatalities': {
            'file': 'cumulative_new_deaths.csv',
            'color': colors['brown'],
            'description': 'Cumulative Fatalities'
        },
        'Total Recoveries': {
            'file': 'cumulative_new_recovered.csv',
            'color': colors['green'],
            'description': 'Cumulative Recoveries'
        },
        'Total Critical': {
            'file': 'cumulative_new_critical.csv',
            'color': colors['gray'],
            'description': 'Cumulative Critical Cases'
        }
    }
    
    fig, axes = plt.subplots(2, 3, figsize=(20, 14))
    
    # Fix title positioning - move main title higher
    fig.suptitle(f'{city_name} - Cumulative Cases Analysis', 
                fontsize=18, fontweight='bold', y=0.98)  # Moved higher
    
    plot_idx = 0
    for metric_name, config in cumulative_metrics.items():
        if plot_idx >= 6:
            break
            
        row = plot_idx // 3
        col = plot_idx % 3
        ax = axes[row, col]
        
        if os.path.exists(config['file']):
            try:
                df = pd.read_csv(config['file'])
                if len(df.columns) >= 2:
                    time_col = df.columns[0]
                    metric_col = df.columns[1]
                    
                    # Line plot for cumulative data
                    ax.plot(df[time_col], df[metric_col],
                           color=config['color'], linewidth=3, alpha=0.8)
                    
                    # Fill area under curve
                    ax.fill_between(df[time_col], df[metric_col], 
                                   alpha=0.3, color=config['color'])
                    
                    # Find and annotate final value
                    final_val = df[metric_col].iloc[-1]
                    final_day = df[time_col].iloc[-1]
                    
                    if final_val > 0:
                        ax.annotate(f'Final: {final_val:,.0f}\nDay {final_day:.0f}', 
                                   xy=(final_day, final_val), 
                                   xytext=(0.7, 0.8), 
                                   textcoords='axes fraction',
                                   fontsize=9,  # Slightly smaller
                                   fontweight='bold',
                                   bbox=dict(boxstyle='round,pad=0.4', 
                                           facecolor='white', 
                                           edgecolor=config['color'],
                                           alpha=0.9),
                                   arrowprops=dict(arrowstyle='->', 
                                                 connectionstyle='arc3,rad=0.1',
                                                 color=config['color'], 
                                                 lw=1.5))
                    
            except Exception as e:
                print(f"Error reading {config['file']}: {e}")
        
        # Smaller subplot titles to avoid overlap
        ax.set_title(config['description'], fontsize=12, fontweight='bold', pad=10)
        ax.set_xlabel('Days', fontsize=11, fontweight='bold')
        ax.set_ylabel('Cumulative Count', fontsize=11, fontweight='bold')
        ax.grid(True, alpha=0.3)
        ax.set_facecolor('#fafafa')
        ax.yaxis.set_major_formatter(plt.FuncFormatter(lambda x, p: f'{x:,.0f}'))
        
        plot_idx += 1
    
    # Adjust layout with more space for titles
    plt.tight_layout(rect=[0, 0, 1, 0.96])  # Leave space at top for main title
    
    safe_city = city_info['dir_name'] if city_info != 'ALL' else 'all_cities'
    filename = f'{output_dir}/{safe_city}_cumulative_analysis.png'
    plt.savefig(filename, dpi=250, bbox_inches='tight',
               facecolor='white', edgecolor='none')
    print(f"Saved: {filename}")
    plt.show()
    plt.close()

def create_comprehensive_current_state_plots(output_dir, city_info):
    """Create comprehensive current state plots from num_*.csv files"""
    
    colors = get_colorblind_friendly_palette()
    city_name = city_info['full_name'] if city_info != 'ALL' else 'Multi-City'
    
    # All current state metrics
    current_state_metrics = {
        'Cases': {
            'file': 'num_cases.csv',
            'color': colors['primary_blue'],
            'description': 'Total Cases'
        },
        'Infected': {
            'file': 'num_infected.csv',
            'color': colors['red'],
            'description': 'Currently Infected'
        },
        'Exposed': {
            'file': 'num_exposed.csv',
            'color': colors['purple'],
            'description': 'Currently Exposed'
        },
        'Symptomatic': {
            'file': 'num_symptomatic.csv',
            'color': colors['orange'],
            'description': 'Currently Symptomatic'
        },
        'Hospitalized': {
            'file': 'num_hospitalised.csv',
            'color': colors['red'],
            'description': 'Currently Hospitalized'
        },
        'Critical': {
            'file': 'num_critical.csv',
            'color': colors['brown'],
            'description': 'Currently Critical'
        },
        'Recovered': {
            'file': 'num_recovered.csv',
            'color': colors['green'],
            'description': 'Total Recovered'
        },
        'Fatalities': {
            'file': 'num_fatalities.csv',
            'color': colors['gray'],
            'description': 'Total Fatalities'
        },
        'Affected': {
            'file': 'num_affected.csv',
            'color': colors['cyan'],
            'description': 'Total Affected'
        }
    }
    
    fig, axes = plt.subplots(3, 3, figsize=(24, 18))
    fig.suptitle(f'{city_name} - Current Population States Analysis', 
                fontsize=20, fontweight='bold', y=0.98)
    
    plot_idx = 0
    for metric_name, config in current_state_metrics.items():
        if plot_idx >= 9:
            break
            
        row = plot_idx // 3
        col = plot_idx % 3
        ax = axes[row, col]
        
        if os.path.exists(config['file']):
            try:
                df = pd.read_csv(config['file'])
                if len(df.columns) >= 2:
                    time_col = df.columns[0]
                    metric_col = df.columns[1]
                    
                    # Smooth line plot
                    x_data = df[time_col].values
                    y_data = df[metric_col].values
                    
                    if len(y_data) > 5:
                        x_smooth, y_smooth = smooth_data(x_data, y_data, method='savgol', window_length=7)
                        ax.plot(x_smooth, y_smooth, color=config['color'], linewidth=3, alpha=0.9)
                        ax.fill_between(x_smooth, y_smooth, alpha=0.2, color=config['color'])
                        max_val = y_data.max()
                        max_day = x_data[np.argmax(y_data)]
                    else:
                        ax.plot(x_data, y_data, color=config['color'], linewidth=3, alpha=0.8)
                        ax.fill_between(x_data, y_data, alpha=0.2, color=config['color'])
                        max_val = y_data.max()
                        max_day = x_data[np.argmax(y_data)]
                    
                    # Annotate peak/final value
                    if max_val > 0:
                        if 'Total' in config['description']:
                            final_val = y_data[-1]
                            final_day = x_data[-1]
                            ax.annotate(f'Final: {final_val:,.0f}\nDay {final_day:.0f}', 
                                       xy=(final_day, final_val), xytext=(0.7, 0.8), 
                                       textcoords='axes fraction', fontsize=9, fontweight='bold',
                                       bbox=dict(boxstyle='round,pad=0.4', facecolor='white', 
                                               edgecolor=config['color'], alpha=0.9),
                                       arrowprops=dict(arrowstyle='->', connectionstyle='arc3,rad=0.1',
                                                     color=config['color'], lw=1.5))
                        else:
                            ax.annotate(f'Peak: {max_val:,.0f}\nDay {max_day:.0f}', 
                                       xy=(max_day, max_val), xytext=(0.7, 0.8), 
                                       textcoords='axes fraction', fontsize=9, fontweight='bold',
                                       bbox=dict(boxstyle='round,pad=0.4', facecolor='white', 
                                               edgecolor=config['color'], alpha=0.9),
                                       arrowprops=dict(arrowstyle='->', connectionstyle='arc3,rad=0.1',
                                                     color=config['color'], lw=1.5))
                    
            except Exception as e:
                print(f"Error reading {config['file']}: {e}")
        
        ax.set_title(config['description'], fontsize=12, fontweight='bold', pad=10)
        ax.set_xlabel('Days', fontsize=11, fontweight='bold')
        ax.set_ylabel('Count', fontsize=11, fontweight='bold')
        ax.grid(True, alpha=0.3, linestyle='-', linewidth=0.5)
        ax.set_facecolor('#fafafa')
        ax.yaxis.set_major_formatter(plt.FuncFormatter(lambda x, p: f'{x:,.0f}'))
        
        plot_idx += 1
    
    plt.tight_layout(rect=[0, 0, 1, 0.96])
    
    safe_city = city_info['dir_name'] if city_info != 'ALL' else 'all_cities'
    filename = f'{output_dir}/{safe_city}_current_population_states.png'
    plt.savefig(filename, dpi=250, bbox_inches='tight', facecolor='white', edgecolor='none')
    print(f"Saved: {filename}")
    plt.show()
    plt.close()

def create_comprehensive_lambda_plots(output_dir, city_info):
    """Create single comprehensive lambda analysis combining all lambda files"""
    
    colors = get_enhanced_colorblind_palette()
    city_name = city_info['full_name'] if city_info != 'ALL' else 'Multi-City'
    
    # Create single plot with 4 subplots for different lambda types
    fig, axes = plt.subplots(2, 2, figsize=(20, 16))
    fig.suptitle(f'{city_name} - Lambda Evolution Analysis', 
                fontsize=20, fontweight='bold', y=0.98)
    
    lambda_categories = [
        ('Susceptible Lambda', {
            'Home': ('susceptible_lambda_H.csv', colors[0]),
            'Work': ('susceptible_lambda_W.csv', colors[1]),
            'Community': ('susceptible_lambda_C.csv', colors[2]),
            'Transport': ('susceptible_lambda_T.csv', colors[3]),
            'Random Community': ('susceptible_lambda_RANDOM_COMMUNITY.csv', colors[4])
        }),
        ('Mean Fraction Lambda', {
            'Home': ('mean_fraction_lambda_H.csv', colors[0]),
            'Work': ('mean_fraction_lambda_W.csv', colors[1]),
            'Community': ('mean_fraction_lambda_C.csv', colors[2]),
            'Transport': ('mean_fraction_lambda_T.csv', colors[3]),
            'Random Community': ('mean_fraction_lambda_RANDOM_COMMUNITY.csv', colors[4])
        }),
        ('Total Fraction Lambda', {
            'Home': ('total_fraction_lambda_H.csv', colors[0]),
            'Work': ('total_fraction_lambda_W.csv', colors[1]),
            'Community': ('total_fraction_lambda_C.csv', colors[2]),
            'Transport': ('total_fraction_lambda_T.csv', colors[3]),
            'Random Community': ('total_fraction_lambda_RANDOM_COMMUNITY.csv', colors[4])
        }),
        ('Cumulative Mean Fraction Lambda', {
            'Home': ('cumulative_mean_fraction_lambda_H.csv', colors[0]),
            'Work': ('cumulative_mean_fraction_lambda_W.csv', colors[1]),
            'Community': ('cumulative_mean_fraction_lambda_C.csv', colors[2]),
            'Transport': ('cumulative_mean_fraction_lambda_T.csv', colors[3]),
            'Random Community': ('cumulative_mean_fraction_lambda_RANDOM_COMMUNITY.csv', colors[4])
        })
    ]
    
    for idx, (category_name, category_files) in enumerate(lambda_categories):
        row = idx // 2
        col = idx % 2
        ax = axes[row, col]
        
        for location, (filename, color) in category_files.items():
            if os.path.exists(filename):
                try:
                    df = pd.read_csv(filename)
                    if len(df.columns) >= 2:
                        time_col = df.columns[0]
                        metric_col = df.columns[1]
                        
                        x_data = df[time_col].values
                        y_data = df[metric_col].values
                        
                        if len(y_data) > 5:
                            x_smooth, y_smooth = smooth_data(x_data, y_data, method='savgol', window_length=5)
                            ax.plot(x_smooth, y_smooth, color=color, linewidth=3, alpha=0.9, label=location)
                        else:
                            ax.plot(x_data, y_data, color=color, linewidth=3, alpha=0.8, label=location)
                        
                except Exception as e:
                    print(f"Error reading {filename}: {e}")
        
        ax.set_title(category_name, fontsize=14, fontweight='bold', pad=10)
        ax.set_xlabel('Days', fontsize=11, fontweight='bold')
        ax.set_ylabel('Lambda Value', fontsize=11, fontweight='bold')
        ax.legend(loc='best', fontsize=10, frameon=True, fancybox=True, shadow=True)
        ax.grid(True, alpha=0.3, linestyle='-', linewidth=0.5)
        ax.set_facecolor('#fafafa')
    
    plt.tight_layout(rect=[0, 0, 1, 0.96])
    
    safe_city = city_info['dir_name'] if city_info != 'ALL' else 'all_cities'
    filename = f'{output_dir}/{safe_city}_lambda_evolution_complete.png'
    plt.savefig(filename, dpi=250, bbox_inches='tight', facecolor='white', edgecolor='none')
    print(f"Saved: {filename}")
    plt.show()
    plt.close()

def create_surveillance_analysis(output_dir, city_info, case_detection_ratios=[0.2, 0.5, 0.8]):
    """Create surveillance parameter analysis showing impact of case detection ratio"""
    
    colors = get_enhanced_colorblind_palette()
    city_name = city_info['full_name'] if city_info != 'ALL' else 'Multi-City'
    
    if not os.path.exists('num_infected.csv'):
        print("No infection data found for surveillance analysis")
        return
    
    try:
        df = pd.read_csv('num_infected.csv')
        if len(df.columns) < 2:
            print("Invalid infection data format")
            return
    except Exception as e:
        print(f"Error reading infection data: {e}")
        return
    
    # Create single plot for surveillance analysis with proper spacing
    fig, ax = plt.subplots(1, 1, figsize=(16, 10))
    
    # Adjust subplot parameters to leave more space at top
    plt.subplots_adjust(top=0.85)
    
    # Create main title with proper spacing
    fig.suptitle(f'{city_name} - Surveillance Parameter Analysis', 
                fontsize=18, fontweight='bold', y=0.95)
    
    time_col = df.columns[0]
    infected_col = df.columns[1]
    
    # Plot actual infections
    ax.plot(df[time_col], df[infected_col], 
            color=colors[0], linewidth=4, alpha=0.9,
            label='Actual Infections', linestyle='-')
    
    # Plot reported cases at different detection ratios
    for i, ratio in enumerate(case_detection_ratios):
        reported_cases = df[infected_col] * ratio
        ax.plot(df[time_col], reported_cases,
                color=colors[i+1], 
                linewidth=3, alpha=0.8,
                linestyle='--', 
                label=f'Reported Cases (ρ={ratio})')
    
    # Set subplot title with adequate padding
    ax.set_title('Actual vs Reported Infections at Different Detection Ratios', 
                fontsize=14, fontweight='bold', pad=20)
    ax.set_xlabel('Days', fontsize=12, fontweight='bold')
    ax.set_ylabel('Count', fontsize=12, fontweight='bold')
    ax.legend(loc='best', fontsize=12, frameon=True, fancybox=True, shadow=True)
    ax.grid(True, alpha=0.3)
    ax.set_facecolor('#fafafa')
    ax.yaxis.set_major_formatter(plt.FuncFormatter(lambda x, p: f'{x:,.0f}'))
    
    # Use tight_layout with proper padding
    plt.tight_layout(rect=[0, 0, 1, 0.92])
    
    safe_city = city_info['dir_name'] if city_info != 'ALL' else 'all_cities'
    filename = f'{output_dir}/{safe_city}_surveillance_analysis.png'
    plt.savefig(filename, dpi=250, bbox_inches='tight',
               facecolor='white', edgecolor='none')
    print(f"Saved: {filename}")
    plt.show()
    plt.close()

def analyze_simulation_parameters(city_info):
    """Read and display simulation parameters"""
    
    if city_info != 'ALL':
        title = f"{city_info['full_name']} SIMULATION PARAMETERS"
    else:
        title = "SIMULATION PARAMETERS"
    
    print("\n" + "="*60)
    print(title)
    print("="*60)
    
    if os.path.exists('global_params.txt'):
        with open('global_params.txt', 'r') as f:
            content = f.read()
            print(content)
    
    if os.path.exists('disease_label_stats.csv'):
        print("\n" + "="*50)
        print("DISEASE STATISTICS SUMMARY") 
        print("="*50)
        df = pd.read_csv('disease_label_stats.csv')
        print(df.head(10))

def print_key_statistics(city_info):
    """Print key statistics from the simulation"""
    
    city_name = city_info['full_name'] if city_info != 'ALL' else 'Current City'
    
    print("\n" + "="*60)
    print(f"{city_name.upper()} - KEY SIMULATION OUTCOMES")
    print("="*60)
    
    key_files = [
        ('num_cases.csv', 'Total Cases'),
        ('num_infected.csv', 'Peak Infected'), 
        ('num_recovered.csv', 'Final Recovered'),
        ('num_fatalities.csv', 'Total Fatalities'),
        ('num_hospitalised.csv', 'Peak Hospitalized')
    ]
    
    for filename, description in key_files:
        if os.path.exists(filename):
            df = pd.read_csv(filename)
            if len(df.columns) >= 2:
                max_val = df.iloc[:, 1].max()
                final_val = df.iloc[-1, 1]
                max_day = df.loc[df.iloc[:, 1].idxmax(), df.columns[0]]
                
                if 'Peak' in description:
                    print(f"{description}: {max_val:,.0f} (Day {max_day})")
                elif 'Total' in description or 'Final' in description:
                    print(f"{description}: {final_val:,.0f}")
                else:
                    print(f"{description}: {final_val:,.0f}")

# COMPARATIVE ANALYSIS FUNCTIONS

def create_comparative_dashboard(output_dir, city_dirs):
    """Create comprehensive comparative dashboard with 0-100% scale"""
    
    colors = get_enhanced_colorblind_palette()
    
    # Define key metrics for comparison
    metrics = {
        'Peak Infections': 'num_infected.csv',
        'Total Cases': 'num_cases.csv',
        'Peak Hospitalizations': 'num_hospitalised.csv',
        'Peak Critical Cases': 'num_critical.csv',
        'Total Recoveries': 'num_recovered.csv',
        'Total Fatalities': 'num_fatalities.csv'
    }
    
    # Create 2x3 subplot layout
    fig, axes = plt.subplots(2, 3, figsize=(24, 16))
    fig.suptitle('Comparative Epidemic Analysis Across All Cities\n'
                'Normalized to Percentage of Population (0-100% Scale)', 
                fontsize=20, fontweight='bold', y=0.98)
    
    plot_idx = 0
    
    for metric_name, metric_file in metrics.items():
        if plot_idx >= 6:
            break
            
        row = plot_idx // 3
        col = plot_idx % 3
        ax = axes[row, col]
        
        # Load data for all cities
        city_data = load_city_data_for_comparison(city_dirs, metric_file)
        
        if not city_data:
            ax.text(0.5, 0.5, f'No data available\nfor {metric_name}', 
                   ha='center', va='center', transform=ax.transAxes,
                   fontsize=14, bbox=dict(boxstyle='round', facecolor='lightgray'))
            ax.set_title(metric_name, fontsize=14, fontweight='bold')
            plot_idx += 1
            continue
        
        # Plot each city
        max_percentage = 0
        for i, (city_name, data) in enumerate(city_data.items()):
            color = colors[i % len(colors)]
            
            ax.plot(data['time'], data['data'], 
                   color=color, linewidth=2.5, alpha=0.8,
                   label=city_name)
            
            max_percentage = max(max_percentage, data['data'].max())
        
        # Format the plot
        ax.set_title(metric_name, fontsize=14, fontweight='bold', pad=15)
        ax.set_xlabel('Days', fontsize=12, fontweight='bold')
        ax.set_ylabel('% of Population', fontsize=12, fontweight='bold')
        
        # Set y-axis to 0-100 scale with appropriate limits
        if max_percentage > 0:
            y_limit = min(100, max_percentage * 1.1)
            ax.set_ylim(0, y_limit)
        else:
            ax.set_ylim(0, 10)
        
        # Add percentage formatting to y-axis
        ax.yaxis.set_major_formatter(plt.FuncFormatter(lambda x, p: f'{x:.1f}%'))
        
        # Grid and styling
        ax.grid(True, alpha=0.3, linestyle='-', linewidth=0.5)
        ax.set_facecolor('#fafafa')
        
        # Legend (only if multiple cities)
        if len(city_data) > 1:
            ax.legend(loc='best', fontsize=9, frameon=True, 
                     fancybox=True, shadow=True, framealpha=0.9)
        
        plot_idx += 1
    
    plt.tight_layout(rect=[0, 0, 1, 0.96])
    
    filename = f'{output_dir}/comparative_dashboard_normalized.png'
    plt.savefig(filename, dpi=300, bbox_inches='tight', 
               facecolor='white', edgecolor='none')
    print(f"Saved: {filename}")
    plt.show()
    plt.close()

def create_peak_comparison_bar_chart(output_dir, city_dirs):
    """Create bar chart comparing peak values across cities"""
    
    colors = get_enhanced_colorblind_palette()
    
    metrics = {
        'Peak Infections (%)': 'num_infected.csv',
        'Peak Hospitalizations (%)': 'num_hospitalised.csv',
        'Total Fatalities (%)': 'num_fatalities.csv'
    }
    
    fig, axes = plt.subplots(1, 3, figsize=(20, 8))
    fig.suptitle('Peak Values Comparison Across Cities\n'
                'Normalized as Percentage of Population (0-100% Scale)', 
                fontsize=18, fontweight='bold', y=0.95)
    
    for idx, (metric_name, metric_file) in enumerate(metrics.items()):
        ax = axes[idx]
        
        # Collect peak values for all cities
        city_names = []
        peak_values = []
        
        for city_info in city_dirs:
            file_path = os.path.join(city_info['path'], metric_file)
            if os.path.exists(file_path):
                try:
                    df = pd.read_csv(file_path)
                    if len(df.columns) >= 2:
                        if 'Fatalities' in metric_name:
                            # For fatalities, use final value (cumulative)
                            raw_value = df.iloc[-1, 1]
                        else:
                            # For others, use peak value
                            raw_value = df.iloc[:, 1].max()
                        
                        percentage = normalize_to_percentage(raw_value, city_info['population_num'])
                        
                        city_names.append(city_info['city'])
                        peak_values.append(percentage)
                except Exception as e:
                    print(f"Error processing {metric_file} for {city_info['full_name']}: {e}")
        
        if peak_values:
            # Create bar chart
            bars = ax.bar(city_names, peak_values, 
                         color=[colors[i % len(colors)] for i in range(len(city_names))],
                         alpha=0.8, edgecolor='black', linewidth=1)
            
            # Add value labels on bars
            for bar, value in zip(bars, peak_values):
                height = bar.get_height()
                ax.text(bar.get_x() + bar.get_width()/2., height + 0.01,
                       f'{value:.2f}%', ha='center', va='bottom',
                       fontweight='bold', fontsize=10)
            
            ax.set_title(metric_name, fontsize=14, fontweight='bold')
            ax.set_ylabel('% of Population', fontsize=12, fontweight='bold')
            ax.set_xlabel('Cities', fontsize=12, fontweight='bold')
            
            # Rotate x-axis labels for better readability
            plt.setp(ax.get_xticklabels(), rotation=45, ha='right')
            
            # Set y-axis limits with 0-100 scale
            max_val = max(peak_values) if peak_values else 1
            y_max = min(100, max_val * 1.2)
            ax.set_ylim(0, y_max)
            
            # Add percentage formatting
            ax.yaxis.set_major_formatter(plt.FuncFormatter(lambda x, p: f'{x:.1f}%'))
            
            ax.grid(True, alpha=0.3, axis='y')
            ax.set_facecolor('#fafafa')
    
    plt.tight_layout()
    
    filename = f'{output_dir}/peak_values_comparison.png'
    plt.savefig(filename, dpi=300, bbox_inches='tight', 
               facecolor='white', edgecolor='none')
    print(f"Saved: {filename}")
    plt.show()
    plt.close()

def create_summary_statistics_table(output_dir, city_dirs):
    """Create comprehensive summary statistics table"""
    
    metrics_files = {
        'Peak Infections (%)': 'num_infected.csv',
        'Total Cases (%)': 'num_cases.csv',
        'Peak Hospitalizations (%)': 'num_hospitalised.csv',
        'Total Fatalities (%)': 'num_fatalities.csv',
        'Final Recoveries (%)': 'num_recovered.csv'
    }
    
    # Collect data
    summary_data = []
    
    for city_info in city_dirs:
        row = {'City': city_info['city'], 'Population': city_info['population']}
        
        for metric_name, metric_file in metrics_files.items():
            file_path = os.path.join(city_info['path'], metric_file)
            if os.path.exists(file_path):
                try:
                    df = pd.read_csv(file_path)
                    if len(df.columns) >= 2:
                        if 'Peak' in metric_name:
                            raw_value = df.iloc[:, 1].max()
                        else:
                            raw_value = df.iloc[-1, 1]  # Final value
                        
                        percentage = normalize_to_percentage(raw_value, city_info['population_num'])
                        row[metric_name] = f"{percentage:.2f}%"
                except Exception as e:
                    row[metric_name] = "N/A"
            else:
                row[metric_name] = "N/A"
        
        summary_data.append(row)
    
    # Create table visualization
    fig, ax = plt.subplots(figsize=(16, 10))
    ax.axis('tight')
    ax.axis('off')
    
    # Convert to DataFrame for easier handling
    df_summary = pd.DataFrame(summary_data)
    
    # Create table
    table = ax.table(cellText=df_summary.values,
                    colLabels=df_summary.columns,
                    cellLoc='center',
                    loc='center',
                    bbox=[0, 0, 1, 1])
    
    # Style the table
    table.auto_set_font_size(False)
    table.set_fontsize(11)
    table.scale(1, 2)
    
    # Header styling
    for i in range(len(df_summary.columns)):
        table[(0, i)].set_facecolor('#4CAF50')
        table[(0, i)].set_text_props(weight='bold', color='white')
    
    # Alternating row colors
    for i in range(1, len(df_summary) + 1):
        for j in range(len(df_summary.columns)):
            if i % 2 == 0:
                table[(i, j)].set_facecolor('#f0f0f0')
            else:
                table[(i, j)].set_facecolor('white')
    
    plt.title('Comparative Summary Statistics\n'
             'Normalized as Percentage of Population (0-100% Scale)', 
             fontsize=16, fontweight='bold', pad=20)
    
    filename = f'{output_dir}/summary_statistics_table.png'
    plt.savefig(filename, dpi=300, bbox_inches='tight', 
               facecolor='white', edgecolor='none')
    print(f"Saved: {filename}")
    plt.show()
    plt.close()
    
    # Also save as CSV
    csv_filename = f'{output_dir}/summary_statistics.csv'
    df_summary.to_csv(csv_filename, index=False)
    print(f"Saved CSV: {csv_filename}")

def create_timeline_comparison(output_dir, city_dirs):
    """Create timeline comparison showing when peaks occur"""
    
    colors = get_enhanced_colorblind_palette()
    
    metrics = {
        'Peak Infections': 'num_infected.csv',
        'Peak Hospitalizations': 'num_hospitalised.csv'
    }
    
    fig, axes = plt.subplots(1, 2, figsize=(16, 8))
    fig.suptitle('Timeline Comparison: Peak Occurrence Across Cities\n'
                'Y-axis: Peak Value as % of Population (0-100% Scale)', 
                fontsize=18, fontweight='bold', y=0.95)
    
    for idx, (metric_name, metric_file) in enumerate(metrics.items()):
        ax = axes[idx]
        
        city_names = []
        peak_days = []
        peak_percentages = []
        
        for city_info in city_dirs:
            file_path = os.path.join(city_info['path'], metric_file)
            if os.path.exists(file_path):
                try:
                    df = pd.read_csv(file_path)
                    if len(df.columns) >= 2:
                        peak_idx = df.iloc[:, 1].idxmax()
                        peak_day = df.iloc[peak_idx, 0]
                        peak_value = df.iloc[peak_idx, 1]
                        peak_percentage = normalize_to_percentage(peak_value, city_info['population_num'])
                        
                        city_names.append(city_info['city'])
                        peak_days.append(peak_day)
                        peak_percentages.append(peak_percentage)
                except Exception as e:
                    print(f"Error processing {metric_file} for {city_info['full_name']}: {e}")
        
        if peak_days:
            # Create scatter plot
            scatter = ax.scatter(peak_days, peak_percentages, 
                               c=[colors[i % len(colors)] for i in range(len(city_names))],
                               s=200, alpha=0.7, edgecolors='black', linewidth=1)
            
            # Add city labels
            for i, (day, percentage, city) in enumerate(zip(peak_days, peak_percentages, city_names)):
                ax.annotate(city, (day, percentage), 
                           xytext=(5, 5), textcoords='offset points',
                           fontsize=10, fontweight='bold')
            
            ax.set_title(f'{metric_name} Timeline', fontsize=14, fontweight='bold')
            ax.set_xlabel('Day of Peak Occurrence', fontsize=12, fontweight='bold')
            ax.set_ylabel('Peak Value (% of Population)', fontsize=12, fontweight='bold')
            
            # Set y-axis limits with 0-100 scale
            max_percentage = max(peak_percentages) if peak_percentages else 1
            y_max = min(100, max_percentage * 1.2)
            ax.set_ylim(0, y_max)
            
            # Format y-axis
            ax.yaxis.set_major_formatter(plt.FuncFormatter(lambda x, p: f'{x:.1f}%'))
            
            ax.grid(True, alpha=0.3)
            ax.set_facecolor('#fafafa')
    
    plt.tight_layout()
    
    filename = f'{output_dir}/timeline_comparison.png'
    plt.savefig(filename, dpi=300, bbox_inches='tight', 
               facecolor='white', edgecolor='none')
    print(f"Saved: {filename}")
    plt.show()
    plt.close()

def analyze_single_city(city_info, base_output_dir):
    """Analyze a single city's simulation data with JavaScript-style visualizations"""
    
    print(f"\n{'='*80}")
    print(f"ANALYZING: {city_info['full_name']}")
    print(f"{'='*80}")
    
    # Change to city directory
    original_dir = os.getcwd()
    os.chdir(city_info['path'])
    
    # Setup output directory
    output_dir = setup_output_directory(city_info)
    
    try:
        # Run streamlined analysis (removed JavaScript dashboard)
        print("\n1. Analyzing simulation parameters...")
        analyze_simulation_parameters(city_info)
        
        print("\n2. Creating surveillance parameter analysis...")
        create_surveillance_analysis(output_dir, city_info)
        
        print("\n3. Creating daily new cases analysis...")
        create_daily_plots(output_dir, city_info)
        
        print("\n4. Creating current population states analysis...")
        create_comprehensive_current_state_plots(output_dir, city_info)
        
        print("\n5. Creating cumulative analysis...")
        create_cumulative_plots(output_dir, city_info)
        
        print("\n6. Creating lambda evolution analysis...")
        create_comprehensive_lambda_plots(output_dir, city_info)
        
        print("\n7. Summary statistics...")
        print_key_statistics(city_info)
        
        return output_dir
        
    finally:
        # Always return to original directory
        os.chdir(original_dir)

def create_comparative_analysis(city_dirs, output_dir):
    """Run comparative analysis with 0-100% scale - Only Dashboard"""
    
    print("\n" + "="*80)
    print("CREATING COMPARATIVE ANALYSIS (0-100% SCALE)")
    print("="*80)
    
    print(f"Analyzing {len(city_dirs)} cities for comparison:")
    for city_info in city_dirs:
        print(f"  - {city_info['full_name']} (Pop: {city_info['population']})")
    
    print("\n1. Creating comparative dashboard (normalized 0-100%)...")
    create_comparative_dashboard(output_dir, city_dirs)

def main():
    """Main function to run enhanced epidemic analysis"""
    
    print("="*80)
    print("ENHANCED EPIDEMIC DATA ANALYZER")
    print("Individual Analysis + Comparative Analysis (0-100% Scale)")
    print("="*80)
    
    # Set up JavaScript-style plotting
    setup_plot_style()
    
    # Discover available city directories
    city_dirs = discover_city_directories()
    
    if not city_dirs:
        return
    
    # Let user select city
    selected = select_city_directory(city_dirs)
    
    if selected is None:
        return
    
    output_paths = []
    
    if selected == 'ALL':
        # Analyze each city individually with JavaScript styling
        print(f"\n{'='*80}")
        print("ANALYZING ALL CITIES INDIVIDUALLY")
        print(f"{'='*80}")
        
        for city_info in city_dirs:
            output_path = analyze_single_city(city_info, os.getcwd())
            output_paths.append(output_path)
            
    elif selected == 'COMPARATIVE':
        # Create comparative analysis with 0-100% scale
        output_dir = setup_output_directory('COMPARATIVE')
        create_comparative_analysis(city_dirs, output_dir)
        output_paths.append(output_dir)
        
    else:
        # Analyze single selected city
        output_path = analyze_single_city(selected, os.getcwd())
        output_paths.append(output_path)
    
    # Final summary
    print("\n" + "="*80)
    print("COMPREHENSIVE ANALYSIS COMPLETE!")
    print("="*80)
    
    total_png_files = 0
    total_csv_files = 0
    
    for output_path in output_paths:
        if os.path.exists(output_path):
            png_files = [f for f in os.listdir(output_path) if f.endswith('.png')]
            csv_files = [f for f in os.listdir(output_path) if f.endswith('.csv')]
            
            total_png_files += len(png_files)
            total_csv_files += len(csv_files)
            
            print(f"\nGenerated files in {os.path.basename(output_path)}/:")
            print(f"  📊 {len(png_files)} visualization files")
            if csv_files:
                print(f"  📋 {len(csv_files)} data summary files")
            
            # Show sample files
            for png in sorted(png_files)[:3]:  # Show first 3
                print(f"    - {png}")
            if len(png_files) > 3:
                print(f"    ... and {len(png_files)-3} more")
    
    print(f"\n📈 TOTAL OUTPUT: {total_png_files} visualizations, {total_csv_files} data files")
    
    print("\n✅ KEY FEATURES:")
    if selected == 'COMPARATIVE':
        print("  🎯 Comparative dashboard with 0-100% normalized scale")
        print("  📊 All metrics shown as percentage of population")
        print("  🏙️  Easy comparison across different city sizes")
        print("  📈 Single comprehensive view of all key metrics")
    else:
        print("  🎨 JavaScript-style individual city analysis")
        print("  📱 Interactive-style dashboards and detailed plots")
        print("  🔬 Lambda evolution and transmission analysis")
        print("  📊 Daily, cumulative, and current state views")
        print("  🎯 Surveillance parameter analysis")
    
    print("  🌈 Colorblind-friendly scientific color palette")
    print("  📸 High-resolution outputs (250-300 DPI)")
    print("  📂 Organized file structure with clear naming")
    print("  ♿ Accessibility-compliant visualizations")
    
    storage_info = {
        'COMPARATIVE': '/data/tarunks/epidemic-simulator-master/output_modified/comparative_analysis_all_cities/',
        'ALL': '/data/tarunks/epidemic-simulator-master/output_modified/individual_analysis_all_cities/',
        'SINGLE': 'output_images_[cityname]/'
    }
    
    if selected == 'COMPARATIVE':
        print(f"\n📁 Results stored in: {storage_info['COMPARATIVE']}")
    elif selected == 'ALL':
        print(f"\n📁 Results stored in: {storage_info['ALL']}")
    else:
        print(f"\n📁 Results stored in: output_images_{selected['dir_name']}/")

if __name__ == "__main__":
    main()