set(COMPONENT_SRCS "main.cpp")
set(COMPONENT_ADD_INCLUDEDIRS ".")
set(COMPONENT_ADD_INCLUDEDIRS "$IDF_PATH/components")

set(COMPONENT_REQUIRES soc nvs_flash)

register_component()