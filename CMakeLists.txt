project(lskat)

add_subdirectory(src)
add_subdirectory(grafix)

########### install files ###############
install( PROGRAMS lskat.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )

kde4_install_icons( ${ICON_INSTALL_DIR}   )

