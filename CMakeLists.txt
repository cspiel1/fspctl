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

link_directories(
	${MODBUS_LIBRARY_DIRS}
)

include_directories(
	${MODBUS_INCLUDE_DIRS}
)

target_link_libraries(fspctl
	${MODBUS_LIBRARIES}
)
