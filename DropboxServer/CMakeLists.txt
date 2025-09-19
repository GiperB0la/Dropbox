set(DROPBOX_SERVER_SOURCES
    Main.cpp
    src/DropboxServer.cpp
    src/HandleClient.cpp
)

include_directories(include)

add_executable(DropboxServer ${DROPBOX_SERVER_SOURCES})

target_link_libraries(DropboxServer PRIVATE ws2_32)