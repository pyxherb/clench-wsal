file(GLOB SRC *.h *.hpp *.hh *.c *.cpp *.cc)
target_sources(clcwsal PUBLIC ${SRC})
target_link_libraries(clcwsal PUBLIC X11)
