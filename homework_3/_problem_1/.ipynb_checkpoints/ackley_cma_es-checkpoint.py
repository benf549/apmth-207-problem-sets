#!/usr/bin/env python3

import korali
import numpy as np

def rosenbrock(x, y):
    """
    The 2D Rosenbrock function
    https://en.wikipedia.org/wiki/Rosenbrock_function
    """
    a = 1
    b = 100
    return (a - x)**2 + b * (y - x**2)**2

def ackley(x, y):
    """
    2D Ackley function.
    """
    t1 = -20 * np.exp(-0.2 * np.sqrt(0.5 * (x**2 + y**2)))
    t2 = np.exp(0.5 * (np.cos(2*np.pi*x) + np.cos(2*np.pi*y)))
    t3 = 20 + np.e

    return t1 - t2 + t3


if __name__ == '__main__':

    # The optimization problem is described in a korali Experiment object
    e = korali.Experiment()

    # Korali requires a specific interface for the function to maximize
    def objective_function(ksample):
        x, y = ksample["Parameters"]
        # note the minus sign because we want to minimize the Rosenbrock function
        ksample["F(x)"] = -ackley(x, y)

    e["Problem"]["Type"] = "Optimization"
    e["Problem"]["Objective Function"] = objective_function

    # Defining the problem's variables.
    e["Variables"][0]["Name"] = "x"
    e["Variables"][0]["Initial Value"] = 5
    e["Variables"][0]["Initial Standard Deviation"] = 5

    e["Variables"][1]["Name"] = "y"
    e["Variables"][1]["Initial Value"] = 5
    e["Variables"][1]["Initial Standard Deviation"] = 5

    # Configuring CMA-ES parameters
    e["Solver"]["Type"] = "Optimizer/CMAES"
    e["Solver"]["Population Size"] = 16
    e["Solver"]["Termination Criteria"]["Min Value Difference Threshold"] = 1e-8
    e["Solver"]["Termination Criteria"]["Max Generations"] = 100

    # Experiments need a Korali Engine object to be executed
    k = korali.Engine()

    # Run the optimization
    k.run(e)
