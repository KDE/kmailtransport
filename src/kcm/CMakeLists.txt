set(mailtransport_kcm_srcs configmodule.cpp)
add_library(kcm_mailtransport MODULE ${mailtransport_kcm_srcs})
target_link_libraries(kcm_mailtransport
                        KF5::KCMUtils
                        KF5::ConfigWidgets
                        KF5::MailTransport
)

install(TARGETS kcm_mailtransport DESTINATION ${KDE_INSTALL_PLUGINDIR})
install(FILES kcm_mailtransport.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})

