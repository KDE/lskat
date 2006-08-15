project(lskat)

add_subdirectory(src)
add_subdirectory(grafix)

########### install files ###############
install( FILES lskat.desktop  DESTINATION  ${XDG_APPS_DIR} )

kde4_install_icons( ${ICON_INSTALL_DIR}  hicolor )

