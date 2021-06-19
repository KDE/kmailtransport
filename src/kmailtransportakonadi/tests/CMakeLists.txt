
include(ECMMarkAsTest)

find_package(KF5TextWidgets ${KF5_MIN_VERSION} CONFIG REQUIRED)

set(queuer_srcs queuer.cpp)
add_executable(queuer ${queuer_srcs})
ecm_mark_as_test(queuer)
target_link_libraries(queuer KF5MailTransportAkonadi Qt::Widgets KF5::I18n KF5::ConfigGui KF5::TextWidgets)

set( sendqueued_srcs sendqueued.cpp )
add_executable( sendqueued ${sendqueued_srcs} )
ecm_mark_as_test(sendqueued)
target_link_libraries( sendqueued KF5MailTransportAkonadi  KF5::AkonadiMime Qt::Widgets)

set( clearerror_srcs clearerror.cpp )
add_executable( clearerror ${clearerror_srcs} )
ecm_mark_as_test(clearerror)
target_link_libraries( clearerror KF5MailTransportAkonadi  KF5::AkonadiMime Qt::Widgets)

set( abort_srcs abort.cpp )
add_executable( abort ${abort_srcs} )
ecm_mark_as_test(abort)
target_link_libraries( abort KF5MailTransportAkonadi KF5::AkonadiCore Qt::Widgets)

