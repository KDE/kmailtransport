include(ECMAddTests)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/..)

ecm_add_test(smtpjobtest.cpp
             ../smtpjob.cpp
             ${CMAKE_CURRENT_BINARY_DIR}/../mailtransportplugin_smtp_debug.cpp
             fakeserver.cpp fakeserver.h
    LINK_LIBRARIES Qt::Network
                   Qt::Test
                   KPim6::MailTransport
                   KF6::I18n
                   KF6::ConfigWidgets
                   KF6::KIOWidgets
                   KPim6::SMTP
                   KPim6::GAPICore
    TEST_NAME smtpjobtest
)
