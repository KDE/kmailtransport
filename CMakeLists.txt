cmake_minimum_required(VERSION 2.8.12)

project(MailTransport)

# ECM setup
set(KF5_VERSION "5.23.0")
find_package(ECM ${KF5_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${MailTransport_SOURCE_DIR}/cmake ${ECM_MODULE_PATH})

include(GenerateExportHeader)
include(ECMGenerateHeaders)
include(ECMGeneratePriFile)
include(ECMPackageConfigHelpers)
include(ECMSetupVersion)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(ECMQtDeclareLoggingCategory)

add_definitions(-DTRANSLATION_DOMAIN=\"libmailtransport5\")


set(PIM_VERSION "5.2.80")

set(KMAILTRANSPORT_LIB_VERSION ${PIM_VERSION})

set(KMIME_LIB_VERSION "5.2.80")
set(AKONADI_LIB_VERSION "5.2.80")
set(AKONADIMIME_LIB_VERSION "5.2.80")
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5MailTransport")

ecm_setup_version(${KMAILTRANSPORT_LIB_VERSION} VARIABLE_PREFIX MAILTRANSPORT
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/mailtransport_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5MailTransportConfigVersion.cmake"
                        SOVERSION 5
)

########### Find packages ###########
find_package(KF5KCMUtils ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5ConfigWidgets ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5Wallet ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5DocTools ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5Mime ${KMIME_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5Akonadi ${AKONADI_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5AkonadiMime ${AKONADIMIME_LIB_VERSION} CONFIG REQUIRED)

add_definitions("-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII")

find_package(Sasl2)
set_package_properties(Sasl2 PROPERTIES
                             DESCRIPTION "The Cyrus-sasl library"
                             URL "http://www.cyrussasl.org"
                             TYPE OPTIONAL
)

ecm_configure_package_config_file(
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

install(FILES
   ${CMAKE_CURRENT_BINARY_DIR}/mailtransport_version.h
   DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}
   COMPONENT Devel
)


install(EXPORT KF5MailTransportTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KF5MailTransportTargets.cmake NAMESPACE KF5::)

########### Targets ###########
add_subdirectory(cmake)
add_subdirectory(src)
add_subdirectory(kioslave)
if(BUILD_TESTING)
    add_subdirectory(tests)
    add_subdirectory(autotests)
endif()

install( FILES kmailtransport.categories DESTINATION ${KDE_INSTALL_CONFDIR} )
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

