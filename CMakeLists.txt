cmake_minimum_required(VERSION 3.0)
set(PIM_VERSION "5.4.81")

project(MailTransport VERSION ${PIM_VERSION})

# ECM setup
set(KF5_VERSION "5.32.0")
find_package(ECM ${KF5_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${MailTransport_SOURCE_DIR}/cmake ${ECM_MODULE_PATH})

include(GenerateExportHeader)
include(ECMGenerateHeaders)
include(ECMGeneratePriFile)
include(CMakePackageConfigHelpers)
include(ECMSetupVersion)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(ECMQtDeclareLoggingCategory)
include(ECMCoverageOption)


add_definitions(-DTRANSLATION_DOMAIN=\"libmailtransport5\")



set(KMAILTRANSPORT_LIB_VERSION ${PIM_VERSION})

set(KMIME_LIB_VERSION "5.4.80")
set(AKONADI_LIB_VERSION "5.4.80")
set(AKONADIMIME_LIB_VERSION "5.4.80")
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5MailTransport")

########### Find packages ###########
find_package(KF5KCMUtils ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5ConfigWidgets ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5Wallet ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5DocTools ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5I18n ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5KIO ${KF5_VERSION} CONFIG REQUIRED)
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

########### Targets ###########
add_subdirectory(cmake)
add_subdirectory(src)
add_subdirectory(kioslave)

install( FILES kmailtransport.renamecategories kmailtransport.categories DESTINATION ${KDE_INSTALL_CONFDIR} )
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

