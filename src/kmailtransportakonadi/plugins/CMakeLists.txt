set(mailtransport_akonadiplugin_SRCS
   akonadimailtransportplugin.cpp
)

kcoreaddons_add_plugin(mailtransport_akonadiplugin JSON akonadimailtransport.json SOURCES
    ${mailtransport_akonadiplugin_SRCS} INSTALL_NAMESPACE mailtransport)

target_link_libraries(mailtransport_akonadiplugin
  KF5::MailTransport KF5::CoreAddons
)
