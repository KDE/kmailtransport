include(ECMAddTests)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/..)

ecm_add_test(smtpjobtest.cpp
             ../smtpjob.cpp
             ${CMAKE_CURRENT_BINARY_DIR}/../mailtransportplugin_smtp_debug.cpp
             fakeserver.cpp
    LINK_LIBRARIES Qt::Network
                   Qt::Test
                   KPim${KF_MAJOR_VERSION}::MailTransport
                   KF${KF_MAJOR_VERSION}::I18n
                   KF${KF_MAJOR_VERSION}::ConfigWidgets
                   KF${KF_MAJOR_VERSION}::KIOWidgets
                   KPim::SMTP
                   KPim::GAPICore
    TEST_NAME smtpjobtest
)
