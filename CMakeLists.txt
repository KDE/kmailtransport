cmake_minimum_required(VERSION 3.0)
set(PIM_VERSION "5.10.0")
project(MailTransport VERSION ${PIM_VERSION})

# ECM setup
set(KF5_VERSION "5.51.0")

find_package(ECM ${KF5_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(GenerateExportHeader)
include(ECMGenerateHeaders)
include(ECMGeneratePriFile)

include(ECMSetupVersion)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(ECMQtDeclareLoggingCategory)


add_definitions(-DTRANSLATION_DOMAIN=\"libmailtransport5\")

set(KMAILTRANSPORT_LIB_VERSION ${PIM_VERSION})

set(KMIME_LIB_VERSION "5.10.0")
set(AKONADI_LIB_VERSION "5.10.0")
set(AKONADIMIME_LIB_VERSION "5.10.0")
set(KSMTP_LIB_VERSION "5.10.0")
set(KGAPI_LIB_VERSION "5.10.0")

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5MailTransport")

########### Find packages ###########
find_package(KF5KCMUtils ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5ConfigWidgets ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5Wallet ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5I18n ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5KIO ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5Mime ${KMIME_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5Akonadi ${AKONADI_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5AkonadiMime ${AKONADIMIME_LIB_VERSION} CONFIG REQUIRED)
find_package(KPimSMTP ${KSMTP_LIB_VERSION} CONFIG REQUIRED)
find_package(KPimGAPI ${KGAPI_LIB_VERSION} CONFIG REQUIRED)


add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x060000)

if(BUILD_TESTING)
   add_definitions(-DBUILD_TESTING)
endif(BUILD_TESTING)

########### Targets ###########
add_subdirectory(src)

install( FILES kmailtransport.renamecategories kmailtransport.categories DESTINATION ${KDE_INSTALL_CONFDIR} )
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

