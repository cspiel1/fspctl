cmake_minimum_required(VERSION 3.10)

# set the project name
project(fspctl)

set(CMAKE_BUILD_TYPE Debug)


option(WITH_MQTT "Build with MQTT" ON)


find_package(PkgConfig REQUIRED)

pkg_check_modules(MODBUS REQUIRED libmodbus)
pkg_check_modules(OPENSSL openssl)
pkg_check_modules(MOSQUITTO libmosquitto)


# add the executable
add_executable(fspctl
	src/main.c
	src/fconf.c
	src/futil.c
	src/freg.c
	src/fenergy.c
)


target_link_directories(fspctl
	PUBLIC ${MODBUS_LIBRARY_DIRS}
)

target_include_directories(fspctl AFTER
	PUBLIC ${MODBUS_INCLUDE_DIRS}
)

target_link_libraries(fspctl
	${MODBUS_LIBRARIES}
)

target_sources(fspctl PRIVATE
	src/fmqtt.c
	)

IF (${MOSQUITTO_FOUND} AND ${OPENSSL_FOUND} AND ${WITH_MQTT})

target_compile_definitions(fspctl PRIVATE
	USE_MQTT
	)

target_link_directories(fspctl
	PUBLIC ${MOSQUITTO_LIBRARY_DIRS}
	${OPENSSL_LIBRARY_DIRS}
)

target_include_directories(fspctl AFTER
	PUBLIC ${MOSQUITTO_INCLUDE_DIRS}
	${OPENSSL_INCLUDE_DIRS}

)

target_link_libraries(fspctl
	${MOSQUITTO_LIBRARIES}
	${OPENSSL_LIBRARIES}
)

ENDIF()
