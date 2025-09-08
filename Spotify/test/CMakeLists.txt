qt_add_executable(testqml main.cpp)

target_link_libraries(testqml PRIVATE Qt6::Quick Qt6::Core Qt6::NetworkAuth
                                      Qt6::Gui spotify)

qt_add_qml_module(
  testqml
  URI
  test
  VERSION
  1.0
  QML_FILES
  main.qml)
