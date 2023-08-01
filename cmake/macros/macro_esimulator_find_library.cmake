MACRO(ESIMULATOR_FIND_LIBRARY _library_name)
  # Save a string representation of the arguments before cmake's
  # FIND_FILE gets its hands on it.
  TO_STRING(_str ${ARGN})


  FIND_LIBRARY(${_library_name} ${ARGN})

  IF(${_library_name} MATCHES "-NOTFOUND")
    MESSAGE(STATUS "${_library_name} not found! The call was:")
    MESSAGE(STATUS "    FIND_LIBRARY(${_library_name} ${_str})")
  ELSE()
    MESSAGE(STATUS "Found ${_library_name} : ${${_library_name}}")
  ENDIF()
ENDMACRO()
