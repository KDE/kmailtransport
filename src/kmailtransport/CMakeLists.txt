add_library(KF5MailTransport)
add_library(KF5::MailTransport ALIAS KF5MailTransport)
ecm_setup_version(PROJECT VARIABLE_PREFIX MAILTRANSPORT
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/mailtransport_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5MailTransportConfigVersion.cmake"
    SOVERSION 5
    )

target_sources(KF5MailTransport PRIVATE
    transport.cpp
    transportmanager.cpp
    transporttype.cpp
    transportjob.cpp
    precommandjob.cpp
    socket.cpp
    servertest.cpp
    )

target_sources(KF5MailTransport PRIVATE
    widgets/transportconfigwidget.cpp
    widgets/transportcombobox.cpp
    widgets/transportlistview.cpp
    widgets/transportmanagementwidget.cpp
    widgets/addtransportdialogng.cpp
    )

target_sources(KF5MailTransport PRIVATE
    plugins/transportabstractplugin.cpp
    plugins/transportpluginmanager.cpp
    ) 


ecm_qt_declare_logging_category(KF5MailTransport HEADER mailtransport_debug.h IDENTIFIER MAILTRANSPORT_LOG CATEGORY_NAME org.kde.pim.mailtransport
        DESCRIPTION "kmailtransport (kmailtransport)"
        OLD_CATEGORY_NAMES log_mailtransport
        EXPORT MAILTRANSPORT
    )

ki18n_wrap_ui(KF5MailTransport
    ui/addtransportdialog.ui
    ui/transportmanagementwidget.ui
    )

kconfig_add_kcfg_files(KF5MailTransport transportbase.kcfgc)

if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(KF5MailTransport PROPERTIES UNITY_BUILD ON)
endif()

generate_export_header(KF5MailTransport BASE_NAME mailtransport)

target_include_directories(KF5MailTransport INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/MailTransport;${KDE_INSTALL_INCLUDEDIR_KF5}/mailtransport>")
target_include_directories(KF5MailTransport PUBLIC "$<BUILD_INTERFACE:${MailTransport_SOURCE_DIR}/src;${MailTransport_BINARY_DIR}/src>")

target_link_libraries(KF5MailTransport
    PUBLIC
    KF5::Wallet
    PRIVATE
    KF5::I18n
    KF5::ConfigGui
    KF5::WidgetsAddons
    KF5::CoreAddons
    KF5::Wallet
    KF5::ConfigWidgets
    Qt::DBus
    Qt::Network
    qt5keychain
    )

set_target_properties(KF5MailTransport PROPERTIES
    VERSION ${MAILTRANSPORT_VERSION}
    SOVERSION ${MAILTRANSPORT_SOVERSION}
    EXPORT_NAME MailTransport
    )


install(TARGETS KF5MailTransport EXPORT KF5MailTransportTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES mailtransport.kcfg DESTINATION ${KDE_INSTALL_KCFGDIR})


ecm_generate_headers(MailTransport_CamelCase_HEADERS
    HEADER_NAMES
    PrecommandJob
    ServerTest
    Transport
    TransportJob
    TransportManager
    TransportType
    PREFIX MailTransport
    REQUIRED_HEADERS MailTransport_HEADERS
    )

ecm_generate_headers(MailTransport_widgets_CamelCase_HEADERS
    HEADER_NAMES
    TransportComboBox
    TransportManagementWidget
    TransportConfigWidget
    PREFIX MailTransport
    REQUIRED_HEADERS MailTransport_widgets_HEADERS
    RELATIVE widgets
    )

ecm_generate_headers(MailTransport_plugins_CamelCase_HEADERS
    HEADER_NAMES
    TransportAbstractPlugin
    PREFIX MailTransport
    REQUIRED_HEADERS MailTransport_plugins_HEADERS
    RELATIVE plugins
    )

install(FILES
    ${MailTransport_SOURCE_DIR}/src/kmailtransport/widgets/transportconfigwidget_p.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/mailtransport/private COMPONENT Devel )

install(FILES
    ${MailTransport_CamelCase_HEADERS}
    ${MailTransport_widgets_CamelCase_HEADERS}
    ${MailTransport_plugins_CamelCase_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/MailTransport/ COMPONENT Devel )

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/mailtransport_export.h
    ${MailTransport_HEADERS}
    ${MailTransport_widgets_HEADERS}
    ${MailTransport_plugins_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/transportbase.h

    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/mailtransport COMPONENT Devel
    )

ecm_generate_pri_file(BASE_NAME KMailTransport LIB_NAME KF5MailTransport DEPS "KWallet" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/MailTransport/)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})


set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5MailTransport")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KF5MailTransportConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KF5MailTransportConfig.cmake"
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
    )

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KF5MailTransportConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KF5MailTransportConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
    )

install(EXPORT KF5MailTransportTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KF5MailTransportTargets.cmake NAMESPACE KF5::)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/mailtransport_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5} COMPONENT Devel
    )


if(BUILD_TESTING)
    add_subdirectory(tests)
    add_subdirectory(plugins/tests)
endif()

add_subdirectory(plugins/smtp)
