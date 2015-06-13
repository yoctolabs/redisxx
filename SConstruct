#################################################
# Build System with SCONS
#
# SConstruct: main file used by scons by default
# SConscript: general routines 
#
# usage:
#
### To build the tests in release mode
#   scons test
### To build the tests in debug mode
#   scons test MODE=debug
### To build the binary files from main_suite/
#   scons 
#
#################################################

package_name     = "redisxx";
deps_runtime = [''];
deps_build   = [''];
deps_test    = deps_build;

#################################################

env = Environment();
env.Decider("MD5");

compilation_mode = ARGUMENTS.get("MODE", "release");


# Setting options for the selected compilation mode (release | debug)
if (compilation_mode == "DEBUG") | (compilation_mode == "debug"):
	# If debug mode is activated the macro DEBUG is made available
	env.Append(CPPFLAGS = ["-std=c++11", "-pipe", "-pedantic", "-Wall", "-g3", "-ggdb3", "-gstabs3", "-DDEBUG"]);
else:
	env.Append(CPPFLAGS = ["-std=c++11", "-pipe", "-pedantic", "-Wall", "-O3"]);

# Building available targets
# 
env.SConscript("SConscript", exports = ["env", "compilation_mode", "package_name", "deps_runtime", "deps_build", "deps_test"], variant_dir = "build");
env.Clean(".", "build");

