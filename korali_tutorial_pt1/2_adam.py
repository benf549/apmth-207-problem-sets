#!/usr/bin/env python3

import korali

def negative_rosenbrock(x, y):
    """
    The 2D Rosenbrock function
    https://en.wikipedia.org/wiki/Rosenbrock_function
    """
    a = 1
    b = 100
    f = - (a - x)**2 - b * (y - x**2)**2
    grad = [2 * (a-x) + b * 4 * x * (y - x**2),
            -2 * b * (y - x**2)]
    return f, grad


if __name__ == '__main__':

    # The optimization problem is described in a korali Experiment object
    e = korali.Experiment()

    # Korali requires a specific interface for the function to maximize
    def objective_function(ksample):
        x, y = ksample["Parameters"]
        f, grad = negative_rosenbrock(x, y)
        ksample["F(x)"] = f
        ksample["Gradient"] = grad

    e["Problem"]["Type"] = "Optimization"
    e["Problem"]["Objective Function"] = objective_function

    # Defining the problem's variables.
    e["Variables"][0]["Name"] = "x"
    e["Variables"][0]["Initial Value"] = 0.0

    e["Variables"][1]["Name"] = "y"
    e["Variables"][1]["Initial Value"] = 0.0

    # Configuring Adam parameters
    e["Solver"]["Type"] = "Optimizer/Adam"
    e["Solver"]["Eta"] = 0.1
    e["Solver"]["Termination Criteria"]["Max Generations"] = 5000

    # Configuring results path
    e["Console Output"]["Frequency"] = 250
    e["File Output"]["Frequency"] = 250

    # Experiments need a Korali Engine object to be executed
    k = korali.Engine()

    # Run the optimization
    k.run(e)
