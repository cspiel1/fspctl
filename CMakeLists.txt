cmake_minimum_required(VERSION 3.18...4.0)

# set the project name
project(fspctl)

# set(CMAKE_BUILD_TYPE Debug)


option(WITH_MQTT "Build with MQTT" ON)


find_package(PkgConfig REQUIRED)

pkg_search_module(MODBUS REQUIRED libmodbus)
pkg_search_module(OPENSSL openssl)
pkg_search_module(MOSQUITTO libmosquitto)


# add the executable
add_executable(fspctl
	src/main.c
	src/fconf.c
	src/futil.c
	src/freg.c
	src/fenergy.c
)


target_include_directories(fspctl PRIVATE
	PUBLIC ${MODBUS_INCLUDE_DIRS}
)

target_link_libraries(fspctl PRIVATE
	${MODBUS_LIBRARIES}
)

if(MOSQUITTO_FOUND AND OPENSSL_FOUND AND WITH_MQTT)

target_sources(fspctl PRIVATE
	src/fmqtt.c
	)

target_compile_definitions(fspctl PRIVATE
	USE_MQTT
	)

	target_include_directories(fspctl PRIVATE
	${MOSQUITTO_INCLUDE_DIRS}
	${OPENSSL_INCLUDE_DIRS}

)

target_link_libraries(fspctl PRIVATE
	${MOSQUITTO_LIBRARIES}
	${OPENSSL_LIBRARIES}
)

ENDIF()
