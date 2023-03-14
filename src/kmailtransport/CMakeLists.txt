add_library(KPim${KF_MAJOR_VERSION}MailTransport)
add_library(KPim${KF_MAJOR_VERSION}::MailTransport ALIAS KPim${KF_MAJOR_VERSION}MailTransport)

ecm_setup_version(PROJECT VARIABLE_PREFIX MAILTRANSPORT
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/mailtransport_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KPim${KF_MAJOR_VERSION}MailTransportConfigVersion.cmake"
    SOVERSION 5
    )

target_sources(KPim${KF_MAJOR_VERSION}MailTransport PRIVATE
    transport.cpp
    transportmanager.cpp
    transporttype.cpp
    transportjob.cpp
    precommandjob.cpp
    socket.cpp
    servertest.cpp
    )

target_sources(KPim${KF_MAJOR_VERSION}MailTransport PRIVATE
    widgets/transportconfigwidget.cpp
    widgets/transportcombobox.cpp
    widgets/transportlistview.cpp
    widgets/transportmanagementwidget.cpp
    widgets/addtransportdialogng.cpp
    )

target_sources(KPim${KF_MAJOR_VERSION}MailTransport PRIVATE
    plugins/transportabstractplugin.cpp
    plugins/transportpluginmanager.cpp
    ) 
target_sources(KPim${KF_MAJOR_VERSION}MailTransport PRIVATE
    transporttype_p.h
    mailtransport_defs.h
    socket.h
    precommandjob.h
    transport_p.h
    transport.h
    kmailtransport_private_export.h
    plugins/transportpluginmanager.h
    plugins/transportabstractplugin.h
    widgets/addtransportdialogng.h
    widgets/transportcombobox.h
    widgets/transportlistview.h
    widgets/transportconfigwidget_p.h
    widgets/transportconfigwidget.h
    widgets/transportmanagementwidget.h
    transportjob.h
    transportmanager.h
    transporttype.h
    servertest.h
    )

ecm_qt_declare_logging_category(KPim${KF_MAJOR_VERSION}MailTransport HEADER mailtransport_debug.h IDENTIFIER MAILTRANSPORT_LOG CATEGORY_NAME org.kde.pim.mailtransport
        DESCRIPTION "kmailtransport (kmailtransport)"
        OLD_CATEGORY_NAMES log_mailtransport
        EXPORT MAILTRANSPORT
    )

ki18n_wrap_ui(KPim${KF_MAJOR_VERSION}MailTransport
    ui/addtransportdialog.ui
    ui/transportmanagementwidget.ui
    )

kconfig_add_kcfg_files(KPim${KF_MAJOR_VERSION}MailTransport GENERATE_MOC transportbase.kcfgc)

if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(KPim${KF_MAJOR_VERSION}MailTransport PROPERTIES UNITY_BUILD ON)
endif()

generate_export_header(KPim${KF_MAJOR_VERSION}MailTransport BASE_NAME mailtransport)

target_include_directories(KPim${KF_MAJOR_VERSION}MailTransport INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/KPim${KF_MAJOR_VERSION}/MailTransport>")
target_include_directories(KPim${KF_MAJOR_VERSION}MailTransport PUBLIC "$<BUILD_INTERFACE:${MailTransport_SOURCE_DIR}/src;${MailTransport_BINARY_DIR}/src>")

target_link_libraries(KPim${KF_MAJOR_VERSION}MailTransport
    PUBLIC
    KF${KF_MAJOR_VERSION}::Wallet
    PRIVATE
    KF${KF_MAJOR_VERSION}::I18n
    KF${KF_MAJOR_VERSION}::ConfigGui
    KF${KF_MAJOR_VERSION}::WidgetsAddons
    KF${KF_MAJOR_VERSION}::CoreAddons
    KF${KF_MAJOR_VERSION}::Wallet
    KF${KF_MAJOR_VERSION}::ConfigWidgets
    Qt::DBus
    Qt::Network
    )

target_link_libraries(KPim${KF_MAJOR_VERSION}MailTransport PRIVATE qt${QT_MAJOR_VERSION}keychain)


set_target_properties(KPim${KF_MAJOR_VERSION}MailTransport PROPERTIES
    VERSION ${MAILTRANSPORT_VERSION}
    SOVERSION ${MAILTRANSPORT_SOVERSION}
    EXPORT_NAME MailTransport
    )


install(TARGETS KPim${KF_MAJOR_VERSION}MailTransport EXPORT KPim${KF_MAJOR_VERSION}MailTransportTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

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
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim${KF_MAJOR_VERSION}/MailTransport/mailtransport/private COMPONENT Devel )

install(FILES
    ${MailTransport_CamelCase_HEADERS}
    ${MailTransport_widgets_CamelCase_HEADERS}
    ${MailTransport_plugins_CamelCase_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim${KF_MAJOR_VERSION}/MailTransport/MailTransport/ COMPONENT Devel )

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/mailtransport_export.h
    ${MailTransport_HEADERS}
    ${MailTransport_widgets_HEADERS}
    ${MailTransport_plugins_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/transportbase.h

    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim${KF_MAJOR_VERSION}/MailTransport/mailtransport COMPONENT Devel
    )

ecm_generate_pri_file(BASE_NAME KMailTransport LIB_NAME KPim${KF_MAJOR_VERSION}MailTransport DEPS "KWallet" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR}/KPim${KF_MAJOR_VERSION}/MailTransport/)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})


set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KPim${KF_MAJOR_VERSION}MailTransport")
set(MAILTRANSPORT_KF5_COMPAT FALSE)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KPimMailTransportConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim${KF_MAJOR_VERSION}MailTransportConfig.cmake"
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
    )

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KPim${KF_MAJOR_VERSION}MailTransportConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim${KF_MAJOR_VERSION}MailTransportConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
    )

install(EXPORT KPim${KF_MAJOR_VERSION}MailTransportTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KPim${KF_MAJOR_VERSION}MailTransportTargets.cmake NAMESPACE KPim${KF_MAJOR_VERSION}::)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/mailtransport_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim${KF_MAJOR_VERSION}/MailTransport COMPONENT Devel
    )


if(BUILD_TESTING)
    add_subdirectory(tests)
    add_subdirectory(plugins/tests)
endif()

if (BUILD_QCH)
    ecm_add_qch(
        KPim${KF_MAJOR_VERSION}MailTransport_QCH
        NAME KMailTransport
        BASE_NAME KPim${KF_MAJOR_VERSION}MailTransport
        VERSION ${PIM_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
            ${MailTransport_HEADERS}
            ${MailTransport_widgets_HEADERS}
            ${MailTransport_plugins_HEADERS}
        #MD_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md"
        #IMAGE_DIRS "${CMAKE_SOURCE_DIR}/docs/pics"
        LINK_QCHS
            Qt${QT_MAJOR_VERSION}Core_QCH
            Qt${QT_MAJOR_VERSION}Gui_QCH
            Qt${QT_MAJOR_VERSION}Widgets_QCH
        INCLUDE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}
        BLANK_MACROS
            MAILTRANSPORT_EXPORT
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )
endif()

add_subdirectory(plugins/smtp)
if (BUILD_WITH_COMPAT_LIBS)
if (QT_MAJOR_VERSION STREQUAL "5")
##
# TODO: Backwards compatiblity. Remove in next major version
##
set(CMAKECONFIG_INSTALL_DIR_KF5 "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5MailTransport")
set(MAILTRANSPORT_KF5_COMPAT TRUE)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KPimMailTransportConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KF5MailTransportConfig.cmake"
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR_KF5}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KF5MailTransportConfig.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR_KF5}"
    COMPONENT Devel
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KPim${KF_MAJOR_VERSION}MailTransportConfigVersion.cmake"
    RENAME "KF5MailTransportConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR_KF5}"
    COMPONENT Devel
)
install(EXPORT KPim${KF_MAJOR_VERSION}MailTransportTargets
    DESTINATION "${CMAKECONFIG_INSTALL_DIR_KF5}"
    FILE KPim${KF_MAJOR_VERSION}MailTransportTargets.cmake
    NAMESPACE KF5::
)
endif()
endif()
