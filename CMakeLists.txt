cmake_minimum_required(VERSION 3.23)
project(click)

set(CMAKE_CXX_STANDARD 20)
find_package(fmt REQUIRED)


add_executable(click main.cpp Where.cpp Where.h GraphiteFinder.cpp GraphiteFormatter.h ConverTime.cpp ConverTime.h  GraphiteFindHandler.cpp GraphiteFind.h GraphiteRenderHandler.cpp GraphiteRenderHandler.h GraphiteFind.cpp GraphiteFind.h GraphitePoints.cpp GraphitePoints.h GraphiteFormatter.cpp GraphiteFormatter.h)
target_link_libraries(click fmt::fmt)

