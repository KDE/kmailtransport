
include(ECMMarkAsTest)

find_package(KF5TextWidgets ${KF5_MIN_VERSION} CONFIG REQUIRED)

add_executable(transportmgr transportmgr.cpp)
ecm_mark_as_test(transportmgr)
target_link_libraries(transportmgr KF5MailTransport Qt::Widgets KF5::I18n KF5::ConfigGui KF5::TextWidgets KF5::CoreAddons)

add_executable(servertest servertest.cpp)
ecm_mark_as_test(servertest)
target_link_libraries(servertest KF5MailTransport KF5::I18n KF5::ConfigGui Qt::Widgets)


add_executable(addtransportdialogng_gui addtransportdialogng_gui.cpp)
ecm_mark_as_test(addtransportdialogng_gui)
target_link_libraries(addtransportdialogng_gui KF5MailTransport KF5::I18n KF5::ConfigGui Qt::Widgets)
