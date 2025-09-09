qt_add_qml_module(
  spotify
  URI
  Spotify
  VERSION
  1.0
  SOURCES
  authorization.cpp
  spotify-api.cpp
  playlist.cpp
  QML_FILES)

find_package(Qt6 REQUIRED COMPONENTS NetworkAuth Network Core Gui Quick)
find_package(Qt6Keychain REQUIRED)
target_link_libraries(spotify PRIVATE Qt6::NetworkAuth Qt6::Network
                                      Qt6Keychain::Qt6Keychain)

qt_query_qml_module(
  spotify
  URI
  module_uri
  VERSION
  module_version
  PLUGIN_TARGET
  module_plugin_target
  TARGET_PATH
  module_target_path
  QMLDIR
  module_qmldir
  TYPEINFO
  module_typeinfo
  QML_FILES
  module_qml_files
  RESOURCES
  module_resources)

set(module_dir "/usr/lib/qt6/qml/${module_target_path}")
# install c++ shared lib
install(
  TARGETS spotify
  LIBRARY DESTINATION "${module_dir}"
  RUNTIME DESTINATION "${module_dir}")
install(
  TARGETS "${module_plugin_target}"
  LIBRARY DESTINATION "${module_dir}"
  RUNTIME DESTINATION "${module_dir}")

# install qml stuff
install(FILES "${module_qmldir}" DESTINATION "${module_dir}")
install(FILES "${module_typeinfo}" DESTINATION "${module_dir}")

add_subdirectory(test)
