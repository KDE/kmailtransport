if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()
kcoreaddons_add_plugin(mailtransport_smtpplugin INSTALL_NAMESPACE "pim6/mailtransport")

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
    KPim6::MailTransport
    KF6::I18n
    KF6::ConfigWidgets
    KF6::KIOWidgets
    KPim6::SMTP
    KPim6::GAPICore
    )
