idf_build_get_property(target IDF_TARGET)

# 1 Example for IT8951 Boards (Adjusted for DEXA-C097 Good-Display)
message(STATUS "→ Building example for ${target}")


set(app_sources "i2c-scan.c")

idf_component_register(SRCS ${app_sources}
        INCLUDE_DIRS "include"
        REQUIRES driver
	)

#target_compile_options(${lib} PRIVATE -Wno-error=format)
