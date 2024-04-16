add_library(KPim6MailTransport)
add_library(KPim6::MailTransport ALIAS KPim6MailTransport)

ecm_setup_version(PROJECT VARIABLE_PREFIX MAILTRANSPORT
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/mailtransport_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KPim6MailTransportConfigVersion.cmake"
    SOVERSION 6
    )

target_sources(KPim6MailTransport PRIVATE
    transport.cpp
    transportmanager.cpp
    transporttype.cpp
    transportjob.cpp
    precommandjob.cpp
    socket.cpp
    servertest.cpp
    outlookoauthtokenrequester.cpp
    )

target_sources(KPim6MailTransport PRIVATE
    widgets/transportconfigwidget.cpp
    widgets/transportcombobox.cpp
    widgets/transportlistwidget.cpp
    widgets/transportmanagementwidget.cpp
    widgets/addtransportdialogng.cpp

    widgets/addtransportdialogng.h
    widgets/transportcombobox.h
    widgets/transportlistwidget.h
    widgets/transportconfigwidget_p.h
    widgets/transportconfigwidget.h
    widgets/transportmanagementwidget.h

    widgets/transporttreeview.h
    widgets/transporttreeview.cpp

    widgets/transporttreedelegate.h
    widgets/transporttreedelegate.cpp

    transportactivitiesabstract.h
    transportactivitiesabstract.cpp
    transportmodel.h
    transportmodel.cpp
    transportsortproxymodel.h
    transportsortproxymodel.cpp


    )

target_sources(KPim6MailTransport PRIVATE
    plugins/transportabstractplugin.cpp
    plugins/transportpluginmanager.cpp
    ) 
target_sources(KPim6MailTransport PRIVATE
    transporttype_p.h
    mailtransport_defs.h
    socket.h
    outlookoauthtokenrequester.h
    precommandjob.h
    transport_p.h
    transport.h
    kmailtransport_private_export.h
    plugins/transportpluginmanager.h
    plugins/transportabstractplugin.h
    transportjob.h
    transportmanager.h
    transporttype.h
    servertest.h
    )

ecm_qt_declare_logging_category(KPim6MailTransport HEADER mailtransport_debug.h IDENTIFIER MAILTRANSPORT_LOG CATEGORY_NAME org.kde.pim.mailtransport
        DESCRIPTION "kmailtransport (kmailtransport)"
        OLD_CATEGORY_NAMES log_mailtransport
        EXPORT MAILTRANSPORT
    )

ki18n_wrap_ui(KPim6MailTransport
    ui/addtransportdialog.ui
    ui/transportmanagementwidget.ui
    )

kconfig_add_kcfg_files(KPim6MailTransport GENERATE_MOC transportbase.kcfgc)

if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(KPim6MailTransport PROPERTIES UNITY_BUILD ON)
endif()

generate_export_header(KPim6MailTransport BASE_NAME mailtransport)

target_include_directories(KPim6MailTransport INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/KPim6/MailTransport>")
target_include_directories(KPim6MailTransport PUBLIC "$<BUILD_INTERFACE:${MailTransport_SOURCE_DIR}/src;${MailTransport_BINARY_DIR}/src>")

target_link_libraries(KPim6MailTransport
    PUBLIC
    KF6::ConfigGui
    PRIVATE
    KF6::I18n
    KF6::WidgetsAddons
    KF6::CoreAddons
    KF6::ConfigWidgets
    Qt::DBus
    Qt::Network
    )

if (TARGET KF6::Wallet)
    target_link_libraries(KPim6MailTransport PRIVATE KF6::Wallet)
endif()

target_link_libraries(KPim6MailTransport PRIVATE qt6keychain)


set_target_properties(KPim6MailTransport PROPERTIES
    VERSION ${MAILTRANSPORT_VERSION}
    SOVERSION ${MAILTRANSPORT_SOVERSION}
    EXPORT_NAME MailTransport
    )


install(TARGETS KPim6MailTransport EXPORT KPim6MailTransportTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES mailtransport.kcfg DESTINATION ${KDE_INSTALL_KCFGDIR})


ecm_generate_headers(MailTransport_CamelCase_HEADERS
    HEADER_NAMES
    PrecommandJob
    ServerTest
    Transport
    TransportJob
    TransportManager
    TransportType
    TransportModel
    OutlookOAuthTokenRequester
    TransportActivitiesAbstract
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
    ${MailTransport_SOURCE_DIR}/src/widgets/transportconfigwidget_p.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim6/MailTransport/mailtransport/private COMPONENT Devel )

install(FILES
    ${MailTransport_CamelCase_HEADERS}
    ${MailTransport_widgets_CamelCase_HEADERS}
    ${MailTransport_plugins_CamelCase_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim6/MailTransport/MailTransport/ COMPONENT Devel )

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/mailtransport_export.h
    ${MailTransport_HEADERS}
    ${MailTransport_widgets_HEADERS}
    ${MailTransport_plugins_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/transportbase.h

    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim6/MailTransport/mailtransport COMPONENT Devel
    )


set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KPim6MailTransport")

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/mailtransport_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim6/MailTransport COMPONENT Devel
    )


if(BUILD_TESTING)
    add_subdirectory(tests)
    add_subdirectory(autotests)
    add_subdirectory(plugins/tests)
endif()

if (BUILD_QCH)
    ecm_add_qch(
        KPim6MailTransport_QCH
        NAME KMailTransport
        BASE_NAME KPim6MailTransport
        VERSION ${PIM_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
            ${MailTransport_HEADERS}
            ${MailTransport_widgets_HEADERS}
            ${MailTransport_plugins_HEADERS}
        #MD_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md"
        #IMAGE_DIRS "${CMAKE_SOURCE_DIR}/docs/pics"
        LINK_QCHS
            Qt6Core_QCH
            Qt6Gui_QCH
            Qt6Widgets_QCH
        INCLUDE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}
        BLANK_MACROS
            MAILTRANSPORT_EXPORT
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )
endif()
if (BUILD_QCH)
    ecm_install_qch_export(
        TARGETS KPim6MailTransport_QCH
        FILE KPim6MailTransportQchTargets.cmake
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
    )
    set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KPim6MailTransportQchTargets.cmake\")")
endif()


configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KPimMailTransportConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6MailTransportConfig.cmake"
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
    )

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6MailTransportConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6MailTransportConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
    )

install(EXPORT KPim6MailTransportTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KPim6MailTransportTargets.cmake NAMESPACE KPim6::)

add_subdirectory(plugins/smtp)
