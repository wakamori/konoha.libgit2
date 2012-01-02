cmake_minimum_required(VERSION 2.6)
set(CMAKE_BUILD_TYPE ${KONOHA_BUILD_TYPE})

project(libgit2)

find_library(HAVE_LIB_LIBGIT2 git2)
if(HAVE_LIB_LIBGIT2)

set(PACKAGE_SOURCE_CODE
	src/libgit2.c
	src/blob.c
	src/commit.c
	src/config.c
	src/index.c
	src/indexer.c
	src/object.c
	src/odb.c
	src/oid.c
	src/reference.c
	src/reflog.c
	src/refspec.c
	src/remote.c
	src/repository.c
	src/revwalk.c
	src/signature.c
	src/status.c
	src/tag.c
	src/transport.c
	src/tree.c
	src/treebuilder.c
	)
set(PACKAGE_SCRIPT_CODE libgit2.k)

set(PACKAGE_NAME    ${PROJECT_NAME})
set(PACKAGE_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})
set(PACKAGE_STRING  ${CMAKE_PROJECT_NAME}-${PACKAGE_VERSION})
set(KONOHA_PACKAGE_DIR konoha/package/${KONOHA_VERSION}/konoha.${PROJECT_NAME})

set(INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}
	${KONOHA_INCLUDE_DIRS})
include_directories(${INCLUDE_DIRS})

add_definitions(-D_SETUP)

add_library(${PACKAGE_NAME} SHARED ${PACKAGE_SOURCE_CODE})
set_target_properties(${PACKAGE_NAME} PROPERTIES PREFIX "")
target_link_libraries(${PACKAGE_NAME} konoha ${HAVE_LIB_LIBGIT2})

install(TARGETS ${PACKAGE_NAME} DESTINATION ${KONOHA_PACKAGE_DIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE_SCRIPT_CODE} DESTINATION ${KONOHA_PACKAGE_DIR})

endif(HAVE_LIB_LIBGIT2)

