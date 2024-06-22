# Set the path to the Swift library
set(SWIFT_LIBRARY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../.build/release")
find_library(SWIFT_LIBRARY NAMES Fcitx5BridgeModule PATHS ${SWIFT_LIBRARY_PATH})
# Include the bridging header directory

set(AZOOKEY_SOURCES
  AzookeyEngine.cpp
  AzookeyState.cpp
  AzookeyCandidateList.cpp
)

add_library(azookey SHARED ${AZOOKEY_SOURCES})
target_link_libraries(azookey PRIVATE Fcitx5::Core ${SWIFT_LIBRARY})
target_include_directories(azookey PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/Fcitx5BridgeModule")
set_target_properties(azookey PROPERTIES PREFIX "")
install(TARGETS azookey DESTINATION "${FCITX_INSTALL_LIBDIR}/fcitx5")

configure_file(azookey-addon.conf.in.in azookey-addon.conf.in)
fcitx5_translate_desktop_file("${CMAKE_CURRENT_BINARY_DIR}/azookey-addon.conf.in" azookey-addon.conf PO_DIRECTORY "${CMAKE_SOURCE_DIR}/po")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/azookey-addon.conf" RENAME azookey.conf DESTINATION "${FCITX_INSTALL_PKGDATADIR}/addon" COMPONENT config)

configure_file(azookey.conf.in.in azookey.conf.in)
fcitx5_translate_desktop_file("${CMAKE_CURRENT_BINARY_DIR}/azookey.conf.in" azookey.conf PO_DIRECTORY "${CMAKE_SOURCE_DIR}/po")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/azookey.conf" DESTINATION "${FCITX_INSTALL_PKGDATADIR}/inputmethod" COMPONENT config)
