project(until)
include_directories(.)
set(SRC_LIST
	TCPSocket.cpp
	UDPSocket.cpp
	Socket.cpp
	Object.cpp
	RTSPSession.cpp
	RTPSession.cpp
	Engine.cpp
	RTSPClient.cpp
	Job.cpp
	RTCPSession.cpp
	ObjectQueue.cpp
	Until.cpp
	StrPtr.cpp
	SocketHelper.cpp
  	base64.cpp
	H264Session.cpp
	TimeoutJob.cpp
	EventJob.cpp
	TimeManager.cpp
	HeartBeatSession.cpp
	ClientInterface.cpp
	OSMutex.cpp
	H264FramesManager.cpp
)
add_library(until ${SRC_LIST})
if(LINUX)
	TARGET_LINK_LIBRARIES(until pthread)
endif(LINUX)

