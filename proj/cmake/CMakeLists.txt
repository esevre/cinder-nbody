cmake_minimum_required( VERSION 3.0 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( BasicApp )

get_filename_component( CINDER_PATH "/Users/esevre/Dropbox/local/cinder/Cinder" ABSOLUTE )
get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE )
get_filename_component( NBODY_PATH "/Users/esevre/Dropbox/courses-education/SNU2016_courses/sem02/adv-modelling-2/midterm/cinder-basic/nbody/" ABSOLUTE)

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

include_directories ( ${NBODY_PATH} )

ci_make_app(
	SOURCES     ${APP_PATH}/src/BasicApp.cpp ${NBODY_PATH}/bh_tree.h ${NBODY_PATH}/bh_tree_node.h ${NBODY_PATH}/body.h ${NBODY_PATH}/point.h ${NBODY_PATH}/region.h ${NBODY_PATH}/body_builder.h
	CINDER_PATH ${CINDER_PATH}
)
