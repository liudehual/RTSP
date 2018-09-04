INCLUDE_DIRECTORIES(${Client_SOURCE_DIR}/until)

project(mediaClient)
include_directories(.)

set(SOURCE_LIST
	main.cpp
)
ADD_EXECUTABLE(mediaClient                                    
     ${SOURCE_LIST}                                       
)  

if(LINUX)
	set(LIB until)
else()
	set(LIB until)
endif()

 
TARGET_LINK_LIBRARIES(mediaClient ${LIB})          
                                                                                               