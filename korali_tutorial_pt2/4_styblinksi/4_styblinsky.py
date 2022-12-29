#!/usr/bin/env python3

import korali

def stybtang(x, y, z):
    """
    The 3D Styblinski-Tang function.
    See: https://www.sfu.ca/~ssurjano/stybtang.html
    """
    result = 0
    for i in [x, y, z]:
        result += (i**4) - (16 * (i**2)) + (5*i) 

    result = result / 2

    return result


if __name__ == '__main__':

    # The optimization problem is described in a korali Experiment object
    e = korali.Experiment()

    # Korali requires a specific interface for the function to maximize
    def objective_function(ksample):
        x, y, z = ksample["Parameters"]

        # note the minus sign because we want to minimize the function
        ksample["F(x)"] = -stybtang(x, y, z)

    e["Problem"]["Type"] = "Optimization"
    e["Problem"]["Objective Function"] = objective_function

    # Defining the problem's variables.
    e["Variables"][0]["Name"] = "x"
    e["Variables"][0]["Initial Value"] = 0.0
    e["Variables"][0]["Initial Standard Deviation"] = 1.0

    e["Variables"][1]["Name"] = "y"
    e["Variables"][1]["Initial Value"] = 0.0
    e["Variables"][1]["Initial Standard Deviation"] = 1.0

    e["Variables"][2]["Name"] = "z"
    e["Variables"][2]["Initial Value"] = 0.0
    e["Variables"][2]["Initial Standard Deviation"] = 1.0

    # Configuring CMA-ES parameters
    e["Solver"]["Type"] = "Optimizer/CMAES"
    e["Solver"]["Population Size"] = 32
    e["Solver"]["Termination Criteria"]["Min Value Difference Threshold"] = 1e-8
    e["Solver"]["Termination Criteria"]["Max Generations"] = 100

    # Experiments need a Korali Engine object to be executed
    k = korali.Engine()

    # Run the optimization
    k.run(e)
