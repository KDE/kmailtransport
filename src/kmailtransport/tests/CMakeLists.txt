
include(ECMMarkAsTest)

find_package(KF5TextWidgets ${KF5_VERSION} CONFIG REQUIRED)
find_package(Qt5Test CONFIG REQUIRED)

set(tm_srcs transportmgr.cpp)
add_executable(transportmgr ${tm_srcs})
ecm_mark_as_test(transportmgr)
target_link_libraries(transportmgr KF5MailTransport Qt5::Widgets KF5::I18n KF5::ConfigGui KF5::Completion KF5::TextWidgets KF5::CoreAddons)

add_executable(servertest servertest.cpp)
ecm_mark_as_test(servertest)
target_link_libraries(servertest KF5MailTransport KF5::I18n KF5::ConfigGui Qt5::Widgets)


add_executable(addtransportdialogng_gui addtransportdialogng_gui.cpp)
ecm_mark_as_test(addtransportdialogng_gui)
target_link_libraries(addtransportdialogng_gui KF5MailTransport KF5::I18n KF5::ConfigGui Qt5::Widgets)
