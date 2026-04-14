add_executable(falling_sand main.c)

target_link_libraries(
  falling_sand
  PRIVATE
  SDL3::SDL3
)
