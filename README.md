# cpp-template

Basic C++ project template using premake5.

## Description

This is a basic template for a C++ project, using premake as a build system. The template comes with basic premake template projects, and when the 
set up script is run premake will be downloaded and project files will be generated if there have been any changes to the premake or source files.

The first time that the setup script is run, the user is prompted to supply the name for the workspace and starting template project. 
The repo directory will still need to be renamed manually as sometimes the permissions for this can be restricted.

## Getting Started

### Dependencies

* Python3
* premake5 - Will be downloaded and extracted as a binary when the setup script is run if it is not found.
* Currently only supports Windows and Linux (Ubuntu tested).

### Installing

* Clone the repo and run `setup.bat` for Windows or `setup.sh` for Linux. 
* Enter the name for the solution and the starting project.
* Project files will be generated for the target platform to use.
* Extend the project as desired!

## License

This project is licensed under the MIT License - see the LICENSE.md file for details