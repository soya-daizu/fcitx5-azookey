cmake_minimum_required(VERSION 3.21)
project(fcitx5-azookey VERSION 0.1.0)

find_package(Fcitx5Core REQUIRED)
find_package(Gettext REQUIRED)
# Setup some compiler option that is generally useful and compatible with Fcitx 5 (C++17)
include("${FCITX_INSTALL_CMAKECONFIG_DIR}/Fcitx5Utils/Fcitx5CompilerSettings.cmake")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}")
message(STATUS "FCITX_INSTALL_CMAKECONFIG_DIR ${FCITX_INSTALL_CMAKECONFIG_DIR}")

add_subdirectory(src)
add_subdirectory(po)

fcitx5_translate_desktop_file(org.fcitx.Fcitx5.Addon.AzooKey.metainfo.xml.in
                              org.fcitx.Fcitx5.Addon.AzooKey.metainfo.xml XML
                              PO_DIRECTORY "${CMAKE_SOURCE_DIR}/po")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/org.fcitx.Fcitx5.Addon.AzooKey.metainfo.xml" DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)
