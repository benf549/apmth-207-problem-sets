#!/usr/bin/env python3

import json, os
import numpy as np

with open(os.path.join("_korali_result", "latest"), "r") as f:
    j = json.load(f)
    samples = np.array(j["Results"]["Sample Database"])
    # 90041

print(samples)
