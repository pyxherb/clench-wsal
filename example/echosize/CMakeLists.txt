link_libraries(clcwsal)
file(GLOB SRC *.c *.h *.cc *.hh *.cpp *.hpp)
add_executable(echosize ${SRC})
