#add_subdirectory(heap)
#add_subdirectory(json)
#add_subdirectory(thread)

set (EVHTP_AUX_SOURCES
	json/evhtp_json.c
	heap/evhtp_heap.c
  thread/evhtp_thread.c
)

set (EVHTP_CORE_SOURCES
	core/evhtp.c
	core/evhtp_numtoa.c
	core/evhtp_parser.c
)



