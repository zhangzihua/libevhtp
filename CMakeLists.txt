#cmake_minimum_required(VERSION 2.8)
#project(libevhtp)
#
set(PROJECT_MAJOR_VERSION 1)
set(PROJECT_MINOR_VERSION 2)
set(PROJECT_PATCH_VERSION 11)
set(PROJECT_VERSION      ${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_VERSION})
#
#set(CMAKE_C_FLAGS         "${CMAKE_C_FLAGS} -DPROJECT_VERSION=${PROJECT_VERSION} -Wall")
#set(CMAKE_C_FLAGS_DEBUG   "${CMAKE_C_FLAGS_DEBUG} -O0 -ggdb3")
#
set (CMAKE_BUILD_TYPE Release CACHE STRING "default to Release")

## will use later for htexpress API
## add_executable(lambda-pp lambda-pp.c)
#
## -DEVHTP_DISABLE_SSL:STRING=ON
option (EVHTP_DISABLE_SSL             "Disable ssl support"      OFF)
#
## -DEVHTP_DISABLE_EVTHR:STRING=ON
option (EVHTP_DISABLE_EVTHR           "Disable evthread support" OFF)
#
## -DEVHTP_DISABLE_REGEX:STRING=ON
option (EVHTP_DISABLE_REGEX           "Disable regex support"    OFF)
#
## -DEVHTP_BUILD_SHARED:STRING=ON
option (EVHTP_BUILD_SHARED            "Build shared library too" OFF)
#
## -DEVHTP_USE_JEMALLOC:STRING=ON
option (EVHTP_USE_JEMALLOC            "Enable jemalloc allocator" OFF)
#
## -DEVHTP_USE_TCMALLOC:STRING=ON
option (EVHTP_USE_TCMALLOC            "Enable tcmalloc allocator" OFF)
#
## fun color stuff
if(NOT WIN32)
    string(ASCII 27 Esc)
    set(ColourReset "${Esc}[m")
    set(ColourBold  "${Esc}[1m")
    set(Red         "${Esc}[31m")
    set(Green       "${Esc}[32m")
    set(Yellow      "${Esc}[33m")
    set(Blue        "${Esc}[34m")
    set(Magenta     "${Esc}[35m")
    set(Cyan        "${Esc}[36m")
    set(White       "${Esc}[37m")
    set(BoldRed     "${Esc}[1;31m")
    set(BoldGreen   "${Esc}[1;32m")
    set(BoldYellow  "${Esc}[1;33m")
    set(BoldBlue    "${Esc}[1;34m")
    set(BoldMagenta "${Esc}[1;35m")
    set(BoldCyan    "${Esc}[1;36m")
    set(BoldWhite   "${Esc}[1;37m")
endif()
#
set (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
#
include (CheckFunctionExists)
include (CheckIncludeFiles)
include (CheckTypeSize)
include (CheckCCompilerFlag)
include (TestBigEndian)
#
find_package (LibEvent REQUIRED)
#
check_function_exists (strndup HAVE_STRNDUP)
check_function_exists (strnlen HAVE_STRNLEN)
#
find_path(LIBEVENT_INCLUDE_DIR event2/event.h REQUIRED)
#
check_type_size ("int" SIZEOF_INT)
check_type_size ("long" SIZEOF_LONG)
check_type_size ("short" SIZEOF_SHORT)
#
check_c_compiler_flag (-fvisibility=hidden EVHTP_HAS_VISIBILITY_HIDDEN)
check_c_compiler_flag (-std=c99 EVHTP_HAS_C99)
#
test_big_endian(HOST_BIG_ENDIAN)
#
check_include_files (alloca.h                 HAVE_ALLOCA_H)
check_include_files (arpa/inet.h              HAVE_ARPA_INET_H)
check_include_files (assert.h                 HAVE_ASSERT_H)
check_include_files (ctype.h                  HAVE_CTYPE_H)
check_include_files (errno.h                  HAVE_ERRNO_H)
check_include_files (event2/listener.h        HAVE_EVENT2_LISTENER_H)
check_include_files (event2/buffer.h          HAVE_EVENT2_BUFFER_H)
check_include_files (event2/bufferevent.h     HAVE_EVENT2_BEV_H)
check_include_files (event2/bufferevent_ssl.h HAVE_EVENT2_SSL_H)
check_include_files (event2/dns.h             HAVE_EVENT2_DNS_H))
check_include_files (event2/event.h           HAVE_EVENT2_H)
check_include_files (fcntl.h                  HAVE_FCNTL_H)
check_include_files (inttypes.h               HAVE_INTTYPES_H)
check_include_files (limits.h                 HAVE_LIMITS_H)
check_include_files (locale.h                 HAVE_LOCALE_H)
check_include_files (malloc.h                 HAVE_MALLOC_H)
check_include_files (netinet/in.h             HAVE_NETINET_IN_H)
check_include_files (netinet/tcp.h            HAVE_NETINET_TCP_H)
check_include_files (pthread.h                HAVE_PTHREAD_H)
check_include_files (signal.h                 HAVE_SIGNAL_H)
check_include_files (stdarg.h                 HAVE_STDARG_H)
check_include_files (stdbool.h                HAVE_STDBOOL_H)
check_include_files (stddef.h                 HAVE_STDDEF_H)
check_include_files (stdint.h                 HAVE_STDINT_H)
check_include_files (stdio.h                  HAVE_STDIO_H)
check_include_files (stdlib.h                 HAVE_STDLIB_H)
check_include_files (string.h                 HAVE_STRING_H)
check_include_files (strings.h                HAVE_STRINGS_H)
check_include_files (sys/ioctl.h              HAVE_SYS_IOCTL_H)
check_include_files (sys/queue.h              HAVE_SYS_QUEUE_H)
check_include_files (sys/socket.h             HAVE_SYS_SOCKET_H)
check_include_files (sys/syscall.h            HAVE_SYS_SYSCALL_H)
check_include_files (sys/time.h               HAVE_SYS_TIME_H)
check_include_files (sys/times.h              HAVE_SYS_TIMES_H)
check_include_files (sys/types.h              HAVE_SYS_TYPES_H)
check_include_files (sys/un.h                 HAVE_SYS_UN_H)
check_include_files (time.h                   HAVE_TIME_H)
check_include_files (unistd.h                 HAVE_UNISTD_H)
check_include_files (sys/tree.h               HAVE_SYS_TREE)
check_include_files (sys/queue.h              HAVE_SYS_QUEUE)

if (EVHTP_HAS_VISIBILITY_HIDDEN)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
endif()
#
if (NOT HAVE_SYS_TREE)
    configure_file(
			${CMAKE_CURRENT_SOURCE_DIR}/compat/sys/tree.h.in
			${CMAKE_CURRENT_BINARY_DIR}/compat/sys/tree.h)
endif()
#
if (NOT HAVE_SYS_QUEUE)
		configure_file(
			${CMAKE_CURRENT_SOURCE_DIR}/compat/sys/queue.h.in
			${CMAKE_CURRENT_BINARY_DIR}/compat/sys/queue.h)
endif()
#
if (NOT HAVE_STRNDUP)
	add_definitions (-DNO_STRNDUP)
endif()
#
if (NOT HAVE_STRNLEN)
	add_definitions (-DNO_STRNLEN)
endif()
#
if (EVHTP_HAS_C99)
	add_definitions (-DEVHTP_HAS_C99)
endif()
#
if (NOT HAVE_SYS_UN)
	add_definitions (-DNO_SYS_UN)
endif()
#
if (EVHTP_THR_SHARED_PIPE)
	add_definitions (-DEVTHR_SHARED_PIPE)
endif()
#
if (HOST_BIG_ENDIAN)
	add_definitions (-DHOST_BIG_ENDIAN)
endif()
#
# Test 32/64 bits
if ("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
	add_definitions (-DEVHTP_SYS_ARCH=64)
elseif("${CMAKE_SIZEOF_VOID_P}" EQUAL "4")
	add_definitions (-DEVHTP_SYS_ARCH=32)
else()
	message(ERROR "Unable to determine architecture")
endif()
#
if (EVHTP_USE_DEFER_ACCEPT)
	add_definitions (-DUSE_DEFER_ACCEPT)
endif()
#
#set (CMAKE_INCLUDE_CURRENT_DIR ON)
#
if (NOT LIBEVENT_OPENSSL_LIBRARY)
	set (EVHTP_DISABLE_SSL ON)
endif()
#
if (NOT EVHTP_DISABLE_SSL)
	find_package (OpenSSL)
endif()
#
if (NOT EVHTP_DISABLE_REGEX)
	find_library (HAS_SYS_ONIG onig)
endif()
#
if (NOT OPENSSL_FOUND)
	message(STATUS "${Yellow}Unable to find libssl.. Disabling SSL support ${ColourReset}")
	#
	set (EVHTP_DISABLE_SSL        ON)
	#	set (OPENSSL_CRYPTO_LIBRARY   "")
	#	set (OPENSSL_INCLUDE_DIR      "")
	#	set (OPENSSL_LIBRARIES        "")
	#	set (LIBEVENT_OPENSSL_LIBRARY "")
elseif(APPLE)
	# darwin based hosts have deprecated normal openssl functions, which is
	# annoying to see warnings, for now, just ignore them.
	add_definitions (-Wno-deprecated-declarations)
#endif()
#
#if (NOT EVHTP_DISABLE_REGEX)
#	if (NOT HAS_SYS_ONIG)
#		message(STATUS "${Blue}Using the built-in onigurma source.${ColourReset}")
#		CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/oniguruma/config.h.in
#			${CMAKE_CURRENT_BINARY_DIR}/oniguruma/config.h)
#
#		set(ONIG_SOURCES
#			oniguruma/regerror.c
#			oniguruma/regparse.c
#			oniguruma/regext.c
#			oniguruma/regcomp.c
#			oniguruma/regexec.c
#			oniguruma/reggnu.c
#			oniguruma/regenc.c
#			oniguruma/regsyntax.c
#			oniguruma/regtrav.c
#			oniguruma/regversion.c
#			oniguruma/st.c
#			oniguruma/regposix.c
#			oniguruma/regposerr.c
#			oniguruma/enc/unicode.c
#			oniguruma/enc/ascii.c
#			oniguruma/enc/utf8.c
#			oniguruma/enc/utf16_be.c
#			oniguruma/enc/utf16_le.c
#			oniguruma/enc/utf32_be.c
#			oniguruma/enc/utf32_le.c
#			oniguruma/enc/euc_jp.c
#			oniguruma/enc/sjis.c
#			oniguruma/enc/iso8859_1.c
#			oniguruma/enc/iso8859_2.c
#			oniguruma/enc/iso8859_3.c
#			oniguruma/enc/iso8859_4.c
#			oniguruma/enc/iso8859_5.c
#			oniguruma/enc/iso8859_6.c
#			oniguruma/enc/iso8859_7.c
#			oniguruma/enc/iso8859_8.c
#			oniguruma/enc/iso8859_9.c
#			oniguruma/enc/iso8859_10.c
#			oniguruma/enc/iso8859_11.c
#			oniguruma/enc/iso8859_13.c
#			oniguruma/enc/iso8859_14.c
#			oniguruma/enc/iso8859_15.c
#			oniguruma/enc/iso8859_16.c
#			oniguruma/enc/euc_tw.c
#			oniguruma/enc/euc_kr.c
#			oniguruma/enc/big5.c
#			oniguruma/enc/gb18030.c
#			oniguruma/enc/koi8_r.c
#			oniguruma/enc/cp1251.c)
#
#		set (ONIG_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/oniguruma;${CMAKE_CURRENT_SOURCE_DIR}/oniguruma)
#	else()
#		message(STATUS "${YELLOW}Using the system-wide version of oniguruma${ColourReset}")
#
#		set(ONIG_SOURCES "")
#		set(ONIG_LIBS ${HAS_SYS_ONIG})
#		set(ONIG_INCLUDE_DIR "")
#	endif(NOT HAS_SYS_ONIG)
#else()
#	set(ONIG_SOURCES "")
#	set(ONIG_LIBS "")
#	set(ONIG_INCLUDE_DIR "")
#endif()
#
#include_directories(
#	${CMAKE_CURRENT_SOURCE_DIR}/src
#	${CMAKE_CURRENT_BINARY_DIR}
#	${CMAKE_CURRENT_BINARY_DIR}/compat
#	${CMAKE_CURRENT_SOURCE_DIR}
#	${ONIG_INCLUDE_DIR}
#	${OPENSSL_INCLUDE_DIR}
#	${LIBEVENT_INCLUDE_DIR}
#)
#
#set(LIBEVHTP_EXTERNAL_LIBS
#		${LIBEVENT_LIBRARY}
#		${LIBEVENT_PTHREADS_LIBRARY}
#		${LIBEVENT_OPENSSL_LIBRARY}
#		${OPENSSL_LIBRARIES}
#		${ONIG_LIBS}
#)
#
#if (NOT ${LIBEVENT_PTHREADS_FOUND})
#	set(EVHTP_DISABLE_EVTHR ON)
#endif(NOT ${LIBEVENT_PTHREADS_FOUND})
#
#if (NOT ${LIBEVENT_OPENSSL_FOUND})
#	set (EVHTP_DISABLE_SSL ON)
#endif(NOT ${LIBEVENT_OPENSSL_FOUND})
#
##SET(source_list  "${source_list}" CACHE INTERNAL "source_list")
#
#set(LIBEVHTP_SOURCES evhtp.c evhtp_numtoa.c htparse.c)
#
#if (EVHTP_USE_JEMALLOC)
#		set(LIBEVHTP_EXTERNAL_LIBS ${LIBEVHTP_EXTERNAL_LIBS} jemalloc)
#endif()
#
#if (EVHTP_USE_TCMALLOC)
#		set(LIBEVHTP_EXTERNAL_LIBS ${LIBEVHTP_EXTERNAL_LIBS} tcmalloc)
#endif()
#
#
##if (NOT EVHTP_DISABLE_EVTHR)
##	set (LIBEVHTP_EXTERNAL_LIBS ${LIBEVHTP_EXTERNAL_LIBS} pthread)
##	set (LIBEVHTP_SOURCES ${LIBEVHTP_SOURCES} evthr.c)
##endif(NOT EVHTP_DISABLE_EVTHR)
#
#IF (WIN32)
#	add_definitions(-DWIN32 -march=i486)
#	find_library (LIB_WS32 ws2_32)
#	set (SYS_LIBS ${SYS_LIBS} ${LIB_WS32})
#ELSE ()
#	if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
#		find_library (LIB_DL dl)
#		set (SYS_LIBS ${LIB_DL})
#	endif()
#
#	if (NOT APPLE)
#		find_library (LIB_RT rt)
#		set (SYS_LIBS ${SYS_LIBS} ${LIB_RT})
#	endif()
#ENDIF (WIN32)
#
#if (EVHTP_BUILD_SHARED)
#	set (EVHTP_LIBTYPE SHARED)
#else()
#	set (EVHTP_LIBTYPE STATIC)
#endif()
#
#configure_file(
#	${CMAKE_CURRENT_SOURCE_DIR}/evhtp-config.h.in
#	${CMAKE_CURRENT_BINARY_DIR}/evhtp-config.h)
#
#message(STATUS "BEFORE " ${LIBEVHTP_SOURCES})
#set (EVHTP_INTERNAL_LIBS "")
#
#add_subdirectory(src)
#
#message(STATUS "AFTER " "${LIBEVHTP_SOURCES}...${EVHTP_INTERNAL_LIBS}")
#
#add_library(evhtp ${EVHTP_LIBTYPE} ${LIBEVHTP_SOURCES} ${ONIG_SOURCES})
#target_link_libraries(evhtp ${LIBEVHTP_EXTERNAL_LIBS})
#
#if (EVHTP_BUILD_SHARED)
#		set_target_properties(evhtp PROPERTIES SOVERSION "${PROJECT_VERSION}")
#endif()
#
#
#if (NOT LIB_INSTALL_DIR)
#  set (LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib")
#endif (NOT LIB_INSTALL_DIR)
#
#if(NOT INCLUDE_INSTALL_DIR)
#	set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include)
#endif()
#
#install (TARGETS evhtp DESTINATION ${LIB_INSTALL_DIR})
#install (FILES evhtp.h DESTINATION ${INCLUDE_INSTALL_DIR}/evhtp)
#install (FILES htparse.h DESTINATION ${INCLUDE_INSTALL_DIR}/evhtp)
#install (FILES ${CMAKE_CURRENT_BINARY_DIR}/evhtp-config.h DESTINATION ${INCLUDE_INSTALL_DIR}/evhtp)
#
##if (NOT EVHTP_DISABLE_EVTHR)
##	install (FILES evthr.h DESTINATION ${INCLUDE_INSTALL_DIR}/evhtp)
##endif()
#
#if (NOT EVHTP_DISABLE_REGEX)
#		if (NOT HAS_SYS_ONIG)
#				install (FILES oniguruma/onigposix.h DESTINATION ${INCLUDE_INSTALL_DIR}/evhtp)
#		endif()
#endif()
#
#if (NOT HAVE_SYS_QUEUE)
#		install(FILES ${CMAKE_CURRENT_BINARY_DIR}/compat/sys/queue.h
#				DESTINATION ${INCLUDE_INSTALL_DIR}/evhtp/sys)
#endif()
#
#IF (WIN32)
#   install (FILES compat/sys/queue.h DESTINATION ${INCLUDE_INSTALL_DIR}/sys)
#   install (FILES oniguruma/onigposix.h DESTINATION ${INCLUDE_INSTALL_DIR})
#ENDIF (WIN32)
#
#configure_file(
#	${CMAKE_CURRENT_SOURCE_DIR}/evhtp.pc.in
#	${CMAKE_CURRENT_BINARY_DIR}/evhtp.pc @ONLY)
#
#install(FILES       "${CMAKE_CURRENT_BINARY_DIR}/evhtp.pc"
#        DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig")
#
#set(LIBEVHTP_LIBS "")
#
#
#add_subdirectory(tools)
#add_subdirectory(examples)
#
## add_definitions() calls do not show up in the C_FLAGS var
## it is instead a property of COMPILE_DEFINITIONS.
##
## so we fetch the property into the variable CDEFS, iterate over each entry,
## append it to a list, and finally replace ";" separators to -D to mimic a CFLAG
#
#get_property(CDEFS DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY COMPILE_DEFINITIONS)
#
#foreach(CDEF ${CDEFS})
#	list(APPEND CFLAG_LIST ${CDEF})
#endforeach()
#
#string(REPLACE ";" " -D" CFLAG_DEFS "${CFLAG_LIST}")
#
#message("")
#message(STATUS "${BoldBlue}EVHTP_VERSION${ColourReset}            : ${BoldGreen} ${PROJECT_VERSION}${ColourReset}")
#message(STATUS "${BoldBlue}EVHTP_DISABLE_SSL${ColourReset}        : ${BoldGreen} ${EVHTP_DISABLE_SSL}${ColourReset}")
#message(STATUS "${BoldBlue}EVHTP_DISABLE_EVTHR${ColourReset}      : ${BoldGreen} ${EVHTP_DISABLE_EVTHR}${ColourReset}")
#message(STATUS "${BoldBlue}EVHTP_DISABLE_REGEX${ColourReset}      : ${BoldGreen} ${EVHTP_DISABLE_REGEX}${ColourReset}")
#message(STATUS "${BoldBlue}EVHTP_BUILD_SHARED${ColourReset}       : ${BoldGreen} ${EVHTP_BUILD_SHARED}${ColourReset}")
#message(STATUS "${BoldBlue}EVHTP_USE_JEMALLOC${ColourReset}       : ${BoldGreen} ${EVHTP_USE_JEMALLOC}${ColourReset}")
#message(STATUS "${BoldBlue}EVHTP_USE_TCMALLOC${ColourReset}       : ${BoldGreen} ${EVHTP_USE_TCMALLOC}${ColourReset}")
#message("")
#message(STATUS "${Blue}CMAKE_BUILD_TYPE${ColourReset}         : " ${BoldRed}${CMAKE_BUILD_TYPE}${ColourReset})
#message(STATUS "${Blue}CMAKE_INSTALL_PREFIX${ColourReset}     : " ${BoldMagenta}${CMAKE_INSTALL_PREFIX}${ColourReset})
#message(STATUS "${Blue}CMAKE_BINARY_DIR${ColourReset}         : " ${CMAKE_BINARY_DIR})
#message(STATUS "${Blue}CMAKE_CURRENT_BINARY_DIR${ColourReset} : " ${CMAKE_CURRENT_BINARY_DIR})
#message(STATUS "${Blue}CMAKE_SOURCE_DIR${ColourReset}         : " ${CMAKE_SOURCE_DIR})
#message(STATUS "${Blue}CMAKE_CURRENT_SOURCE_DIR${ColourReset} : " ${CMAKE_CURRENT_SOURCE_DIR})
#message(STATUS "${Blue}PROJECT_BINARY_DIR${ColourReset}       : " ${PROJECT_BINARY_DIR})
#message(STATUS "${Blue}PROJECT_SOURCE_DIR${ColourReset}       : " ${PROJECT_SOURCE_DIR})
#message(STATUS "${Blue}CMAKE_MODULE_PATH${ColourReset}        : " ${CMAKE_MODULE_PATH})
#message(STATUS "${Blue}CMAKE_SYSTEM_NAME${ColourReset}        : " ${CMAKE_SYSTEM_NAME})
#message(STATUS "${Blue}CMAKE_SYSTEM_VERSION${ColourReset}     : " ${CMAKE_SYSTEM_VERSION})
#message(STATUS "${Blue}CMAKE_C_COMPILER${ColourReset}         : " ${CMAKE_C_COMPILER})
#message(STATUS "${Blue}CMAKE_AR${ColourReset}                 : " ${CMAKE_AR})
#message(STATUS "${Blue}CMAKE_RANLIB${ColourReset}             : " ${CMAKE_RANLIB})
#message(STATUS "${Blue}CFLAGS${ColourReset}                   : -D${CFLAG_DEFS} ${CMAKE_C_FLAGS}")
#
#if (CMAKE_BUILD_TYPE MATCHES Debug)
#		message("                              ${CMAKE_C_FLAGS_DEBUG}")
#else(CMAKE_BUILD_TYPE MATCHES Release)
#		message("                              ${CMAKE_C_FLAGS_RELEASE}")
#endif()
#
#set (CPACK_GENERATOR DEB)
#set (CPACK_PACKAGE_NAME "libevhtp")
#set (CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
#set (CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_MAJOR_VERSION})
#set (CPACK_PACKAGE_VERSION_MINOR ${PROJECT_MINOR_VERSION})
#set (CPACK_PACKAGE_VERSION_PATCH ${PROJECT_PATCH_VERSION})
#
#set(PROJECT_VERSION_REVISION 0)
#find_program(GIT_PROGRAM git DOC "git program")
#if(GIT_PROGRAM)
#  execute_process(
#    COMMAND ${GIT_PROGRAM} rev-parse --verify --short HEAD
#    OUTPUT_VARIABLE PROJECT_VERSION_REVISION
#    OUTPUT_STRIP_TRAILING_WHITESPACE
#  )
#endif(GIT_PROGRAM)
#
#string(TOLOWER "${CPACK_PACKAGE_NAME}" CPACK_PACKAGE_NAME_LOWERCASE)
#find_program(DPKG_PROGRAM dpkg DOC "dpkg program of Debian-based systems")
#if(DPKG_PROGRAM)
#  execute_process(
#    COMMAND ${DPKG_PROGRAM} --print-architecture
#    OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE
#    OUTPUT_STRIP_TRAILING_WHITESPACE
#  )
#
#  set(CPACK_PACKAGE_FILE_NAME
#		"${CPACK_PACKAGE_NAME_LOWERCASE}_${PROJECT_VERSION}-${PROJECT_VERSION_REVISION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
#
#else(DPKG_PROGRAM)
#  set(CPACK_PACKAGE_FILE_NAME
#		"${CPACK_PACKAGE_NAME_LOWERCASE}_${PROJECT_VERSION}-${PROJECT_VERSION_REVISION}_${CMAKE_SYSTEM_NAME}")
#endif(DPKG_PROGRAM)
#
#set (CPACK_DEBIAN_PACKAGE_MAINTAINER "unknown")
#find_program(WHOAMI_PROGRAM whoami DOC "whoami program")
#
#if(WHOAMI_PROGRAM)
#  execute_process(
#    COMMAND ${WHOAMI_PROGRAM}
#    OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_MAINTAINER
#    OUTPUT_STRIP_TRAILING_WHITESPACE
#  )
#endif(WHOAMI_PROGRAM)
#INCLUDE(CPack)
#
#message("")
