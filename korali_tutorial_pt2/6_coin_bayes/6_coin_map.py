#!/usr/bin/env python3

import korali
import numpy as np


def likelihood_model(s, data):
    N = len(data)
    R = int(data.sum())
    H = s['Parameters'][0]

    s['logLikelihood'] = R * np.log(H) + (N - R) * np.log(1 - H)


if __name__ == '__main__':

    # load the data
    # data = np.loadtxt('MAP_coin_A.txt')
    # data = np.loadtxt('MAP_coin_B.txt')
    # data = np.loadtxt('MAP_coin_C.txt')
    data = np.loadtxt('MAP_coin_D.txt')

    e = korali.Experiment()

    # Configure the problem, specifying the likelihood model
    e["Problem"]["Type"] = "Bayesian/Custom"
    e["Problem"]["Likelihood Model"] = lambda x: likelihood_model(x, data)

    # Setup prior distribution
    e["Distributions"][0]["Name"] = 'Uniform 0'
    e["Distributions"][0]["Type"] = "Univariate/Uniform"
    e["Distributions"][0]["Minimum"] = +0.0
    e["Distributions"][0]["Maximum"] = +1.0

    # Defining the problem's variables.
    e["Variables"][0]["Name"] = "bias"
    e["Variables"][0]["Prior Distribution"] = "Uniform 0"  # Note: The name here matches the name we gave our prior'
    e["Variables"][0]["Initial Mean"] = +0.5
    e["Variables"][0]["Initial Standard Deviation"] = +0.5
    e["Variables"][0]["Initial Value"] = +0.5
    e["Variables"][0]["Upper Bound"] = +1.0
    e["Variables"][0]["Lower Bound"] = +0.0

    # Configuring the adaptive MH sampler parameters
    e["Solver"]["Type"] = "Sampler/MCMC"
    e["Solver"]["Burn In"] = 1_000
    e["Solver"]["Use Adaptive Sampling"] = True

    # Configuring output settings
    e["File Output"]["Frequency"] = 500
    e["Console Output"]["Frequency"] = 500
    e["Console Output"]["Verbosity"] = "Detailed"

    # Experiments need a Korali Engine object to be executed
    k = korali.Engine()

    # Run the optimization
    k.run(e)
