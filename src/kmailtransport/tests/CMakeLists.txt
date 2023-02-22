
include(ECMMarkAsTest)

find_package(KF${KF_MAJOR_VERSION}TextWidgets ${KF_MIN_VERSION} CONFIG REQUIRED)

add_executable(transportmgr transportmgr.cpp transportmgr.h)
ecm_mark_as_test(transportmgr)
target_link_libraries(transportmgr KPim${KF_MAJOR_VERSION}MailTransport Qt::Widgets KF${KF_MAJOR_VERSION}::I18n KF${KF_MAJOR_VERSION}::ConfigGui
    KF${KF_MAJOR_VERSION}::TextWidgets KF${KF_MAJOR_VERSION}::CoreAddons)

add_executable(servertest servertest.cpp)
ecm_mark_as_test(servertest)
target_link_libraries(servertest KPim${KF_MAJOR_VERSION}MailTransport KF${KF_MAJOR_VERSION}::I18n KF${KF_MAJOR_VERSION}::ConfigGui Qt::Widgets)


add_executable(addtransportdialogng_gui addtransportdialogng_gui.cpp addtransportdialogng_gui.h)
ecm_mark_as_test(addtransportdialogng_gui)
target_link_libraries(addtransportdialogng_gui KPim${KF_MAJOR_VERSION}MailTransport KF${KF_MAJOR_VERSION}::I18n KF${KF_MAJOR_VERSION}::ConfigGui Qt::Widgets)
