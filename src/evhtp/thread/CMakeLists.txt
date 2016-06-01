if (NOT EVHTP_DISABLE_EVTHR)
	message(STATUS 	"threads!!")

	set(LIBEVHTP_SOURCES
		${LIBEVHTP_SOURCES} "${CMAKE_CURRENT_SOURC_DIR}/evhtp_thread.c")

	message(STATUS ${LIBEVHTP_SOURCES})

	set(LIBEVHTP_EXTERNAL_LIBS
		${LIBEVHTP_EXTERNAL_LIBS} pthread)

	install(FILES
		evhtp_thread.h
		DESTINATION ${INCLUDE_INSTALL_DIR}/evhtp/thread)
endif()
