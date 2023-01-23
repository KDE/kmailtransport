if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()
kcoreaddons_add_plugin(mailtransport_smtpplugin INSTALL_NAMESPACE "pim${QT_MAJOR_VERSION}/mailtransport")

target_sources(mailtransport_smtpplugin PRIVATE
    smtpmailtransportplugin.cpp
    smtpconfigdialog.cpp
    smtpjob.cpp
    smtpconfigwidget.cpp
    smtpconfigdialog.h
    smtpjob.h
    sessionuiproxy.h
    smtpmailtransportplugin.h
    smtpconfigwidget.h

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
    KF${KF_MAJOR_VERSION}::I18n
    KF${KF_MAJOR_VERSION}::ConfigWidgets
    KF${KF_MAJOR_VERSION}::KIOWidgets
    KPim::SMTP
    KPim::GAPICore
    )
