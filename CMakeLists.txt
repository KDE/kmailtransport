cmake_minimum_required(VERSION 3.5)
set(PIM_VERSION "5.16.40")
project(MailTransport VERSION ${PIM_VERSION})

# ECM setup
set(KF5_MIN_VERSION "5.75.0")

find_package(ECM ${KF5_MIN_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)

include(GenerateExportHeader)
include(ECMGenerateHeaders)
include(ECMGeneratePriFile)

include(ECMSetupVersion)
include(FeatureSummary)
include(ECMQtDeclareLoggingCategory)


add_definitions(-DTRANSLATION_DOMAIN=\"libmailtransport5\")

set(KMAILTRANSPORT_LIB_VERSION ${PIM_VERSION})

set(KMIME_LIB_VERSION "5.15.80")
set(AKONADI_LIB_VERSION "5.15.80")
set(AKONADIMIME_LIB_VERSION "5.15.80")
set(KSMTP_LIB_VERSION "5.15.80")
set(KGAPI_LIB_VERSION "5.15.80")

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5MailTransport")

########### Find packages ###########
find_package(KF5KCMUtils ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5ConfigWidgets ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5Wallet ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5I18n ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5KIO ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5Service ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5Mime ${KMIME_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5Akonadi ${AKONADI_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5AkonadiMime ${AKONADIMIME_LIB_VERSION} CONFIG REQUIRED)
find_package(KPimSMTP ${KSMTP_LIB_VERSION} CONFIG REQUIRED)
find_package(KPimGAPI ${KGAPI_LIB_VERSION} CONFIG REQUIRED)

#it will replace kwallet support
find_package(Qt5Keychain CONFIG)
set_package_properties(Qt5Keychain PROPERTIES
                                   DESCRIPTION "Provides support for secure credentials storage"
                                   URL "https://github.com/frankosterfeld/qtkeychain"
                                   TYPE OPTIONAL)

option(KDEPIM_RUN_AKONADI_TEST "Enable autotest based on Akonadi." TRUE)
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050f00)
add_definitions(-DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x054C00)

add_definitions(-DQT_NO_FOREACH)
add_definitions(-DQT_NO_KEYWORDS)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(BUILD_TESTING)
   add_definitions(-DBUILD_TESTING)
endif(BUILD_TESTING)

########### Targets ###########
add_subdirectory(src)

ecm_qt_install_logging_categories(
        EXPORT MAILTRANSPORT
        FILE kmailtransport.categories
        DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
        )
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

