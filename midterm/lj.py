import numpy as np

def energy(x):
    """
    Compute the Lennard-Jones energy U(x) of the system.
    """
    E = 0
    n = len(x) // 3
    x = x.reshape((n,3))
    dx = np.subtract.outer(x[:,0], x[:,0])
    dy = np.subtract.outer(x[:,1], x[:,1])
    dz = np.subtract.outer(x[:,2], x[:,2])
    r2 = dx**2 + dy**2 + dz**2
    np.fill_diagonal(r2, 1) # fill diagonal with non zero to avoid division by zero
    r6inv = r2**(-3)
    r12inv = r6inv**2
    E = r12inv - r6inv
    np.fill_diagonal(E, 0)
    return 2 * np.sum(E)

if __name__ == "__main__":
    np.random.seed(123456)

    T = 1.0
    sigma = 0.05
    N = 32
    M = 50000
    B = 40000



    # TODO Write your code here
