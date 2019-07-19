include(CMakeFindDependencyMacro)

# Same syntax as find_package
find_dependency(OpenCV REQUIRED)
find_dependency(Git REQUIRED)
# for testing
find_dependency(fmt)

# Add the targets file
include("${CMAKE_CURRENT_LIST_DIR}/NotQRCodeTargets.cmake")