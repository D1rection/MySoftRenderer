set(LINE_TRY_SOURCES
  parametric_v1.cpp
  parametric_v2.cpp
  parametric_v3.cpp
  dda.cpp
  bresenham.cpp
)

foreach(src IN LISTS LINE_TRY_SOURCES)
  get_filename_component(basename "${src}" NAME_WE)
  add_executable("line_${basename}" "${src}")
  target_link_libraries("line_${basename}"
    PRIVATE
      tiny_renderer
      $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>
  )
endforeach()
