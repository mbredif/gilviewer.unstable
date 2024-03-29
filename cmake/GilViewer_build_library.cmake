set( GILVIEWER_LINK_EXTERNAL_LIBRARIES ${wxWidgets_LIBRARIES}
                                           ${Boost_LIBRARIES}
                                           ${JPEG_LIBRARIES}
                                           tinyxml
                                            )
if(WIN32)    
else()
    set( GILVIEWER_LINK_EXTERNAL_LIBRARIES ${GILVIEWER_LINK_EXTERNAL_LIBRARIES}
                                           ${PNG_LIBRARIES}
                                           ${ZLIB_LIBRARIES})
endif()

# Option to choose to use GDAL/OGR
# Find GDAL
find_package(GDAL)
option(USE_GDAL_OGR "Build GilViewer with GDAL/OGR" GDAL_FOUND)
if(USE_GDAL_OGR)
    if(GDAL_FOUND)
        include_directories(${GDAL_INCLUDE_DIR})
        set( GILVIEWER_LINK_EXTERNAL_LIBRARIES ${GILVIEWER_LINK_EXTERNAL_LIBRARIES} ${GDAL_LIBRARY} )
    else()
        message(FATAL_ERROR "GDAL not found!")
    endif()
    set( GILVIEWER_USE_GDAL_OGR 1 )
else()
    set( GILVIEWER_USE_GDAL_OGR 0 )
endif()

# Option to choose to use CGAL
# Find CGAL
find_package(CGAL COMPONENTS Core)
option(USE_CGAL "Build GilViewer with CGAL" CGAL_FOUND)
if(USE_CGAL)
    if(CGAL_FOUND)
        include( ${CGAL_USE_FILE} )
        set( GILVIEWER_LINK_EXTERNAL_LIBRARIES ${GILVIEWER_LINK_EXTERNAL_LIBRARIES} ${CGAL_LIBRARIES}  ${CGAL_3RD_PARTY_LIBRARIES} )
    else()
        message(FATAL_ERROR "CGAL not found!")
    endif()
    set( GILVIEWER_USE_CGAL 1 )
else()
    set( GILVIEWER_USE_CGAL 0 )
endif()

# Option to choose to use GDAL_JP2
option(USE_GDAL_JP2 "Build GilVeiwer with GDAL and JP2" OFF)
if(USE_GDAL_JP2)
    set( GILVIEWER_USE_GDALJP2 1 )
else()
    set( GILVIEWER_USE_GDALJP2 0 )
endif()


# Find ImageIO
find_package(ImageIO)
option(USE_ImageIO "Build GilViewer with ImageIO" ImageIO_FOUND)
if(USE_ImageIO)
    if(ImageIO_FOUND)
        include_directories(${ImageIO_INCLUDE_DIRS})
        link_directories(${ImageIO_LIBRARY_DIRS})
        set( GILVIEWER_LINK_EXTERNAL_LIBRARIES ${GILVIEWER_LINK_EXTERNAL_LIBRARIES} ${ImageIO_LIBRARIES} )
        message(STATUS  "ImageIO libraries : ${ImageIO_LIBRARIES}" )
    else()
        message(FATAL_ERROR "ImageIO not found!")
    endif()
    set( GILVIEWER_USE_IMAGEIO 1 )
else()
    set( GILVIEWER_USE_IMAGEIO 0 )
endif()

find_package(TIFF REQUIRED)
if(TIFF_FOUND)
    include_directories(${TIFF_INCLUDE_DIR})
    set( GILVIEWER_LINK_EXTERNAL_LIBRARIES ${GILVIEWER_LINK_EXTERNAL_LIBRARIES} ${TIFF_LIBRARIES} )
else()
    message(FATAL_ERROR "TIFF not found ! Please set TIFF path ...")
endif()


configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/src/GilViewer/config/config.hpp.cmake.in ${CMAKE_CURRENT_SOURCE_DIR}/src/GilViewer/config/config.hpp )

# Options pour construire une librarie (et la choisir en .a ou .so)
if( UNIX )
	SET( GILVIEWER_LIBRARY_TYPE SHARED )
	# Verification du type de bibliotheque a construire
	OPTION( BUILD_GILVIEWERLIB_SHARED "Build GilViewer as a SHARED library." ON )
	IF( BUILD_GILVIEWERLIB_SHARED )
		SET( GILVIEWER_LIBRARY_TYPE SHARED )
	ELSE()
		SET( GILVIEWER_LIBRARY_TYPE STATIC )
	ENDIF()
# Currently, dll are not supported on Windows platforms
else()
    set( GILVIEWER_LIBRARY_TYPE STATIC )
endif()

ADD_LIBRARY( GilViewer ${GILVIEWER_LIBRARY_TYPE} ${ALL_VIEWER_SOURCES} ${ALL_VIEWER_HEADERS} ${ALL_GIL_HEADER_FILES} )
TARGET_LINK_LIBRARIES( GilViewer ${GILVIEWER_LINK_EXTERNAL_LIBRARIES} )
IF(WIN32)
    SET_TARGET_PROPERTIES(GilViewer PROPERTIES COMPILE_DEFINITIONS "${wxWidgets_DEFINITIONS}" )
	if (${MSVC_VERSION} EQUAL 1600)
		add_definitions( /Zc:wchar_t- )
	endif()
ENDIF(WIN32)
