project(parpeloadbalancer)

set(SRC_LIST
    loadBalancerWorker.cpp
    loadBalancerMaster.cpp
)

add_library(${PROJECT_NAME} ${SRC_LIST})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME}
    parpecommon
    ${CMAKE_THREAD_LIBS_INIT}
)

install(TARGETS ${PROJECT_NAME} EXPORT ParPETargets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

install(
    DIRECTORY
    ${CMAKE_SOURCE_DIR}/include/parpeloadbalancer
    DESTINATION
    include)
