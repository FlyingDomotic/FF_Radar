# This script deletes 3 obsolete files in ESPAsyncWebServer library
#   It's run by (pre) extra_script command in platformio.ini
import os
import inspect
import pathlib
Import("env")

thisScriptFullFileName = inspect.getouterframes(inspect.currentframe())[0].filename
thisScriptFileName = pathlib.Path(thisScriptFullFileName).name
thisScriptPath = pathlib.Path(thisScriptFullFileName).parent.resolve()

LIBDEPS_DIR = env['PROJECT_LIBDEPS_DIR']
LIBRARY_NAME = 'ESP Async WebServer'
LIBRARY_FOLDER = 'src'
FILE_TO_DELETE_LIST = ['SPIFFSEditor.h','SPIFFSEditor.cpp','edit.htm']

# For each file to delete
for fileToDelete in FILE_TO_DELETE_LIST:
    # For each folder in LIBDEPS_DIR (for the different targets)
        for folder in os.listdir(LIBDEPS_DIR):
            # Does file exist?
            fullFileName = os.path.join(LIBDEPS_DIR, folder, LIBRARY_NAME, LIBRARY_FOLDER, fileToDelete)
            if os.path.isfile(fullFileName):
                print(F'{thisScriptFileName}: Deleting {fullFileName}')
                os.remove(fullFileName)
