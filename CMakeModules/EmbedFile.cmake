set(EMBEDPROGRAM ${CMAKE_BINARY_DIR}/bin/tools/Embed.exe)

macro(embed_file sourceFiles file)
	set(optional_args ${ARGN})
	list(LENGTH optional_args num_optional_args)
	if(num_optional_args EQUAL 1)
		list(GET optional_args 0 optional_arg)
		set(namespace_value ${optional_arg})
	endif()

	get_filename_component(fileDir ${file} DIRECTORY)
    get_filename_component(fileName ${file} NAME_WE)
	get_filename_component(fileExt ${file} EXT)
	STRING(REGEX REPLACE "\\." "_" fileExt "${fileExt}")
	
	set(fileCpp "${fileDir}/${fileName}${fileExt}.cxx")

    source_group("Generated Files" FILES ${PROJECT_BINARY_DIR}/${fileCpp}) 
    source_group("Embed Files" FILES ${file})

	if(namespace_value)
		add_custom_command(
				OUTPUT ${PROJECT_BINARY_DIR}/${fileCpp}
				DEPENDS ${file}
				COMMAND ${EMBEDPROGRAM} -i ${PROJECT_SOURCE_DIR}/${file} -n ${namespace_value} ${PROJECT_BINARY_DIR}/${fileCpp}
				COMMENT "Converting ${file} file into ${PROJECT_BINARY_DIR}/${fileCpp}."
				)
	else()
		add_custom_command(
			OUTPUT ${PROJECT_BINARY_DIR}/${fileCpp}
			DEPENDS ${file}
			COMMAND ${EMBEDPROGRAM} -i ${PROJECT_SOURCE_DIR}/${file} ${PROJECT_BINARY_DIR}/${fileCpp}
			COMMENT "Converting ${file} file into ${PROJECT_BINARY_DIR}/${fileCpp}."
			)
	endif()
    
    #Tell cmake that this fileCpp is generated.
    set(SET_SOURCE_FILE_PROPERTIES ${PROJECT_BINARY_DIR}/${fileCpp} PROPERTIES GENERATED TRUE)
    set(${sourceFiles} ${${sourceFiles}} ${PROJECT_BINARY_DIR}/${fileCpp})
    set(${sourceFiles} ${${sourceFiles}} ${file})
endmacro(embed_file sourceFiles file)

macro(embed_files sourceFiles files)
    foreach(file ${files})
        embed_file(${sourceFiles} ${file} ${ARGN})
    endforeach(file)
endmacro(embed_files sourceFiles files)