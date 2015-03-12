pkg_check_modules(ELEMENTARY elementary)

if (ELEMENTARY_FOUND)

   include_directories(
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}/../include
      ${ELEMENTARY_INCLUDE_DIRS}
   )

   add_definitions(-Wall)

   set(
      WAR2EDIT_SRC
      main.c
      editor.c
      log.c
      grid.c
      texture.c
   )
   add_executable(war2edit ${WAR2EDIT_SRC})

   link_directories(${ELEMENTARY_LIBRARY_DIRS})
   target_link_libraries(war2edit ${LIBPUD_LIBRARIES} ${ELEMENTARY_LIBRARIES})

   install(
      TARGETS war2edit
      RUNTIME DESTINATION bin
   )

else()
   message(WARNING "Missing dependancies. War2Edit will not be compiled")
endif()
