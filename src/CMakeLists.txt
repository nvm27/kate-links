set(ktexteditor_links_PART_SRCS linksplugin.cpp linksplugindocument.cpp)

kde4_add_plugin(ktexteditor_links ${ktexteditor_links_PART_SRCS})

target_link_libraries(ktexteditor_links ${KDE4_KIO_LIBS} ktexteditor kdeui kfile)

install(TARGETS ktexteditor_links DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES ktexteditor_links.desktop DESTINATION ${SERVICES_INSTALL_DIR})

