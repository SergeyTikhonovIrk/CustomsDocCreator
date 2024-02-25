cmake_minimum_required(VERSION 3.22)
project(CustomsDocCreator)

set(CMAKE_CXX_STANDARD 20)
set(wxWidgets_ROOT_DIR E:/DevLibs/wxWidgets_3_28_1)
set(wxWidgets_LIB_DIR E:/DevLibs/wxWidgets_3_28_1/lib/gcc810_x64_dll)
set(CPACK_SOURCE_GENERATOR "ZIP")
find_package(wxWidgets REQUIRED COMPONENTS net core base)

add_subdirectory(libs/tinyxml)
add_subdirectory(libs/xlnt)

if (wxWidgets_USE_FILE) # not defined in CONFIG mode
    include(${wxWidgets_USE_FILE})
endif ()
add_executable(CustomsDocCreator src/main.cpp
        gui/Window.cpp
        gui/Window.h
        domain/Placeholder.cpp
        domain/Placeholder.h)
target_link_libraries(CustomsDocCreator ${wxWidgets_LIBRARIES} tinyxml2 xlnt)
install(TARGETS CustomsDocCreator DESTINATION bin)
if (CMAKE_BUILD_TYPE MATCHES Release)
    install(FILES E:/DevLibs/wxWidgets_3_28_1/lib/gcc810_x64_dll/wxbase32u_gcc810_x64.dll
            RUNTIME DESTINATION bin COMPONENT RuntimeLibraries)
    install(FILES bin E:/DevLibs/wxWidgets_3_28_1/lib/gcc810_x64_dll/wxmsw32u_core_gcc810_x64.dll
            RUNTIME DESTINATION bin COMPONENT RuntimeLibraries)
else ()
    install(FILES bin E:/DevLibs/wxWidgets_3_28_1/lib/gcc810_x64_dll/wxbase32ud_gcc810_x64.dll
            RUNTIME DESTINATION bin COMPONENT RuntimeLibraries)
    install(FILES bin E:/DevLibs/wxWidgets_3_28_1/lib/gcc810_x64_dll/wxmsw32ud_core_gcc810_x64.dl
            RUNTIME DESTINATION bin COMPONENT RuntimeLibraries)
endif ()
include(InstallRequiredSystemLibraries)
include(CPack)


