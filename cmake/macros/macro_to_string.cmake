MACRO(TO_STRING _variable)
  SET(${_variable} "")
  FOREACH(_var  ${ARGN})
    SET(${_variable} "${${_variable}} ${_var}")
  ENDFOREACH()
  STRING(STRIP "${${_variable}}" ${_variable})
ENDMACRO()

