include(CMakeFindDependencyMacro)

# Same syntax as find_package
find_dependency(fmt REQUIRED)
find_dependency(OpenCV REQUIRED)

# Add the targets file
include("${CMAKE_CURRENT_LIST_DIR}/ToyDecoderTargets.cmake")