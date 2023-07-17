cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
set(PIM_VERSION "5.24.40")
project(MailTransport VERSION ${PIM_VERSION})

# ECM setup
set(KF_MIN_VERSION "5.105.0")

find_package(ECM ${KF_MIN_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(GenerateExportHeader)
include(ECMGenerateHeaders)
include(ECMGeneratePriFile)

include(ECMSetupVersion)
include(FeatureSummary)
include(KDEGitCommitHooks)
include(KDEClangFormat)
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h *.c)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

include(ECMQtDeclareLoggingCategory)
include(ECMDeprecationSettings)
include(ECMAddQch)

if (QT_MAJOR_VERSION STREQUAL "6")
    set(QT_REQUIRED_VERSION "6.4.0")
    set(KF_MIN_VERSION "5.240.0")
    set(KF_MAJOR_VERSION "6")
else()
    set(KF_MAJOR_VERSION "5")
endif()
option(BUILD_WITH_COMPAT_LIBS "Generate compat cmake file (Set at false will allow to co-install lib)" ON)

option(BUILD_QCH "Build API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)" OFF)
add_feature_info(QCH ${BUILD_QCH} "API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)")


add_definitions(-DTRANSLATION_DOMAIN=\"libmailtransport5\")

set(KMAILTRANSPORT_LIB_VERSION ${PIM_VERSION})

set(KPIM_MIME_VERSION "5.23.40")
set(AKONADI_LIB_VERSION "5.23.40")
set(AKONADIMIME_LIB_VERSION "5.23.40")
set(KSMTP_LIB_VERSION "5.23.40")
set(KGAPI_LIB_VERSION "5.23.40")

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5MailTransport")

########### Find packages ###########
find_package(KF${KF_MAJOR_VERSION}ConfigWidgets ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF${KF_MAJOR_VERSION}I18n ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF${KF_MAJOR_VERSION}KIO ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KPim${KF_MAJOR_VERSION}Mime ${KPIM_MIME_VERSION} CONFIG REQUIRED)

find_package(KPim${KF_MAJOR_VERSION}SMTP ${KSMTP_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim${KF_MAJOR_VERSION}GAPI ${KGAPI_LIB_VERSION} CONFIG REQUIRED)

find_package(Qt${QT_MAJOR_VERSION}Keychain CONFIG)
set_package_properties(Qt${QT_MAJOR_VERSION}Keychain PROPERTIES
                                   DESCRIPTION "Provides support for secure credentials storage"
                                   URL "https://github.com/frankosterfeld/qtkeychain"
                                   TYPE REQUIRED)
if (Qt${QT_MAJOR_VERSION}Keychain_VERSION VERSION_LESS 0.14.1)
    find_package(KF${KF_MAJOR_VERSION}Wallet ${KF_MIN_VERSION} CONFIG REQUIRED)
    add_definitions(-DUSE_LEGACY_WALLET_CODE=1)
else()
    add_definitions(-DUSE_LEGACY_WALLET_CODE=0)
endif()

ecm_set_disabled_deprecation_versions(QT 6.4  KF 5.105.0)


if(BUILD_TESTING)
    find_package(Qt${QT_MAJOR_VERSION}Test CONFIG REQUIRED)
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

if (BUILD_QCH)
    ecm_install_qch_export(
        TARGETS KPim${KF_MAJOR_VERSION}MailTransport_QCH
        FILE KPim${KF_MAJOR_VERSION}MailTransportQchTargets.cmake
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
    )
    set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KPim${KF_MAJOR_VERSION}MailTransportQchTargets.cmake\")")
endif()

ki18n_install(po)
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

