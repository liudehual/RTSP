message("Support media")
INCLUDE_DIRECTORIES(${server_SOURCE_DIR}/os)

project(media)

include_directories(.)
set(SOURCE_LIST
     base64.cpp
     RTSPMediaManager.cpp
     RTSPClientSession.cpp
     H264FramesManager.cpp
     myMutex.cpp
     myUntil.cpp
     RTSPInterface.cpp
     RTSPListener.cpp
     RTSPParserSession.cpp
     strDup.cpp
     H264Rtp.cpp
     Rtp.cpp
     netInfo.cpp
     TCPClientSession.cpp
)
add_library(media STATIC                                                                                                        
     ${SOURCE_LIST}         
)  
TARGET_LINK_LIBRARIES(media os pthread -static) 
                        