import sys
import os
import platform
from unicodedata import name
import requests
from pathlib import Path

def ReplaceNamespace(namespace):
    with open('premake5.lua', 'r') as premakeRoot:
        filedata = premakeRoot.read()
    filedata = filedata.replace('cpp-template', namespace)
    with open('premake5.lua', 'w') as premakeRoot:
        premakeRoot.write(filedata)

def ReplaceProjectName(projectName):
    with open('premake5.lua', 'r') as premakeRoot:
        filedata = premakeRoot.read()
    filedata = filedata.replace('TemplateProject', projectName)
    with open('premake5.lua', 'w') as premakeRoot:
        premakeRoot.write(filedata)
        
    with open('{name}/premake5.lua'.format(name=projectName), 'r') as premakeProj:
        filedata = premakeProj.read()
    filedata = filedata.replace('TemplateProject', projectName)
    with open('{name}/premake5.lua'.format(name=projectName), 'w') as premakeProj:
        premakeProj.write(filedata)
        
def ReplaceProjectType(projectType):
    match projectType:
        case 'l':
            winProjType = 'StaticLib'
            linuxProjType = 'SharedLib'
        case 'e':
            winProjType = 'ConsoleApp'
            linuxProjType = 'ConsoleApp'
        case _:
            return

    with open('TemplateProject/premake5.lua', 'r') as premakeProj:
        filedata = premakeProj.read()
    filedata = filedata.replace('ProjectTypeLinux', linuxProjType)
    filedata = filedata.replace('ProjectTypeWin', winProjType)
    with open('TemplateProject/premake5.lua', 'w') as premakeProj:
        premakeProj.write(filedata)
    
class ProjectConfiguration:

    @classmethod
    def CheckProjectConfig(cls):
        return not os.path.isdir('TemplateProject')

    @classmethod
    def SetupNamespace(cls, namespace):
        ReplaceNamespace(namespace)
        
    @classmethod
    def SetupProject(cls, projectName):
        projectTypeSetup = False
        projectType = ''
        while not projectTypeSetup:
            reply = str(input("Please choose binary type of project, executable or library? [E/L]: ")).lower().strip()[:1]
            projectTypeSetup = (reply == 'e' or reply == 'l')
            projectType = reply
            
        ReplaceProjectType(projectType)
        
        os.rename("TemplateProject", projectName)
        ReplaceProjectName(projectName)