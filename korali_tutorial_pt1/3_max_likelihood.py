#!/usr/bin/env python3

import korali
import numpy as np
import pandas as pd


if __name__ == '__main__':

    # load the data
    df = pd.read_csv("data.csv")
    t = df["t"].to_numpy()
    z = df["z"].to_numpy()


    e = korali.Experiment()

    def objective_function(ksample):
        z0, G, sigma = ksample["Parameters"]

        # predictions with the model
        zth = z0 - G/2 * t**2

        log_likelihood = -np.sum((z-zth)**2 / (2*sigma**2)) - len(t) * np.log(sigma)
        ksample["F(x)"] = log_likelihood

    e["Problem"]["Type"] = "Optimization"
    e["Problem"]["Objective Function"] = objective_function

    # Defining the problem's variables.
    e["Variables"][0]["Name"] = "z0"
    e["Variables"][0]["Lower Bound"] = 0.0
    e["Variables"][0]["Upper Bound"] = 2.0

    e["Variables"][1]["Name"] = "G"
    e["Variables"][1]["Lower Bound"] = 5.0
    e["Variables"][1]["Upper Bound"] = 15.0

    e["Variables"][2]["Name"] = "sigma"
    e["Variables"][2]["Lower Bound"] = 0.01
    e["Variables"][2]["Upper Bound"] = 0.1

    # Configuring CMA-ES parameters
    e["Solver"]["Type"] = "Optimizer/CMAES"
    e["Solver"]["Population Size"] = 16
    e["Solver"]["Termination Criteria"]["Min Value Difference Threshold"] = 1e-9
    e["Solver"]["Termination Criteria"]["Max Generations"] = 100

    # Experiments need a Korali Engine object to be executed
    k = korali.Engine()

    # Run the optimization
    k.run(e)
