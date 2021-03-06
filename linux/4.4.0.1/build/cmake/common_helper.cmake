macro (list2string out in)
    #string(REPLACE ";" " " ${out} "${in}")
    #message(STATUS "OUT:${${out}}")
    set(list ${ARGV})
    list(REMOVE_ITEM list ${out})
    foreach(item ${list})
        if(NOT ${out})
            set(${out} "${item}")
        else()
            set(${out} "${${out}} ${item}")
        endif()
    endforeach()
endmacro(list2string)
