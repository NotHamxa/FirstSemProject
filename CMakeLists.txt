cmake_minimum_required(VERSION 4.0)
project(AudioEditor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Multimedia
        REQUIRED)

add_executable(AudioEditor
        main.cpp
        mainWindow.cpp
        mainWindow.h
        pages/homePage.cpp
        pages/homePage.h
        structures/appData.h
        util/readAudioFile.cpp
        util/readAudioFile.h
        pages/mainPage.cpp
        pages/mainPage.h
        components/waveFormCanvas.cpp
        components/waveFormCanvas.h
        util/audioBufferDevice.h
        util/audioPlayer.h
        pages/eqPage.h
        pages/eqPage.cpp
        util/eqEditor.h
        util/eqEditor.cpp
        util/biquad.h
        structures/history.h
)

target_include_directories(AudioEditor PRIVATE
        "C:/hamza/c++/lib/libsndfile/include"
)

target_link_directories(AudioEditor PRIVATE
        "C:/hamza/c++/lib/libsndfile/lib"
)

target_link_libraries(AudioEditor
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt6::Multimedia
        sndfile
)

if(WIN32)
    # libsndfile DLL
    add_custom_command(TARGET AudioEditor POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "C:/hamza/c++/lib/libsndfile/bin/sndfile.dll"
            "$<TARGET_FILE_DIR:AudioEditor>"
    )

    # Qt DLLs
    set(DEBUG_SUFFIX "")
    if(MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif()

    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if(NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if(NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif()
    endif()

    foreach(QT_LIB Core Gui Widgets Multimedia MultimediaWidgets MultimediaQuick Network)
        if(EXISTS "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll")
            add_custom_command(TARGET AudioEditor POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                    "$<TARGET_FILE_DIR:AudioEditor>"
            )
        endif()
    endforeach()

    # Copy platform plugin
    add_custom_command(TARGET AudioEditor POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:AudioEditor>/plugins/platforms"
    )
    if(EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET AudioEditor POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:AudioEditor>/plugins/platforms"
        )
    endif()

    # ----------------------------
    # Copy MinGW runtime DLLs
    # ----------------------------
    set(MINGW_BIN "C:/Hamza/c++/lib/Qt/6.10.1/mingw_64/bin")  # <-- replace with your MinGW bin path
    foreach(MINGW_DLL libstdc++-6.dll libgcc_s_seh-1.dll libwinpthread-1.dll Qt6Network.dll)
        if(EXISTS "${MINGW_BIN}/${MINGW_DLL}")
            add_custom_command(TARGET AudioEditor POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${MINGW_BIN}/${MINGW_DLL}"
                    "$<TARGET_FILE_DIR:AudioEditor>"
            )
        endif()
    endforeach()
endif()

