cmake_minimum_required(VERSION 3.16)

project(GeoCatch VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing(true)
# Find required Qt6 components
find_package(Qt6 6.5 REQUIRED COMPONENTS Quick Widgets Core5Compat Sql Test)

# Standard project setup for Qt6
qt_standard_project_setup(REQUIRES 6.5)

# Define the executable
qt_add_executable(appGeoCatch
    main.cpp
)

# Add the QML module
qt_add_qml_module(appGeoCatch
    URI GeoCatch
    VERSION 1.0
    QML_FILES
        Main.qml
    RESOURCES
        qml.qrc
        SOURCES databaseManager.cpp
        SOURCES databaseManager.h
        SOURCES networkManager.h
        SOURCES networkManager.cpp
        SOURCES validator.h
        SOURCES validator.cpp
        SOURCES networkManagerTest.cpp
        # SOURCES networkManagerTest.cpp
)

qt_add_resources(appGeoCatch "resources"
    PREFIX "/"
    FILES
        resources/online.png
        resources/offline.png
        resources/globe.png
)

# prepared for QTest
# add_executable(network_manager_tests networkManagerTest.cpp)

# target_link_libraries(network_manager_tests PRIVATE Qt6::Core Qt6::Network Qt6::Test)
# add_test(NAME NetworkManagerTests COMMAND network_manager_tests)

# Set target properties
set_target_properties(appGeoCatch PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    SKIP_AUTOMOC ON
)


# Link the executable with Qt6 libraries
target_link_libraries(appGeoCatch
    PRIVATE Qt6::Quick
    PRIVATE Qt6::Widgets
    PRIVATE Qt6::Core5Compat
    PRIVATE Qt6::Sql
    PRIVATE Qt6::Test
)

# Installation rules
include(GNUInstallDirs)
install(TARGETS appGeoCatch
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
