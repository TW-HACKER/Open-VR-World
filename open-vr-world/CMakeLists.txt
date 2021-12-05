add_executable(open-vr-world)
find_package(bgfx REQUIRED)

target_link_libraries(open-vr-world PRIVATE glfw bgfx::bgfx)
target_sources(open-vr-world PRIVATE
	main.cpp)
