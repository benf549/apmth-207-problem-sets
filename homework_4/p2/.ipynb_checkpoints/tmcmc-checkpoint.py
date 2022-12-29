#!/usr/bin/env python3

import korali
import numpy as np
import pandas as pd


if __name__ == '__main__':

    data = pd.read_csv('data.csv')
    n = len(data.x)

    def log_likelihood(ks):
        a, b = ks["Parameters"]
        t3 = np.nan_to_num([ np.log((a - x)**2 - (b**2)) for x in data.x ], nan=1e10)
        log_like = (-n * np.log(np.pi)) + (n * np.log(b)) - sum(t3)
        ks["logLikelihood"] = log_like


    # The optimization problem is described in a korali Experiment object
    e = korali.Experiment()

    # Loading previous run (if exist)
    e["File Output"]["Path"] = "_result_korali"
    found = e.loadState('_result_korali/latest')
    print(found)

    e["Problem"]["Type"] = "Bayesian/Custom"
    e["Problem"]["Likelihood Model"] = log_likelihood


    e["Solver"]["Type"] = "Sampler/TMCMC"
    e["Solver"]["Population Size"] = 5000
    # e["Solver"]["Target Coefficient of Variation"] = 1.0
    e["Solver"]["Covariance Scaling"] = 0.2


    e["Distributions"] = [
           { "Name" : "Prior a",
             "Type" : "Univariate/Uniform",
             "Minimum": 1e-3,
             "Maximum": 100 },
           { "Name": "Prior b",
             "Type" : "Univariate/Uniform",
             "Minimum": 1e-3,
             "Maximum": 100 },
    ]

    e["Variables"] = [
            {"Name": "a", "Prior Distribution": "Prior a"},
            {"Name": "b", "Prior Distribution": "Prior b"},
    ]

    e["Store Sample Information"] = True
    e["Console Output"]["Verbosity"] = "Detailed"

    k = korali.Engine()
    k.run(e)
