# CMake project to compile Unplugged-RT remote SDK examples

cmake_minimum_required (VERSION 3.4)
project (commsignia-upl-remote-sdk-examples)
set (CMAKE_BUILD_TYPE Debug)

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

set (CMS_V2X_SDK_DIR ${PROJECT_SOURCE_DIR})
set (CMS_V2X_EXAMPLES_DIR ${CMS_V2X_SDK_DIR}/examples)

include (${CMS_V2X_SDK_DIR}/cms_v2x_sdk.cmake)
include (${CMS_V2X_EXAMPLES_DIR}/cms_v2x_sdk_examples.cmake)
