import pathlib
import inspect
Import("env")

thisScriptFullFileName = inspect.getouterframes(inspect.currentframe())[0].filename
thisScriptFileName = pathlib.Path(thisScriptFullFileName).name
thisScriptPath = pathlib.Path(thisScriptFullFileName).parent.resolve()

# Name firmware after current environment
env.Replace(PROGNAME="%s" % str(env['PIOENV']))

# Change name to "PROG_NAME" item of BUILD_FLAGS if it exists
my_flags = env.ParseFlags(env['BUILD_FLAGS'])
for x in my_flags.get("CPPDEFINES"):
    #print(x)
    if isinstance(x, list):
        k, v = x
        if k == "PROG_NAME":
            # Set PROGNAME to given value
            env.Replace(PROGNAME="%s" % str(v))
            break

print(F"{thisScriptFileName}: Changing firmware name to {env['PROGNAME']}")
