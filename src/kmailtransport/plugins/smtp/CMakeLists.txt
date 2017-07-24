set(mailtransport_smtpplugin_SRCS
    smtpmailtransportplugin.cpp
    smtpconfigdialog.cpp
    smtpjob.cpp
    smtpconfigwidget.cpp
    lineeditpassword.cpp
    )
qt5_add_resources(mailtransport_smtpplugin_SRCS icons.qrc)
ki18n_wrap_ui(mailtransport_smtpplugin_SRCS
    smtpsettings.ui
    )
ecm_qt_declare_logging_category(mailtransport_smtpplugin_SRCS HEADER mailtransportplugin_smtp_debug.h IDENTIFIER MAILTRANSPORT_SMTP_LOG CATEGORY_NAME org.kde.pim.mailtransport.smtpplugin)

kcoreaddons_add_plugin(mailtransport_smtpplugin JSON smtpmailtransport.json SOURCES
    ${mailtransport_smtpplugin_SRCS} INSTALL_NAMESPACE mailtransport)

target_link_libraries(mailtransport_smtpplugin
    KF5::MailTransport
    KF5::I18n
    KF5::ConfigWidgets
    KF5::KIOCore
    KF5::Completion
    )
