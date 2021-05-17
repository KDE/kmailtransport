set(mailtransport_akonadiplugin_SRCS
    akonadimailtransportplugin.cpp
    resourcesendjob.cpp
    )

ecm_qt_declare_logging_category(mailtransport_akonadiplugin_SRCS HEADER mailtransportplugin_akonadi_debug.h IDENTIFIER
        MAILTRANSPORT_AKONADI_LOG CATEGORY_NAME org.kde.pim.mailtransport.akonadiplugin
        DESCRIPTION "kmailtransport (akonadi plugin)"
        EXPORT MAILTRANSPORT
    )



kcoreaddons_add_plugin(mailtransport_akonadiplugin JSON akonadimailtransport.json SOURCES
    ${mailtransport_akonadiplugin_SRCS} INSTALL_NAMESPACE mailtransport)

target_link_libraries(mailtransport_akonadiplugin
    KF5::MailTransportAkonadi
    KF5::CoreAddons
    KF5::AkonadiCore
    KF5::I18n
    KF5::ConfigWidgets
    Qt::DBus
    )
