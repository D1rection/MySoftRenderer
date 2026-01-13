set(LINE_TRY_SOURCES
  try_01.cpp
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
