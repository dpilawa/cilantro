import sys
import os

def AddBuildPath():
    project_root = os.path.dirname(os.path.abspath(__file__))
    build_path = os.path.join(project_root, "../build")
    if os.path.isdir(build_path):
        sys.path.append(build_path)
        print(f"Added build path: {build_path}")
    else:
        print("Build path not found.")