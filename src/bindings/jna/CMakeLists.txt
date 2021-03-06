find_program (MAVEN_EXECUTABLE mvn)
find_package (Java QUIET)

if (Java_JAVAC_EXECUTABLE)

	if (NOT Java_VERSION)
		exclude_binding (jna "unable to determine version of javac (java compiler)")
		return ()
	endif (NOT Java_VERSION)

	# ~~~
	# the version schemes are different on the jdks
	# on osx/oracle jdk its called java 9.0.1
	# osx/oracle jdk 8 is called java 1.8.x
	# on ubuntu/open jdk its called java 1.9.0
	# therefore this workaround should work in all cases
	# as 9.x.x > 1.8 and 1.8.x > 1.8 and 1.9.x > 1.8
	# ~~~
	if ((${Java_VERSION} VERSION_GREATER "1.8.0") OR (${Java_VERSION} VERSION_EQUAL "1.8.0"))
		if (NOT BUILD_SHARED)
			# See also: https://travis-ci.org/sanssecours/elektra/jobs/445840045
			exclude_binding (jna "it can only be built if `BUILD_SHARED` is enabled")
			return ()
		endif (NOT BUILD_SHARED)

		if (MAVEN_EXECUTABLE) # set by find_program
			add_binding (jna)

			file (MAKE_DIRECTORY libelektra4j)

			if (NOT APPLE)
				set (USE_SYSTEM_CLASS_LOADER "<useSystemClassLoader>false</useSystemClassLoader>")
			endif (NOT APPLE)

			# first fill the elektra version in the pom file @ONLY to avoid replacing pom.xml placeholders which also use the
			# format ${}
			configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/libelektra4j/pom.xml.in"
					"${CMAKE_CURRENT_BINARY_DIR}/libelektra4j/pom.xml" @ONLY)

			# required to have a recognizable name for the pom upon installation in case other software installs a pom to the
			# target directory and calls it pom.xml
			file (
				GENERATE
				OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/libelektra4j/libelektra4j-${KDB_VERSION}.pom.xml"
				INPUT "${CMAKE_CURRENT_BINARY_DIR}/libelektra4j/pom.xml")

			set (JNA_BINDING_PREFIX libelektra4j/src)
			set (JNA_BINDING_NAMESPACE java/org/libelektra)

			set (JNA_BINDING_SOURCE_DIRECTORY ${JNA_BINDING_PREFIX}/main/${JNA_BINDING_NAMESPACE})
			set (JNA_BINDING_SOURCE_DIRECTORY_PLUGIN ${JNA_BINDING_SOURCE_DIRECTORY}/plugin)
			set (JNA_BINDING_SOURCE_DIRECTORY_EXCEPTION ${JNA_BINDING_SOURCE_DIRECTORY}/exception)
			set (JNA_BINDING_SOURCE_DIRECTORY_TEST ${JNA_BINDING_PREFIX}/test/${JNA_BINDING_NAMESPACE})

			# The build system calls Maven to recompile the binding, if we change any of the following source files.
			set (
				JNA_BINDING_SOURCE_FILES
				${JNA_BINDING_SOURCE_DIRECTORY}/Elektra.java
				${JNA_BINDING_SOURCE_DIRECTORY}/KDB.java
				${JNA_BINDING_SOURCE_DIRECTORY}/Key.java
				${JNA_BINDING_SOURCE_DIRECTORY}/KeyNameIterator.java
				${JNA_BINDING_SOURCE_DIRECTORY}/KeySet.java
				${JNA_BINDING_SOURCE_DIRECTORY}/KeySetIterator.java
				${JNA_BINDING_SOURCE_DIRECTORY}/Plugin.java
				${JNA_BINDING_SOURCE_DIRECTORY}/PluginLoader.java
				${JNA_BINDING_SOURCE_DIRECTORY_PLUGIN}/NativePlugin.java
				${JNA_BINDING_SOURCE_DIRECTORY_PLUGIN}/Echo.java
				${JNA_BINDING_SOURCE_DIRECTORY_PLUGIN}/PropertiesStorage.java
				${JNA_BINDING_SOURCE_DIRECTORY_PLUGIN}/Return.java
				${JNA_BINDING_SOURCE_DIRECTORY_EXCEPTION}/ConflictingStateException.java
				${JNA_BINDING_SOURCE_DIRECTORY_EXCEPTION}/InstallationException.java
				${JNA_BINDING_SOURCE_DIRECTORY_EXCEPTION}/InterfaceException.java
				${JNA_BINDING_SOURCE_DIRECTORY_EXCEPTION}/InternalException.java
				${JNA_BINDING_SOURCE_DIRECTORY_EXCEPTION}/KDBException.java
				${JNA_BINDING_SOURCE_DIRECTORY_EXCEPTION}/LogicalException.java
				${JNA_BINDING_SOURCE_DIRECTORY_EXCEPTION}/OutOfMemoryException.java
				${JNA_BINDING_SOURCE_DIRECTORY_EXCEPTION}/PermanentException.java
				${JNA_BINDING_SOURCE_DIRECTORY_EXCEPTION}/PluginMisbehaviorException.java
				${JNA_BINDING_SOURCE_DIRECTORY_EXCEPTION}/ResourceException.java
				${JNA_BINDING_SOURCE_DIRECTORY_EXCEPTION}/SemanticValidationException.java
				${JNA_BINDING_SOURCE_DIRECTORY_EXCEPTION}/SyntacticValidationException.java
				${JNA_BINDING_SOURCE_DIRECTORY_EXCEPTION}/ValidationException.java
				${JNA_BINDING_SOURCE_DIRECTORY_EXCEPTION}/model/WarningEntry.java
				${JNA_BINDING_SOURCE_DIRECTORY_EXCEPTION}/mapper/ExceptionMapperService.java
				${JNA_BINDING_SOURCE_DIRECTORY_TEST}/AllTests.java
				${JNA_BINDING_SOURCE_DIRECTORY_TEST}/ExceptionMapperIT.java
				${JNA_BINDING_SOURCE_DIRECTORY_TEST}/PluginLoaderIT.java
				${JNA_BINDING_SOURCE_DIRECTORY_TEST}/exception/model/WarningEntryTest.java
				${JNA_BINDING_SOURCE_DIRECTORY_TEST}/ExceptionMapperTest.java
				${JNA_BINDING_SOURCE_DIRECTORY_TEST}/KDBExceptionTest.java
				${JNA_BINDING_SOURCE_DIRECTORY_TEST}/KDBTest.java
				${JNA_BINDING_SOURCE_DIRECTORY_TEST}/KeySetTest.java
				${JNA_BINDING_SOURCE_DIRECTORY_TEST}/KeyTest.java)

			# Compile the source files and build the jar. We copy the source files into the build folder as that is the easiest
			# way to handle the integration between CMake and Maven.
			add_custom_command (
				OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/libelektra4j/target/libelektra4j-${KDB_VERSION}.jar"
				COMMAND ${CMAKE_COMMAND} ARGS -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/libelektra4j"
					"${CMAKE_CURRENT_BINARY_DIR}/libelektra4j"
				COMMAND ${MAVEN_EXECUTABLE} -q -B -DskipTests=true package
				WORKING_DIRECTORY libelektra4j
				DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/libelektra4j/pom.xml" ${JNA_BINDING_SOURCE_FILES})

			create_lib_symlink (libelektra4j-${KDB_VERSION}.jar libelektra4j.jar JAVA)
			create_lib_symlink (libelektra4j-${KDB_VERSION}.pom.xml libelektra4j.pom.xml JAVA)

			add_custom_target (jna ALL
					   DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/libelektra4j/target/libelektra4j-${KDB_VERSION}.jar")

			install (FILES "${CMAKE_CURRENT_BINARY_DIR}/libelektra4j/target/libelektra4j-${KDB_VERSION}.jar"
				       "${CMAKE_CURRENT_BINARY_DIR}/libelektra4j/libelektra4j-${KDB_VERSION}.pom.xml"
				 DESTINATION "share/java")

			# and add maven test to execute with ctest in the testing phase
			if ((CMAKE_SYSTEM_NAME STREQUAL "Darwin") AND NOT (CMAKE_SYSTEM_VERSION VERSION_LESS 15))

				# we cannot set DYLD_LIBRARY_PATH on new macOS versions, making the kdb tests fail if its not installed in
				# the system
				set (MAVEN_TEST_ENV "env-test-without-kdb")
			else ()
				set (MAVEN_TEST_ENV "env-test")
			endif ()

			if (NOT ENABLE_ASAN)
				add_test (
					NAME testjna_maven
					COMMAND
						${MAVEN_EXECUTABLE} -q -B -e -P ${MAVEN_TEST_ENV} test
					WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/libelektra4j")

				set_property (TEST testjna_maven PROPERTY ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/lib")

				set_property (TEST testjna_maven PROPERTY LABELS kdbtests bindings memleak)
			endif (NOT ENABLE_ASAN)
		else (MAVEN_EXECUTABLE)
			exclude_binding (jna "Maven Executable not found, but required to build the jna bindings")
		endif (MAVEN_EXECUTABLE)
	else ((${Java_VERSION} VERSION_GREATER "1.8.0") OR (${Java_VERSION} VERSION_EQUAL "1.8.0"))
		exclude_binding (jna "At least Java 8 is required, but ${Java_VERSION_STRING} was found")
	endif ((${Java_VERSION} VERSION_GREATER "1.8.0") OR (${Java_VERSION} VERSION_EQUAL "1.8.0"))
else (Java_JAVAC_EXECUTABLE)
	exclude_binding (jna "javac (java compiler) not found, which is only included in a jdk")
endif (Java_JAVAC_EXECUTABLE)

mark_as_advanced (MAVEN_EXECUTABLE)
