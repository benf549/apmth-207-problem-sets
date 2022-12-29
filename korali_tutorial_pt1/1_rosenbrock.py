#!/usr/bin/env python3

import korali

def rosenbrock(x, y):
    """
    The 2D Rosenbrock function
    https://en.wikipedia.org/wiki/Rosenbrock_function
    """
    a = 1
    b = 100
    return (a - x)**2 + b * (y - x**2)**2


if __name__ == '__main__':

    # The optimization problem is described in a korali Experiment object
    e = korali.Experiment()

    # Korali requires a specific interface for the function to maximize
    def objective_function(ksample):
        x, y = ksample["Parameters"]
        # note the minus sign because we want to minimize the Rosenbrock function
        ksample["F(x)"] = -rosenbrock(x, y)

    e["Problem"]["Type"] = "Optimization"
    e["Problem"]["Objective Function"] = objective_function

    # Defining the problem's variables.
    e["Variables"][0]["Name"] = "x"
    e["Variables"][0]["Initial Value"] = 0.0
    e["Variables"][0]["Initial Standard Deviation"] = 1.0

    e["Variables"][1]["Name"] = "y"
    e["Variables"][1]["Initial Value"] = 0.0
    e["Variables"][1]["Initial Standard Deviation"] = 1.0

    # Configuring CMA-ES parameters
    e["Solver"]["Type"] = "Optimizer/CMAES"
    e["Solver"]["Population Size"] = 16
    e["Solver"]["Termination Criteria"]["Min Value Difference Threshold"] = 1e-8
    e["Solver"]["Termination Criteria"]["Max Generations"] = 100

    # Experiments need a Korali Engine object to be executed
    k = korali.Engine()

    # Run the optimization
    k.run(e)
