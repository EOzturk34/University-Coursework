import numpy as np
import matplotlib.pyplot as plt
from scipy import stats

# Problem 13: Central Limit Theorem Simulation
def simulate_clt():
    """
    Simulate the Central Limit Theorem using uniform distribution
    """
    # Parameters
    n_samples = 10000  # Number of samples
    sample_size = 30   # Size of each sample
    
    # Generate samples and compute means
    sample_means = []
    for i in range(n_samples):
        # Generate sample from Uniform(0,1)
        sample = np.random.uniform(0, 1, sample_size)
        # Compute and store the mean
        sample_means.append(np.mean(sample))
    
    sample_means = np.array(sample_means)
    
    # Create histogram
    plt.figure(figsize=(10, 6))
    
    # Plot histogram of sample means
    n, bins, patches = plt.hist(sample_means, bins=50, density=True, 
                                alpha=0.7, color='blue', edgecolor='black')
    
    # Theoretical parameters for the normal distribution
    # For Uniform(0,1): mean = 0.5, variance = 1/12
    # For sample mean: mean = 0.5, variance = (1/12)/n
    theoretical_mean = 0.5
    theoretical_std = np.sqrt(1/12) / np.sqrt(sample_size)
    
    # Plot normal density curve
    x = np.linspace(sample_means.min(), sample_means.max(), 100)
    normal_pdf = stats.norm.pdf(x, theoretical_mean, theoretical_std)
    plt.plot(x, normal_pdf, 'r-', linewidth=2, label='Normal density curve')
    
    # Add labels and title
    plt.xlabel('Sample Mean')
    plt.ylabel('Density')
    plt.title(f'Central Limit Theorem: Distribution of Sample Means\n' + 
              f'(n={sample_size}, {n_samples} samples from Uniform(0,1))')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    # Add statistics text
    empirical_mean = np.mean(sample_means)
    empirical_std = np.std(sample_means)
    plt.text(0.02, 0.95, f'Empirical mean: {empirical_mean:.4f}\n' + 
             f'Theoretical mean: {theoretical_mean:.4f}\n' + 
             f'Empirical std: {empirical_std:.4f}\n' + 
             f'Theoretical std: {theoretical_std:.4f}',
             transform=plt.gca().transAxes, verticalalignment='top',
             bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))
    
    plt.tight_layout()
    # Don't show yet - wait until both figures are created

# Problem 14: Maximum Likelihood Estimation for Uniform Distribution
def mle_uniform_simulation():
    """
    Perform MLE for the parameter θ of uniform distribution U(0,θ)
    """
    # Parameters
    true_theta = 10
    sample_size = 100
    n_simulations = 1000
    
    # Store MLE estimates
    mle_estimates = []
    
    for i in range(n_simulations):
        # Generate sample from U(0,10)
        sample = np.random.uniform(0, true_theta, sample_size)
        
        # MLE for U(0,θ) is the maximum observed value
        mle_theta = np.max(sample)
        mle_estimates.append(mle_theta)
    
    mle_estimates = np.array(mle_estimates)
    
    # Create plot
    plt.figure(figsize=(10, 6))
    
    # Plot histogram of MLE estimates
    n, bins, patches = plt.hist(mle_estimates, bins=50, density=True, 
                                alpha=0.7, color='green', edgecolor='black')
    
    # Add vertical line for true parameter
    plt.axvline(x=true_theta, color='red', linestyle='--', linewidth=2, 
                label=f'True θ = {true_theta}')
    
    # Add vertical line for mean of estimates
    mean_estimate = np.mean(mle_estimates)
    plt.axvline(x=mean_estimate, color='blue', linestyle='--', linewidth=2, 
                label=f'Mean MLE = {mean_estimate:.3f}')
    
    # Labels and title
    plt.xlabel('MLE Estimate of θ')
    plt.ylabel('Density')
    plt.title(f'Distribution of Maximum Likelihood Estimates\n' + 
              f'for θ in U(0,θ) with true θ = {true_theta}\n' + 
              f'({n_simulations} simulations, sample size = {sample_size})')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    # Add statistics text
    bias = mean_estimate - true_theta
    plt.text(0.02, 0.95, f'Mean MLE: {mean_estimate:.4f}\n' + 
             f'Bias: {bias:.4f}\n' + 
             f'Std of MLE: {np.std(mle_estimates):.4f}\n' + 
             f'Min MLE: {np.min(mle_estimates):.4f}\n' + 
             f'Max MLE: {np.max(mle_estimates):.4f}',
             transform=plt.gca().transAxes, verticalalignment='top',
             bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))
    
    plt.tight_layout()
    # Don't show yet - wait until both figures are created

# Main execution
if __name__ == "__main__":
    print("Running Central Limit Theorem Simulation...")
    simulate_clt()
    
    print("\nRunning Maximum Likelihood Estimation Simulation...")
    mle_uniform_simulation()
    
    # Show both figures at the same time
    plt.show()
    
    # Additional analysis for MLE
    print("\nTheoretical Notes:")
    print("1. For U(0,θ), the MLE is biased but consistent")
    print("2. The expected value of max(X₁,...,Xₙ) = θ * n/(n+1)")
    print("3. As n → ∞, the bias → 0")
    
    # Demonstrate the theoretical bias
    n = 100
    theta = 10
    theoretical_mean_mle = theta * n / (n + 1)
    print(f"\nFor n={n}, θ={theta}:")
    print(f"Theoretical mean of MLE: {theoretical_mean_mle:.4f}")
    print(f"Theoretical bias: {theoretical_mean_mle - theta:.4f}")