#!/usr/bin/env python3

import korali
import numpy as np
import sympy as sp

def negative_ackley(x, y):
    """
    2D Ackley function.
    """
    t1 = -20 * np.exp(-0.2 * np.sqrt(0.5 * (x**2 + y**2)))
    t2 = np.exp(0.5 * (np.cos(2*np.pi*x) + np.cos(2*np.pi*y)))
    t3 = 20 + np.e

    return -1 * (t1 - t2 + t3)

def sym_negative_ackley(x, y):
    """
    2D Ackley function.
    """
    t1 = -20 * sp.exp(-0.2 * sp.sqrt(0.5 * (x**2 + y**2)))
    t2 = sp.exp(0.5 * (sp.cos(2*sp.pi*x) + sp.cos(2*sp.pi*y)))
    t3 = 20 + sp.exp(1)

    return -1 * (t1 - t2 + t3)

def evaluate_ackley(xx, yy):
    x = sp.Symbol('x')
    y = sp.Symbol('y')

    sa = sym_negative_ackley(x, y)
    dack_dx = sp.diff(sa, x)
    dack_dy = sp.diff(sa, y)

    f_of_x_y = negative_ackley(xx,yy)
    gradient = [float(k.subs(x, xx).subs(y, yy).evalf()) for k in [dack_dx, dack_dy]]
    return f_of_x_y, gradient

if __name__ == '__main__':

    # The optimization problem is described in a korali Experiment object
    e = korali.Experiment()

    # Korali requires a specific interface for the function to maximize
    def objective_function(ksample):
        x, y = ksample["Parameters"]
        f, grad = evaluate_ackley(x, y)
        ksample["F(x)"] = f
        ksample["Gradient"] = grad

    e["Problem"]["Type"] = "Optimization"
    e["Problem"]["Objective Function"] = objective_function

    # Defining the problem's variables.
    e["Variables"][0]["Name"] = "x"
    e["Variables"][0]["Initial Value"] = 5.0

    e["Variables"][1]["Name"] = "y"
    e["Variables"][1]["Initial Value"] = 5.0

    # Configuring Adam parameters
    e["Solver"]["Type"] = "Optimizer/Adam"
    e["Solver"]["Eta"] = 0.40
    e["Solver"]["Termination Criteria"]["Max Generations"] = 100

    # Configuring results path
    e["Console Output"]["Frequency"] = 10
    e["File Output"]["Frequency"] = 10

    # Experiments need a Korali Engine object to be executed
    k = korali.Engine()

    # Run the optimization
    k.run(e)
