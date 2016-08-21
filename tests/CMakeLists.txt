
include(ECMMarkAsTest)

find_package(KF5TextWidgets ${KF5_VERSION} CONFIG REQUIRED)
find_package(Qt5Test CONFIG REQUIRED)

set(tm_srcs transportmgr.cpp)
add_executable(transportmgr ${tm_srcs})
ecm_mark_as_test(transportmgr)
target_link_libraries(transportmgr KF5MailTransport Qt5::Widgets KF5::I18n KF5::ConfigGui KF5::Completion KF5::TextWidgets)

add_executable(servertest servertest.cpp)
ecm_mark_as_test(servertest)
target_link_libraries(servertest KF5MailTransport KF5::I18n KF5::ConfigGui Qt5::Widgets)

set(queuer_srcs queuer.cpp)
add_executable(queuer ${queuer_srcs})
ecm_mark_as_test(queuer)
target_link_libraries(queuer KF5MailTransport Qt5::Widgets KF5::I18n KF5::ConfigGui KF5::Completion KF5::TextWidgets)

set( sendqueued_srcs sendqueued.cpp )
add_executable( sendqueued ${sendqueued_srcs} )
ecm_mark_as_test(sendqueued)
target_link_libraries( sendqueued KF5MailTransport  KF5::AkonadiMime Qt5::Widgets)

set( clearerror_srcs clearerror.cpp )
add_executable( clearerror ${clearerror_srcs} )
ecm_mark_as_test(clearerror)
target_link_libraries( clearerror KF5MailTransport  KF5::AkonadiMime Qt5::Widgets)

set( abort_srcs abort.cpp )
add_executable( abort ${abort_srcs} )
ecm_mark_as_test(abort)
target_link_libraries( abort KF5MailTransport KF5::AkonadiCore Qt5::Widgets)

