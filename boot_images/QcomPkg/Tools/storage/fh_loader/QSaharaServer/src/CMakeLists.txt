cmake_minimum_required(VERSION 3.0)

set(CMAKE_BUILD_TYPE Debug)
set(CMake_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c")

project(QSaharaServer)


add_executable(
 QSaharaServer
 kickstart.c
 sahara_protocol.c
 usb_comm_lnx.c
 uart_comm_lnx.c
 comm.h
 common_protocol_defs.h
 external_utils.h
 external_utils.c
 kickstart_log.h
 kickstart_log.c
 kickstart_utils.h
 kickstart_utils.c
 sahara_protocol.h
 transport_protocol.h
 transport_protocol.c
)
