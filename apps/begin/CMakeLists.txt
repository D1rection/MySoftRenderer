add_executable(begin
  main.cpp
)

target_link_libraries(begin
  PRIVATE
    tiny_renderer
    $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>
)
