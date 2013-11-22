# Include the directory itself as a path to include directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Create a variable containing all header files
FILE(GLOB h_files *.hpp)

INSTALL(FILES ${h_files}
  DESTINATION include/Common/xdr COMPONENT sdk
)
