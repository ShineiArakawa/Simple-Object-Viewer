set(GLFW_INC lib/glfw/include)
set(GLFW_LIB lib/glfw/lib-vc2022)

include_directories(${GLFW_INC})
link_directories(${GLFW_LIB})

message("GLFW_INC: ${GLFW_INC}")
message("GLFW_LIB: ${GLFW_LIB}")
