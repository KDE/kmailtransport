if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()
kcoreaddons_add_plugin(mailtransport_smtpplugin JSON smtpmailtransport.json INSTALL_NAMESPACE mailtransport)

target_sources(mailtransport_smtpplugin PRIVATE
    smtpmailtransportplugin.cpp
    smtpconfigdialog.cpp
    smtpjob.cpp
    smtpconfigwidget.cpp
    )
ki18n_wrap_ui(mailtransport_smtpplugin
    smtpsettings.ui
    )
ecm_qt_declare_logging_category(mailtransport_smtpplugin HEADER mailtransportplugin_smtp_debug.h IDENTIFIER MAILTRANSPORT_SMTP_LOG CATEGORY_NAME org.kde.pim.mailtransport.smtpplugin
        DESCRIPTION "kmailtransport (smtp plugin)"
        OLD_CATEGORY_NAMES log_smtp
        EXPORT MAILTRANSPORT
    )



target_link_libraries(mailtransport_smtpplugin
    KF5::MailTransport
    KF5::I18n
    KF5::ConfigWidgets
    KF5::KIOWidgets
    KPim::SMTP
    KPim::GAPICore
    )
