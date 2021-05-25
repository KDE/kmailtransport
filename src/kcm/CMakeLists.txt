
add_library(kcm_mailtransport MODULE)
target_sources(kcm_mailtransport PRIVATE configmodule.cpp)
target_link_libraries(kcm_mailtransport
                        KF5::KCMUtils
                        KF5::ConfigWidgets
                        KF5::MailTransport
)

install(TARGETS kcm_mailtransport DESTINATION ${KDE_INSTALL_PLUGINDIR})
install(FILES kcm_mailtransport.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})

