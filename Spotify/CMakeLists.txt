qt_add_qml_module(
  spotify
  URI
  Spotify
  VERSION
  1.0
  SOURCES
  authorization.cpp
  spotify-api.cpp
  QML_FILES)

find_package(Qt6 REQUIRED COMPONENTS NetworkAuth Network Core Gui Quick)
target_link_libraries(spotify PRIVATE Qt6::NetworkAuth Qt6::Network)

add_subdirectory(test)
