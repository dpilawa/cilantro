import sys
import os

# Add the build directory to the Python path
sys.path.append(os.path.join(os.path.dirname(__file__), "..\\build\\Debug"))

import pycilantro

pycilantro.CGame.Initialize()
pycilantro.CGame.CreateGameScene("scene")



