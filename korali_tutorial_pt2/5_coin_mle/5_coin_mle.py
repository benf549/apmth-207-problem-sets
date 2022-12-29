#!/usr/bin/env python3

import korali
import numpy as np


if __name__ == '__main__':

    # load the data
    # data = np.loadtxt('MLE_coin_A.txt')
    # data = np.loadtxt('MLE_coin_B.txt')
    data = np.loadtxt('MLE_coin_C.txt')
    N = len(data)
    R = int(data.sum())

    e = korali.Experiment()

    def objective_function(ksample):
        H = ksample["Parameters"][0]
        log_likelihood = R * np.log(H) + (N - R) * np.log(1 - H)
        ksample["F(x)"] = log_likelihood

    e["Problem"]["Type"] = "Optimization"
    e["Problem"]["Objective Function"] = objective_function

    # Defining the problem's variables.
    e["Variables"][0]["Name"] = "bias"
    e["Variables"][0]["Lower Bound"] = 0.0
    e["Variables"][0]["Upper Bound"] = 1.0

    # Configuring CMA-ES parameters
    e["Solver"]["Type"] = "Optimizer/CMAES"
    e["Solver"]["Population Size"] = 16
    e["Solver"]["Termination Criteria"]["Min Value Difference Threshold"] = 1e-9
    e["Solver"]["Termination Criteria"]["Max Generations"] = 100

    # Experiments need a Korali Engine object to be executed
    k = korali.Engine()

    # Run the optimization
    k.run(e)
