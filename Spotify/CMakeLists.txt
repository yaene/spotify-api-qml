qt_add_qml_module(
  spotify
  URI
  Spotify
  VERSION
  1.0
  SOURCES
  authorization.cpp
  QML_FILES)

find_package(Qt6 REQUIRED COMPONENTS NetworkAuth Network Core Gui)
target_link_libraries(spotify PRIVATE Qt6::NetworkAuth Qt6::Network)

qt_add_executable(test main.cpp)

target_link_libraries(test PRIVATE Qt6::Core Qt6::NetworkAuth Qt6::Gui spotify)
