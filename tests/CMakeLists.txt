add_executable(bomberman-tests test_logic.cpp)
target_link_libraries(bomberman-tests PRIVATE bomberman-logic)
target_compile_options(bomberman-tests PRIVATE -Wall -Wextra)
