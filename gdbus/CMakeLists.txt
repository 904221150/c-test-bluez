cmake_minimum_required (VERSION 2.8.1)

# XML file name
SET(XML_FILE_LIST org.bluez.agent1.xml)
# generated file name
SET(XML_GEN_FILE_LIST bluez-agent)

# check if the length of one of the above two lists is same as one of the other one
LIST(LENGTH XML_FILE_LIST XML_FILE_LIST_LEN)
LIST(LENGTH XML_GEN_FILE_LIST NAME_LIST_LEN)
MATH(EXPR XML_FILE_LIST_LEN "${XML_FILE_LIST_LEN}-1")
MATH(EXPR NAME_LIST_LEN "${NAME_LIST_LEN}-1")

message("XML_FILE_LIST_LEN = ${XML_FILE_LIST_LEN}")
message("NAME_LIST_LEN = ${NAME_LIST_LEN}")

IF(${XML_FILE_LIST_LEN} EQUAL ${NAME_LIST_LEN})
    message("test1")
# generate files from XML
    FOREACH(INDEX RANGE 0 ${XML_FILE_LIST_LEN})
        message("test2")
        LIST(GET XML_FILE_LIST ${INDEX} XML)
        LIST(GET XML_GEN_FILE_LIST ${INDEX} NAME_TMP)
        message("XML = ${XML}")
        message("NAME_TMP = ${NAME_TMP}")
        EXECUTE_PROCESS(COMMAND gdbus-codegen --interface-prefix org.bluez. --c-namespace Agent --generate-c-code ${PROJECT_BINARY_DIR}/${NAME_TMP} ${PROJECT_SOURCE_DIR}/gdbus/${XML})
    ENDFOREACH(INDEX)
ELSE (${XML_FILE_LIST_LEN} EQUAL ${NAME_LIST_LEN})
    MESSAGE(FATAL_ERROR "The length (${XML_FILE_LIST_LEN}) of xml files list does not equal to the length(${NAME_LIST_LEN}) of the generated files name list")
ENDIF(${XML_FILE_LIST_LEN} EQUAL ${NAME_LIST_LEN})


