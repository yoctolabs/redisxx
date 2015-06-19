#################################################
#
# SCons building routine for the RedisXX project
#
# header-only library
#
### Handled targets:
#   test
#   coverage
#   //install
#
#################################################
import os
from os import path,walk

Import("env", "compilation_mode", "package_name", "deps_runtime", "deps_build", "deps_test");

### Directory architecture configuration
conf_testdir    = "test_suite";

# gentoo building package support
conf_portage    = ARGUMENTS.get("PORTAGE", "no");
conf_libdir     = ARGUMENTS.get("LIBDIR", "");

conf_sandbox    = ARGUMENTS.get("DESTDIR", "sandbox");
if (conf_portage != "yes"):
	conf_sandbox = os.sep;

conf_installdir = path.join("usr", "local");
if ( len(conf_libdir) > 1 ):
	conf_installdir = path.join(conf_sandbox, conf_libdir[1:])
else:
	conf_installdir = path.join(conf_sandbox, conf_installdir)


conf_installdir_lib = path.join(conf_installdir, "lib")
conf_installdir_inc = path.join(conf_installdir, "include")

### Environment
env.Append(CPPPATH = ["include", conf_installdir_inc])

### Routine for gathering files
def getSuffixedFiles(dirpath, filesuffix):
	dirpath  = path.join('..', dirpath);
	files    = [];
	for (dirpath, dirnames, filenames) in walk(dirpath):
		files += Glob(path.join(dirpath, filesuffix));
	return files;


### TEST target
if ("test" in COMMAND_LINE_TARGETS) | ("coverage" in COMMAND_LINE_TARGETS):
	testenv = env.Clone()
	testenv.Append(CPPFLAGS = ["-DTEST", "-pthread"])

	cxxfiles = getSuffixedFiles(conf_testdir, "*.cpp")
	if (cxxfiles != []):
		# For shared libraries
		# test_libs = [package_name, "boost_unit_test_framework"]
		boost_socket_deps = ["boost_system", "boost_thread"] # including unix domain socket
		sfml_socket_deps = ["sfml-system", "sfml-network"]
		
		test_libs = ["boost_unit_test_framework"]
		test_libs.extend(boost_socket_deps)
		test_libs.extend(sfml_socket_deps)
		test_libs.append(deps_test);

		# attach program to test target
		prog_test = testenv.Program(target = path.join(conf_testdir, "test_" + package_name), source = cxxfiles, LIBS = test_libs, LIBPATH=[conf_installdir_lib]);
		testenv.Alias("test", [prog_test]);

### COVERAGE target
if ("coverage" in COMMAND_LINE_TARGETS):
	testenv.Alias("coverage", [conf_testdir])
