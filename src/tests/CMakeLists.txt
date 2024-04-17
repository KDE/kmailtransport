
include(ECMMarkAsTest)

add_executable(transportmgr transportmgr.cpp transportmgr.h)
ecm_mark_as_test(transportmgr)
target_link_libraries(transportmgr KPim6MailTransport Qt::Widgets KF6::I18n KF6::ConfigGui
    KF6::CoreAddons)

####
add_executable(servertest servertest.cpp)
ecm_mark_as_test(servertest)
target_link_libraries(servertest KPim6MailTransport KF6::I18n KF6::ConfigGui Qt::Widgets)

####
add_executable(addtransportdialogng_gui addtransportdialogng_gui.cpp addtransportdialogng_gui.h)
ecm_mark_as_test(addtransportdialogng_gui)
target_link_libraries(addtransportdialogng_gui KPim6MailTransport KF6::I18n KF6::ConfigGui Qt::Widgets)

####
add_executable(transportmanagementwidget_gui transportmanagementwidget_gui.cpp)
ecm_mark_as_test(transportmanagementwidget_gui)
target_link_libraries(transportmanagementwidget_gui KPim6MailTransport KF6::I18n KF6::ConfigGui Qt::Widgets)

####
add_executable(transportcombobox_gui transportcombobox_gui.cpp)
ecm_mark_as_test(transportcombobox_gui)
target_link_libraries(transportcombobox_gui KPim6MailTransport KF6::I18n KF6::ConfigGui Qt::Widgets)

####
add_executable(transporttreeview_gui transporttreeview_gui.cpp)
ecm_mark_as_test(transporttreeview_gui)
target_link_libraries(transporttreeview_gui KPim6MailTransport KF6::I18n Qt::Widgets Qt::Gui)

####
add_executable(transportmanagementwidgetng_gui transportmanagementwidgetng_gui.cpp)
ecm_mark_as_test(transportmanagementwidgetng_gui)
target_link_libraries(transportmanagementwidgetng_gui KPim6MailTransport KF6::I18n Qt::Widgets Qt::Gui)

