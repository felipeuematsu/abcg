project(baleiaPoints)
add_executable(${PROJECT_NAME} main.cpp openglwindow.cpp stb_image.h player.cpp objects.cpp )
enable_abcg(${PROJECT_NAME})
