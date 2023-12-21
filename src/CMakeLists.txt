file(GLOB SRC *.c *.h *.cc *.hh *.cpp *.hpp)
add_library(clcwsal ${SRC})

if(WIN32)
    add_subdirectory("win32")
elseif(UNIX)
    message(CHECK_START "Looking for X11 support...")
    find_package(X11)

    if(X11_FOUND)
        message(CHECK_PASS "Enabled X11 suport")
        add_subdirectory("x11")
    else()
        message(CHECK_FAIL "X11 support not found")
    endif()
endif()

install(TARGETS clcwsal LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)