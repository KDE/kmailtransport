set(mailtransport_akonadiplugin_SRCS
    akonadimailtransportplugin.cpp
    resourcesendjob.cpp
    )

kcoreaddons_add_plugin(mailtransport_akonadiplugin JSON akonadimailtransport.json SOURCES
    ${mailtransport_akonadiplugin_SRCS} INSTALL_NAMESPACE mailtransport)

target_link_libraries(mailtransport_akonadiplugin
    KF5::MailTransportAkonadi
    KF5::CoreAddons
    KF5::AkonadiCore
    KF5::I18n
    KF5::ConfigWidgets
    Qt5::DBus
    )
