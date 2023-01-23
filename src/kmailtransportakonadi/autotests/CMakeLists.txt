find_package(KF${KF_MAJOR_VERSION}DBusAddons CONFIG REQUIRED)

# Akonadi testrunner-based tests:
add_akonadi_isolated_test(
    SOURCE attributetest.cpp
    LINK_LIBRARIES KF5::MailTransportAkonadi
)
add_akonadi_isolated_test(
    SOURCE messagequeuejobtest.cpp
    LINK_LIBRARIES KF5::MailTransportAkonadi KF5::MailTransport KF${KF_MAJOR_VERSION}::I18n KF${KF_MAJOR_VERSION}::ConfigGui
)
# TODO: port & re-enable
#add_akonadi_isolated_test(
#    SOURCE filteractiontest.cpp
#    LINK_LIBRARIES KF5::AkonadiCore KF5::MailTransportAkonadi
#)

