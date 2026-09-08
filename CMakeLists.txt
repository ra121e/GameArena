add_executable(random_bot "random_bot.c")
target_compile_features(random_bot PRIVATE c_std_11)
target_link_libraries(random_bot PRIVATE arena)

add_executable(flipflop_5x5_bot "flipflop_5x5_bot.c")
target_compile_features(flipflop_5x5_bot PRIVATE c_std_11)
target_link_libraries(flipflop_5x5_bot PRIVATE arena)

add_executable(flipfour_bot "flipfour_bot.c")
target_compile_features(flipfour_bot PRIVATE c_std_11)
target_link_libraries(flipfour_bot PRIVATE arena)

add_executable(amoeba_bot "amoeba_bot.c")
target_compile_features(amoeba_bot PRIVATE c_std_11)
target_link_libraries(amoeba_bot PRIVATE arena)
