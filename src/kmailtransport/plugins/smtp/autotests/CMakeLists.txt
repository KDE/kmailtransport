include(ECMAddTests)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/..)

ecm_add_test(smtpjobtest.cpp
             ../smtpjob.cpp
             ${CMAKE_CURRENT_BINARY_DIR}/../mailtransportplugin_smtp_debug.cpp
             fakeserver.cpp
    LINK_LIBRARIES Qt${QT_MAJOR_VERSION}::Network
                   Qt${QT_MAJOR_VERSION}::Test
                   KF5::MailTransport
                   KF5::I18n
                   KF5::ConfigWidgets
                   KF5::KIOWidgets
                   KPim::SMTP
                   KPim::GAPICore
    TEST_NAME smtpjobtest
)
