import numpy as np
import matplotlib.pyplot as plt
from scipy import stats

# PROBLEM 1: Kyoto Population Linear Regression
print("="*60)
print("PROBLEM 1: KYOTO POPULATION LINEAR REGRESSION")
print("="*60)

# Data
years = np.array([1990, 1995, 2000, 2005, 2010, 2015, 2020, 2024])
population = np.array([1461, 1464, 1468, 1475, 1474, 1475, 1464, 1450])

# Linear regression
slope, intercept, r_value, p_value, std_err = stats.linregress(years, population)

print(f"\nRegression Equation: y = {slope:.4f}x + {intercept:.2f}")
print(f"Slope (β₁): {slope:.4f}")
print(f"Intercept (β₀): {intercept:.2f}")
print(f"R-squared: {r_value**2:.4f}")
print(f"p-value: {p_value:.6f}")

# Prediction for 2050
year_2050 = 2050
pop_2050 = slope * year_2050 + intercept
print(f"\nPredicted population for 2050: {pop_2050:.2f} thousand")
print(f"That is: {int(pop_2050 * 1000):,} people")

# Calculate residuals
predictions = slope * years + intercept
residuals = population - predictions

print("\n" + "-"*50)
print("Year\tActual\tPredicted\tResidual")
print("-"*50)
for i in range(len(years)):
    print(f"{years[i]}\t{population[i]}\t{predictions[i]:.2f}\t\t{residuals[i]:.2f}")

# Error analysis
print(f"\nError Analysis:")
print(f"Mean of residuals: {np.mean(residuals):.4f}")
print(f"Std dev of residuals: {np.std(residuals, ddof=1):.4f}")

# Normality test
shapiro_stat, shapiro_p = stats.shapiro(residuals)
print(f"\nShapiro-Wilk test for normality:")
print(f"Statistic: {shapiro_stat:.4f}")
print(f"p-value: {shapiro_p:.4f}")
print(f"Conclusion: {'Normal' if shapiro_p > 0.05 else 'Not normal'} distribution (α = 0.05)")

# Create figure for Problem 1
fig1, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(12, 10))
fig1.suptitle('Problem 1: Kyoto Population Analysis', fontsize=16)

# 1. Scatter plot with regression line
ax1.scatter(years, population, color='blue', s=100, label='Actual data')
ax1.plot(years, predictions, 'r-', label='Regression line', linewidth=2)

# Extend to 2050
extended_years = np.linspace(years[-1], 2050, 50)
extended_preds = slope * extended_years + intercept
ax1.plot(extended_years, extended_preds, 'r--', alpha=0.5)
ax1.scatter([2050], [pop_2050], color='green', s=150, 
            label=f'2050: {pop_2050:.0f}k', zorder=5)

ax1.set_xlabel('Year')
ax1.set_ylabel('Population (thousands)')
ax1.set_title('Population Trend and Prediction')
ax1.legend()
ax1.grid(True, alpha=0.3)

# 2. Residual plot
ax2.scatter(years, residuals, color='purple', s=80)
ax2.axhline(y=0, color='black', linestyle='--', linewidth=1)
ax2.set_xlabel('Year')
ax2.set_ylabel('Residuals')
ax2.set_title('Residual Plot')
ax2.grid(True, alpha=0.3)

# 3. Histogram of residuals
ax3.hist(residuals, bins=5, density=True, alpha=0.7, color='green', edgecolor='black')
x_norm = np.linspace(residuals.min()-5, residuals.max()+5, 100)
ax3.plot(x_norm, stats.norm.pdf(x_norm, residuals.mean(), np.std(residuals)), 
         'r-', linewidth=2, label='Normal curve')
ax3.set_xlabel('Residuals')
ax3.set_ylabel('Density')
ax3.set_title('Distribution of Residuals')
ax3.legend()
ax3.grid(True, alpha=0.3)

# 4. Q-Q plot
stats.probplot(residuals, dist="norm", plot=ax4)
ax4.set_title('Q-Q Plot')
ax4.grid(True, alpha=0.3)

plt.tight_layout()

# PROBLEM 2: Least Squares Fit
print("\n" + "="*60)
print("PROBLEM 2: LEAST SQUARES FIT FOR y = b|x - 2|")
print("="*60)

# Given data
x_data = np.array([-2, 0, 4])
y_data = np.array([1.5, 0.5, 1])

# Transform x values
x_transformed = np.abs(x_data - 2)

print("\nData transformation:")
print("x\t|x-2|\ty")
print("-"*25)
for i in range(len(x_data)):
    print(f"{x_data[i]}\t{x_transformed[i]}\t{y_data[i]}")

# Calculate least squares estimate
numerator = np.sum(y_data * x_transformed)
denominator = np.sum(x_transformed**2)
b_estimate = numerator / denominator

print(f"\nLeast squares calculation:")
print(f"Σ(yᵢ|xᵢ-2|) = {numerator}")
print(f"Σ(|xᵢ-2|²) = {denominator}")
print(f"b = {numerator}/{denominator} = {b_estimate:.4f}")

# Calculate fitted values and residuals
y_fitted = b_estimate * x_transformed
residuals_p2 = y_data - y_fitted

print("\nFitted values and residuals:")
print("x\ty_actual\ty_fitted\tresidual")
print("-"*45)
for i in range(len(x_data)):
    print(f"{x_data[i]}\t{y_data[i]}\t\t{y_fitted[i]:.4f}\t\t{residuals_p2[i]:.4f}")

print(f"\nSum of squared residuals: {np.sum(residuals_p2**2):.4f}")

# Create figure for Problem 2
fig2 = plt.figure(figsize=(10, 6))
plt.title('Problem 2: Least Squares Fit for y = b|x - 2|', fontsize=14)

# Plot data points
plt.scatter(x_data, y_data, color='blue', s=100, label='Data points', zorder=5)

# Plot fitted curve
x_range = np.linspace(-3, 5, 1000)
y_curve = b_estimate * np.abs(x_range - 2)
plt.plot(x_range, y_curve, 'r-', linewidth=2, 
         label=f'y = {b_estimate:.3f}|x - 2|')

# Add vertical line at x = 2
plt.axvline(x=2, color='gray', linestyle='--', alpha=0.5, label='x = 2')

# Annotate points
for i, (xi, yi) in enumerate(zip(x_data, y_data)):
    plt.annotate(f'({xi}, {yi})', (xi, yi), xytext=(10, 10), 
                textcoords='offset points', fontsize=10,
                bbox=dict(boxstyle='round,pad=0.3', facecolor='yellow', alpha=0.5))

plt.xlabel('x', fontsize=12)
plt.ylabel('y', fontsize=12)
plt.legend(fontsize=11)
plt.grid(True, alpha=0.3)
plt.xlim(-3.5, 5.5)
plt.ylim(-0.5, 2)

# Show all plots
plt.show()

print("\n" + "="*60)
print("ANALYSIS COMPLETE")
print("="*60)