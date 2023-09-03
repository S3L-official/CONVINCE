# find FL_BIBRARY
if(APPLE)
    set(CMAKE_PREFIX_PATH "/usr/local/opt/flex")
endif()
FIND_PACKAGE(FLEX REQUIRED)
FIND_PACKAGE(BISON REQUIRED)
message( ${CMAKE_CURRENT_SOURCE_DIR} )
BISON_TARGET(parser ../../lib/language/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.h
        )
FLEX_TARGET(lexer ../../lib/language/lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)


ADD_FLEX_BISON_DEPENDENCY(lexer parser)

ADD_EXECUTABLE(convince
        ${FLEX_lexer_OUTPUTS}
        ${BISON_parser_OUTPUTS}
        main.cpp
        )

TARGET_LINK_LIBRARIES(convince Safelang jsoncpp)
