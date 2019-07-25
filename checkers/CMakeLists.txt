cmake_minimum_required(VERSION 3.12)

project(${EXEC_NAME}
    VERSION 0.1
    DESCRIPTION "A simple checker game"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 11)

if(${ENABLE_STATS})
    set(STATS_ENV "-D_STATS")
elseif($ENV{ENABLE_STATS})
    set(STATS_ENV "-D_STATS")
else()
    set(STATS_ENV "")
endif()

# Also look for Find-cmake modules here
set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/sdl2")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c11 -D_DEBUG=1 ${STATS_ENV} -Wall -Wextra -ggdb
 -fsanitize=address,undefined")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++17 -O3 -D_DEBUG=0")
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")


set(SOURCES
    src/Main.c
    src/Gameclock.c
    src/Gamestate.c
    src/states/Boardstate.c
    src/states/Pausestate.c
    src/states/Gameoverstate.c
    src/States.c
    src/convert.c
)

set(INCLUDE_DIRS
    "${PROJECT_SOURCE_DIR}/src"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})

if(WIN32)
    # I use the gcc toolchain even on windows
    # so this branch just checks for the included libs
    set(SDL_INCLUDE_DIRS
        "${PROJECT_SOURCE_DIR}/libs/SDL2-2.0.9/x86_64-w64-mingw32/include"
        "${PROJECT_SOURCE_DIR}/libs/SDL2_image-2.0.4/x86_64-w64-mingw32/include"
        "${PROJECT_SOURCE_DIR}/libs/SDL2_ttf-2.0.15/x86_64-w64-mingw32/include"
        "${PROJECT_SOURCE_DIR}/libs/SDL2-2.0.9/x86_64-w64-mingw32/include/SDL2"
        "${PROJECT_SOURCE_DIR}/libs/SDL2_image-2.0.4/x86_64-w64-mingw32/include/SDL2"
        "${PROJECT_SOURCE_DIR}/libs/SDL2_ttf-2.0.15/x86_64-w64-mingw32/include/SDL2"
    )

    set(SDL_LINK_DIRS
        "${PROJECT_SOURCE_DIR}/libs/SDL2-2.0.9/x86_64-w64-mingw32/lib"
        "${PROJECT_SOURCE_DIR}/libs/SDL2_image-2.0.4/x86_64-w64-mingw32/lib"
        "${PROJECT_SOURCE_DIR}/libs/SDL2_ttf-2.0.15/x86_64-w64-mingw32/lib"
    )

    set(SDL_LIBS
        SDL2
        SDL2_image
        SDL2_ttf
    )

    target_link_directories(${PROJECT_NAME} PRIVATE ${SDL_LINK_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL_LIBS})
    target_include_directories(${PROJECT_NAME} PRIVATE ${SDL_INCLUDE_DIRS})

else()
    # on ubuntu we use the cmake modules to find the libraries and the header include dirs

    target_link_libraries(${PROJECT_NAME} PRIVATE "asan" "lsan")

    find_package(SDL2 REQUIRED)
    target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES})

    find_package(SDL2_image REQUIRED)
    target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_IMAGE_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_IMAGE_LIBRARIES})

    find_package(SDL2_ttf REQUIRED)
    target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_TTF_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_TTF_LIBRARIES})

endif(WIN32)

