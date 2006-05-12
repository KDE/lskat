project(lskat)

add_subdirectory(src)

########### install files ###############
install_files( ${XDG_APPS_DIR} FILES lskat.desktop )

kde4_install_icons( ${ICON_INSTALL_DIR}  hicolor )

