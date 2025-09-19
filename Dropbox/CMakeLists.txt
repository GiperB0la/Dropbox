set(DROPBOX_SOURCES
    Main.cpp
    src/DragAndDrop.cpp
    src/DropboxApp.cpp
    src/FileDeletion.cpp
    src/FileDownloader.cpp
    src/FileUploader.cpp
    src/FileWidget.cpp
    src/UpdateFilesList.cpp
    src/Widgets.cpp
)

include_directories(include)

add_executable(DropboxClient ${DROPBOX_SOURCES})

find_package(SFML 2.6 COMPONENTS system window graphics network audio REQUIRED)

target_link_libraries(DropboxClient
    PRIVATE sfml-system sfml-window sfml-graphics sfml-audio sfml-network
)

add_custom_command(TARGET DropboxClient POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Dropbox/Graphics
        $<TARGET_FILE_DIR:DropboxClient>/Graphics
)