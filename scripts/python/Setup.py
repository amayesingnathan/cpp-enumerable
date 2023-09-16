import sys
import os
import subprocess
import platform

from PremakeSetup import PremakeConfiguration as PremakeRequirements
from ProjectSetup import ProjectConfiguration as ProjectConfig

os.chdir(f"{sys.path[0]}/../..") # Change working dir to repo root
projectConfigured = ProjectConfig.CheckProjectConfig()
premakeInstalled = PremakeRequirements.Validate()

if (not projectConfigured):
    namespace = str(input("Enter the top level name for the repo...\n")).strip()
    ProjectConfig.SetupNamespace(namespace)

if (not projectConfigured):
    projectName = str(input("Enter the name for the template project...\n")).strip()
    ProjectConfig.SetupProject(projectName)

if (premakeInstalled):
    print("\nRunning premake...")
    if platform.system() == "Windows":
        subprocess.call([os.path.abspath("./scripts/gen-projects/msvc.bat"), "nopause"])
    elif platform.system() == "Linux":
        subprocess.call([os.path.abspath("./scripts/gen-projects/gcc.sh"), "nopause"])

    print("\nSetup completed!")
else:
    print("Labyrinth requires Premake to generate project files.")