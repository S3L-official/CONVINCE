file(GLOB_RECURSE SAFELANG_SRC *.h *.cpp)

add_library(Safelang
        STATIC
        ${SAFELANG_SRC}
        )

set_target_properties(Safelang
        PROPERTIES
        LINKER_LANGUAGE CXX
        PREFIX "lib"
        )
