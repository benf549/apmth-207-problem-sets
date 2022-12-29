#!/usr/bin/env python3

import json, os
import numpy as np

with open(os.path.join("_result_korali", "gen00000012.json"), "r") as f:
    j = json.load(f)
    samples = np.array(j["Solver"]["Chain Candidates"])
    # 90041

print(samples)
