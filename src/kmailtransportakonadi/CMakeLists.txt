add_library(KF5MailTransportAkonadi)
add_library(KF5::MailTransportAkonadi ALIAS KF5MailTransportAkonadi)


ecm_setup_version(PROJECT VARIABLE_PREFIX MAILTRANSPORT
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/mailtransportakonadi_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5MailTransportAkonadiConfigVersion.cmake"
    SOVERSION 5
    )




target_sources(KF5MailTransportAkonadi PRIVATE
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
    )

ecm_qt_declare_logging_category(KF5MailTransportAkonadi HEADER mailtransportakonadi_debug.h IDENTIFIER MAILTRANSPORTAKONADI_LOG CATEGORY_NAME org.kde.pim.mailtransportakonadi
        DESCRIPTION "kmailtransportakonadi (kmailtransportakonadi)"
        EXPORT MAILTRANSPORT
    )

if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(KF5MailTransportAkonadi PROPERTIES UNITY_BUILD ON)
endif()
generate_export_header(KF5MailTransportAkonadi BASE_NAME mailtransportakonadi)

target_include_directories(KF5MailTransportAkonadi INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/MailTransportAkonadi;${KDE_INSTALL_INCLUDEDIR_KF5}/mailtransportakonadi>")
target_include_directories(KF5MailTransportAkonadi PUBLIC "$<BUILD_INTERFACE:${MailTransport_SOURCE_DIR}/src;${MailTransport_BINARY_DIR}/src>")


target_link_libraries(KF5MailTransportAkonadi
    PUBLIC
    KF5::AkonadiCore
    KF5::Mime
    KF5::AkonadiMime
    KF5::MailTransport
    PRIVATE
    KF5::I18n
    KF5::CoreAddons
    KF5::ConfigGui
    )

set_target_properties(KF5MailTransportAkonadi PROPERTIES
    VERSION ${MAILTRANSPORT_VERSION}
    SOVERSION ${MAILTRANSPORT_SOVERSION}
    EXPORT_NAME MailTransportAkonadi
    )


install(TARGETS KF5MailTransportAkonadi EXPORT KF5MailTransportAkonadiTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

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
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/MailTransportAkonadi COMPONENT Devel )

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/mailtransportakonadi_export.h
    ${MailTransport_kmailtransportakonadi_HEADERS}

    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/mailtransportakonadi COMPONENT Devel
    )

ecm_generate_pri_file(BASE_NAME KMailTransportAkonadi LIB_NAME KF5MailTransportAkonadi DEPS "KMailTransport AkonadiCore KMime AkonadiMime" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/MailTransportAkonadi/)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})


set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5MailTransportAkonadi")

install(EXPORT KF5MailTransportAkonadiTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KF5MailTransportAkonadiTargets.cmake NAMESPACE KF5::)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KF5MailTransportAkonadiConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KF5MailTransportAkonadiConfig.cmake"
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
    )

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KF5MailTransportAkonadiConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KF5MailTransportAkonadiConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
    )


install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/mailtransportakonadi_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5} COMPONENT Devel
    )

if(BUILD_TESTING)
    add_subdirectory(tests)
    if (KDEPIM_RUN_AKONADI_TEST)
	    add_subdirectory(autotests)
    endif()
endif()

add_subdirectory(plugins)
