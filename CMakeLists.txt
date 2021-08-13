cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
set(PIM_VERSION "5.18.1")
project(MailTransport VERSION ${PIM_VERSION})

# ECM setup
set(KF5_MIN_VERSION "5.83.0")

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
include(KDEGitCommitHooks)
include(ECMQtDeclareLoggingCategory)


add_definitions(-DTRANSLATION_DOMAIN=\"libmailtransport5\")

set(KMAILTRANSPORT_LIB_VERSION ${PIM_VERSION})

set(KMIME_LIB_VERSION "5.18.1")
set(AKONADI_LIB_VERSION "5.18.1")
set(AKONADIMIME_LIB_VERSION "5.18.1")
set(KSMTP_LIB_VERSION "5.18.1")
set(KGAPI_LIB_VERSION "5.18.1")

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5MailTransport")

########### Find packages ###########
find_package(KF5KCMUtils ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5ConfigWidgets ${KF5_MIN_VERSION} CONFIG REQUIRED)
#remove in the future
find_package(KF5Wallet ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5I18n ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5KIO ${KF5_MIN_VERSION} CONFIG REQUIRED)
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
                                   TYPE REQUIRED)

option(KDEPIM_RUN_AKONADI_TEST "Enable autotest based on Akonadi." ON)
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050f00)
add_definitions(-DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x055400)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(BUILD_TESTING)
    find_package(Qt5Test CONFIG REQUIRED)
   add_definitions(-DBUILD_TESTING)
endif(BUILD_TESTING)

option(USE_UNITY_CMAKE_SUPPORT "Use UNITY cmake support (speedup compile time)" OFF)

set(COMPILE_WITH_UNITY_CMAKE_SUPPORT OFF)
if (USE_UNITY_CMAKE_SUPPORT)
    set(COMPILE_WITH_UNITY_CMAKE_SUPPORT ON)
endif()


########### Targets ###########
add_subdirectory(src)

ecm_qt_install_logging_categories(
        EXPORT MAILTRANSPORT
        FILE kmailtransport.categories
        DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
        )
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
ki18n_install(po)
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

