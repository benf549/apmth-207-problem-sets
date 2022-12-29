#!/usr/bin/env python3

import korali
import numpy as np

def lennard_jones(x): 
    E = 0
    m = len(x)
    for i in range(0, m, 3):
        for j in range(i + 3, m, 3):
            dx = x[i] - x[j]
            dy = x[i + 1] - x[j + 1]
            dz = x[i + 2] - x[j + 2]
            r2 = dx**2 + dy**2 + dz**2
            r2inv = 1 / r2
            r6inv = r2inv * r2inv * r2inv
            r12inv = r6inv * r6inv
            E += r12inv - r6inv
    return 4 * E

N = 8
if __name__ == '__main__':

    # The optimization problem is described in a korali Experiment object
    e = korali.Experiment()

    # Korali requires a specific interface for the function to maximize
    def objective_function(ksample):
        # note the minus sign because we want to minimize the lennard jones function
        ksample["F(x)"] = -lennard_jones(np.array(ksample["Parameters"]))

    e["Problem"]["Type"] = "Optimization"
    e["Problem"]["Objective Function"] = objective_function

    # Defining the problem's variables.
    for i in range(3*N):
        e["Variables"][i]["Name"] = f"x_{i}"
        e["Variables"][i]["Initial Value"] = np.random.uniform(-1, 1)
        e["Variables"][i]["Initial Standard Deviation"] = 0.5

    # Configuring CMA-ES parameters
    e["Solver"]["Type"] = "Optimizer/CMAES"
    e["Solver"]["Population Size"] = 7 * N * 3
    e["Solver"]["Termination Criteria"]["Min Value Difference Threshold"] = 1e-8
    e["Solver"]["Termination Criteria"]["Max Generations"] = 1000

    # Experiments need a Korali Engine object to be executed
    k = korali.Engine()

    # Run the optimization
    k.run(e)
