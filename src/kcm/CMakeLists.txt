
add_library(kcm_mailtransport MODULE)
target_sources(kcm_mailtransport PRIVATE configmodule.cpp configmodule.h)
target_link_libraries(kcm_mailtransport
                        KF${KF_MAJOR_VERSION}::KCMUtils
                        KF${KF_MAJOR_VERSION}::ConfigWidgets
                        KF${KF_MAJOR_VERSION}::CoreAddons
                        KF5::MailTransport
)

install(TARGETS kcm_mailtransport DESTINATION ${KDE_INSTALL_PLUGINDIR})
install(FILES kcm_mailtransport.desktop DESTINATION ${KDE_INSTALL_KSERVICESDIR})

