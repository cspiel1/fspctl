cmake_minimum_required(VERSION 3.10)

# set the project name
project(fspctl)

set(CMAKE_BUILD_TYPE Debug)

find_package(PkgConfig REQUIRED)

pkg_check_modules(MODBUS REQUIRED libmodbus)

# add the executable
add_executable(fspctl
	src/main.c
	src/fconf.c
	src/futil.c
	src/freg.c
)

message("MODBUS_LIBRARY_DIRS=${MODBUS_LIBRARY_DIRS}")

target_link_directories(fspctl
	PUBLIC ${MODBUS_LIBRARY_DIRS}
)

target_include_directories(fspctl AFTER
	PUBLIC ${MODBUS_INCLUDE_DIRS}
)

target_link_libraries(fspctl
	${MODBUS_LIBRARIES}
)
