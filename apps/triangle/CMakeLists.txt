set(TRI_TRY_SOURCES
  hidden_face.cpp
  scanline.cpp
  modern.cpp
  gray.cpp
  rgb.cpp
  main.cpp
)

foreach(src IN LISTS TRI_TRY_SOURCES)
  get_filename_component(basename "${src}" NAME_WE)
  add_executable("tri_${basename}" "${src}")
  target_link_libraries("tri_${basename}"
    PRIVATE
      tiny_renderer
      $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>
  )
endforeach()
