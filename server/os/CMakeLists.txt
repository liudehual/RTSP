PROJECT(os)
SET(OSLIB_SRC  atomic.cpp
			   EventContext.cpp
			   InternalStdLib.cpp
			   IdleTask.cpp
			   MyAssert.cpp
			   OS.cpp
			   OSCond.cpp
			   OSFileSource.cpp
			   OSHeap.cpp
			   OSRef.cpp
			   OSMutex.cpp
			   OSMutexRW.cpp
			   OSQueue.cpp
			   OSThread.cpp
			   QueryParamList.cpp
			   ResizeableStringFormatter.cpp
			   SDPUtils.cpp
			   Socket.cpp
			   SocketUtils.cpp
			   StringFormatter.cpp
			   StringParser.cpp
			   StringTranslator.cpp
			   StrPtrLen.cpp
			   Task.cpp
			   TCPListenerSocket.cpp
			   TCPSocket.cpp
			   TimeoutTask.cpp
			   UDPDemuxer.cpp
			   UDPSocket.cpp
			   UDPSocketPool.cpp
			   UserAgentParser.cpp
			   StreamLinkRef.cpp
				)
if(LINUX)
	SET(OSLIB_SRC ${OSLIB_SRC} ev.cpp ev_hw.cpp HWFile.cpp ev_epoll.cpp)
elseif(HI3520D)
	SET(OSLIB_SRC ${OSLIB_SRC} ev.cpp ev_hw.cpp HWFile.cpp ev_epoll.cpp)
else()
	SET(OSLIB_SRC ${OSLIB_SRC} win32ev.cpp)
endif()
message(${OSLIB_SRC})

ADD_LIBRARY(os STATIC ${OSLIB_SRC})
