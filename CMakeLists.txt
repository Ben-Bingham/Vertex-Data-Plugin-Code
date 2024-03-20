remove_definitions(-DTRANSLATION_DOMAIN="okular")
add_definitions(-DTRANSLATION_DOMAIN="okular_vertex_data")

########### next target ###############
set(okularGenerator_vertex_data_SRCS vertex_data_generator.cpp)

okular_add_generator(okularGenerator_vertex_data ${okularGenerator_vertex_data_SRCS})

target_link_libraries(okularGenerator_vertex_data okularcore Qt6::Core KF6::I18n GLEW glfw)

#list( APPEND CMAKE_INSTALL_RPATH "/usr/lib/dependencies" )
set_target_properties(okularGenerator_vertex_data
        PROPERTIES
        LINK_FLAGS "-Wl,-rpath,/usr/lib:/usr/lib/dependencies"
)

########### install files ###############
install( PROGRAMS okularApplication_vertex_data.desktop org.kde.mobile.okular_vertex_data.desktop  DESTINATION  ${KDE_INSTALL_APPDIR} )
install( FILES org.kde.okular-vertex-data.metainfo.xml DESTINATION ${KDE_INSTALL_METAINFODIR} )

install( FILES vertex-data-mime.xml PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ DESTINATION "/usr/share/mime/packages" )
# IMPORTANT - need to update mime database