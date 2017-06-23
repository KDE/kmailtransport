
include(ECMMarkAsTest)

find_package(Qt5Test CONFIG REQUIRED)

macro(add_akonadi_isolated_test _source _path)
  get_filename_component(_targetName ${_source} NAME_WE)
  set(_srcList ${_source} )

  add_executable(${_targetName} ${_srcList})
  target_link_libraries(${_targetName}
    Qt5::Test
    KF5::AkonadiCore
    KF5::AkonadiMime
    KF5::MailTransportAkonadi
    KF5::Mime
    KF5::I18n
    KF5::ConfigGui
    Qt5::Widgets
  )

  # based on kde4_add_unit_test
  if (WIN32)
    set(_executable $<TARGET_FILE_DIR:${_targetName}>/${_targetName}.bat)
  else()
    set(_executable ${EXECUTABLE_OUTPUT_PATH}/${_targetName})
  endif()
  if (UNIX)
    set(_executable ${_executable}.shell)
  endif()

  find_program(_testrunner akonaditest)

  if (KDEPIMLIBS_RUN_ISOLATED_TESTS)
    add_test( mailtransport-${_targetName} ${_testrunner} -c ${CMAKE_CURRENT_SOURCE_DIR}/${_path}/config.xml ${_executable} )
  endif()
endmacro(add_akonadi_isolated_test)



# Akonadi testrunner-based tests:

add_akonadi_isolated_test( attributetest.cpp unittestenv )
add_akonadi_isolated_test( messagequeuejobtest.cpp unittestenv )
MESSAGE(STATUS "REACTIVATE IT")
if (KDEPIMLIBS_RUN_KDEPIMRUNTIME_ISOLATED_TESTS)
   add_akonadi_isolated_test( filteractiontest.cpp unittestenv_akonadi )
endif()
