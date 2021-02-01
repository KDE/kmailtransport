include(ECMAddTests)

find_package(Qt5Test ${QT_REQUIRED_VERSION} REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/..)

ecm_add_test(smtpjobtest.cpp
             ../smtpjob.cpp
             ${CMAKE_CURRENT_BINARY_DIR}/../mailtransportplugin_smtp_debug.cpp
             fakeserver.cpp
    LINK_LIBRARIES Qt5::Network
                   Qt5::Test
                   KF5::MailTransport
                   KF5::I18n
                   KF5::ConfigWidgets
                   KF5::KIOWidgets
                   KPim::SMTP
                   KPim::GAPICore
    TEST_NAME smtpjobtest
)
