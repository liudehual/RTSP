
project(mediaServer)
include_directories(.)
INCLUDE_DIRECTORIES(${server_SOURCE_DIR}/os)
include_directories(${server_SOURCE_DIR}/media)

set(SOURCE_LIST
	server.cpp
)
ADD_EXECUTABLE(mediaServer                                    
     ${SOURCE_LIST}                                       
)  
 
TARGET_LINK_LIBRARIES(mediaServer  media)          
                                                                                               