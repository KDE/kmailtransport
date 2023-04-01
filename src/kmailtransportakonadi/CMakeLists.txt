add_library(KPim${KF_MAJOR_VERSION}MailTransportAkonadi)
add_library(KPim${KF_MAJOR_VERSION}::MailTransportAkonadi ALIAS KPim${KF_MAJOR_VERSION}MailTransportAkonadi)

ecm_setup_version(PROJECT VARIABLE_PREFIX MAILTRANSPORT
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/mailtransportakonadi_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KPim${KF_MAJOR_VERSION}MailTransportAkonadiConfigVersion.cmake"
    SOVERSION 5
    )

target_sources(KPim${KF_MAJOR_VERSION}MailTransportAkonadi PRIVATE
    dispatcherinterface.cpp
    attributeregistrar.cpp
    dispatchmodeattribute.cpp
    errorattribute.cpp
    transportattribute.cpp
    sentactionattribute.cpp
    sentbehaviourattribute.cpp
    messagequeuejob.cpp
    outboxactions.cpp
    filteractionjob.cpp

    filteractionjob_p.h
    errorattribute.h
    outboxactions_p.h
    dispatcherinterface_p.h
    sentbehaviourattribute.h
    messagequeuejob.h
    sentactionattribute.h
    transportattribute.h
    dispatchmodeattribute.h
    dispatcherinterface.h

    )

ecm_qt_declare_logging_category(KPim${KF_MAJOR_VERSION}MailTransportAkonadi HEADER mailtransportakonadi_debug.h IDENTIFIER MAILTRANSPORTAKONADI_LOG CATEGORY_NAME org.kde.pim.mailtransportakonadi
        DESCRIPTION "kmailtransportakonadi (kmailtransportakonadi)"
        EXPORT MAILTRANSPORT
    )

if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(KPim${KF_MAJOR_VERSION}MailTransportAkonadi PROPERTIES UNITY_BUILD ON)
endif()
generate_export_header(KPim${KF_MAJOR_VERSION}MailTransportAkonadi BASE_NAME mailtransportakonadi)

target_include_directories(KPim${KF_MAJOR_VERSION}MailTransportAkonadi INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/KPim${KF_MAJOR_VERSION}/MailTransportAkonadi>")
target_include_directories(KPim${KF_MAJOR_VERSION}MailTransportAkonadi PUBLIC "$<BUILD_INTERFACE:${MailTransport_SOURCE_DIR}/src;${MailTransport_BINARY_DIR}/src>")


target_link_libraries(KPim${KF_MAJOR_VERSION}MailTransportAkonadi
    PUBLIC
    KPim${KF_MAJOR_VERSION}::AkonadiCore
    KPim${KF_MAJOR_VERSION}::Mime
    KPim${KF_MAJOR_VERSION}::AkonadiMime
    PRIVATE
    KF${KF_MAJOR_VERSION}::I18n
    KF${KF_MAJOR_VERSION}::CoreAddons
    )

set_target_properties(KPim${KF_MAJOR_VERSION}MailTransportAkonadi PROPERTIES
    VERSION ${MAILTRANSPORT_VERSION}
    SOVERSION ${MAILTRANSPORT_SOVERSION}
    EXPORT_NAME MailTransportAkonadi
    )


install(TARGETS KPim${KF_MAJOR_VERSION}MailTransportAkonadi EXPORT KPim${KF_MAJOR_VERSION}MailTransportAkonadiTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

ecm_generate_headers(MailTransport_kmailtransportakonadi_CamelCase_HEADERS
    HEADER_NAMES
    DispatcherInterface
    MessageQueueJob
    TransportAttribute
    SentBehaviourAttribute
    DispatchModeAttribute
    ErrorAttribute
    SentActionAttribute
    PREFIX MailTransportAkonadi
    REQUIRED_HEADERS MailTransport_kmailtransportakonadi_HEADERS
    )

install(FILES
    ${MailTransport_kmailtransportakonadi_CamelCase_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim${KF_MAJOR_VERSION}/MailTransportAkonadi/MailTransportAkonadi COMPONENT Devel )

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/mailtransportakonadi_export.h
    ${MailTransport_kmailtransportakonadi_HEADERS}

    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim${KF_MAJOR_VERSION}/MailTransportAkonadi/mailtransportakonadi COMPONENT Devel
    )

ecm_generate_pri_file(BASE_NAME KMailTransportAkonadi LIB_NAME KF${KF_MAJOR_VERSION}MailTransportAkonadi DEPS "KMailTransport AkonadiCore KMime AkonadiMime" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR}/KPim${KF_MAJOR_VERSION}/MailTransportAkonadi/)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})


set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KPim${KF_MAJOR_VERSION}MailTransportAkonadi")
set(MAILTRANSPORTAKONADI_KF5_COMPAT FALSE)
install(EXPORT KPim${KF_MAJOR_VERSION}MailTransportAkonadiTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    FILE KPim${KF_MAJOR_VERSION}MailTransportAkonadiTargets.cmake NAMESPACE KPim${KF_MAJOR_VERSION}::)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KPimMailTransportAkonadiConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim${KF_MAJOR_VERSION}MailTransportAkonadiConfig.cmake"
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
    )

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KPim${KF_MAJOR_VERSION}MailTransportAkonadiConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim${KF_MAJOR_VERSION}MailTransportAkonadiConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
    )


install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/mailtransportakonadi_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim${KF_MAJOR_VERSION}/MailTransportAkonadi COMPONENT Devel
    )

if(BUILD_TESTING)
    add_subdirectory(tests)
    add_subdirectory(autotests)
endif()

if (BUILD_WITH_COMPAT_LIBS)
if (QT_MAJOR_VERSION STREQUAL "5")
##
# TODO: Backwards compatiblity. Remove in next major version
##
set(CMAKECONFIG_INSTALL_DIR_KF5 "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5MailTransportAkonadi")
set(MAILTRANSPORTAKONADI_KF5_COMPAT TRUE)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KPimMailTransportAkonadiConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KF5MailTransportAkonadiConfig.cmake"
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR_KF5}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KF5MailTransportAkonadiConfig.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR_KF5}"
    COMPONENT Devel
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KPim${KF_MAJOR_VERSION}MailTransportAkonadiConfigVersion.cmake"
    RENAME "KF5MailTransportAkonadiConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR_KF5}"
    COMPONENT Devel
)
install(EXPORT KPim${KF_MAJOR_VERSION}MailTransportAkonadiTargets
    DESTINATION "${CMAKECONFIG_INSTALL_DIR_KF5}"
    FILE KPim${KF_MAJOR_VERSION}MailTransportAkonadiTargets.cmake
    NAMESPACE KF5::
)
endif()
endif()
