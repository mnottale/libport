# - Provide the unrtf_file function.
# Call unrtf and remove the header.

if(NOT COMMAND UNRTF_FILE)


function(UNRTF_FILE input output options)
  find_package(Unrtf REQUIRED)
  set(tmpout ${output}.tmp)
  # FIXME: We do not use add_custom_command because CPack.cmake needs the
  # output file at configuration time.
  # (see http://public.kitware.com/Bug/view.php?id=6674)
  EXECUTE_PROCESS(
    COMMAND ${UNRTF_EXECUTABLE} ${options} ${input}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    TIMEOUT 30
    RESULT_VARIABLE ret
    ERROR_VARIABLE err
    OUTPUT_FILE ${tmpout}
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  if(ret)
    message(SEND_ERROR
      "'${UNRTF_EXECUTABLE}' failed to build '${input}' to '${output}'")
    message(STATUS "Unrtf error begins:\n${err}\nUnrtf error ends.")
  endif(ret)
  # Remove the header.
  file(READ ${tmpout} inbuf)
  string(REGEX REPLACE "^.*-----------------\n" "" outbuf ${inbuf})
  file(WRITE ${output} ${outbuf})
  file(REMOVE ${tmpout})
  message(STATUS "Unrtf file '${input}' to '${output}'")
endfunction(UNRTF_FILE)

endif(NOT COMMAND UNRTF_FILE)
