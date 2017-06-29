set(mailtransport_smtpplugin_SRCS
    smtpmailtransportplugin.cpp
    smtpconfigdialog.cpp
    )

kcoreaddons_add_plugin(mailtransport_smtpplugin JSON smtpmailtransport.json SOURCES
    ${mailtransport_smtpplugin_SRCS} INSTALL_NAMESPACE mailtransport)

target_link_libraries(mailtransport_smtpplugin
    KF5::MailTransport KF5::CoreAddons KF5::I18n Qt5::Widgets KF5::ConfigWidgets
    )
