add_executable(random_bot "random_bot.c")
target_compile_features(random_bot PRIVATE c_std_11)
target_link_libraries(random_bot PRIVATE arena)
