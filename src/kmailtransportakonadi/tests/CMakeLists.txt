
include(ECMMarkAsTest)

find_package(KF${KF_MAJOR_VERSION}TextWidgets ${KF_MIN_VERSION} CONFIG REQUIRED)

set(queuer_srcs queuer.cpp queuer.h)
add_executable(queuer ${queuer_srcs})
ecm_mark_as_test(queuer)
target_link_libraries(queuer KPim${KF_MAJOR_VERSION}::MailTransportAkonadi KPim${KF_MAJOR_VERSION}::MailTransport Qt::Widgets KF${KF_MAJOR_VERSION}::I18n KF${KF_MAJOR_VERSION}::ConfigGui KF${KF_MAJOR_VERSION}::TextWidgets)

set( sendqueued_srcs sendqueued.cpp sendqueued.h)
add_executable( sendqueued ${sendqueued_srcs} )
ecm_mark_as_test(sendqueued)
target_link_libraries( sendqueued KPim${KF_MAJOR_VERSION}MailTransportAkonadi  KPim${KF_MAJOR_VERSION}::AkonadiMime Qt::Widgets)

set( clearerror_srcs clearerror.cpp clearerror.h)
add_executable( clearerror ${clearerror_srcs} )
ecm_mark_as_test(clearerror)
target_link_libraries( clearerror KPim${KF_MAJOR_VERSION}MailTransportAkonadi  KPim${KF_MAJOR_VERSION}::AkonadiMime Qt::Widgets)

set( abort_srcs abort.cpp abort.h)
add_executable( abort ${abort_srcs} )
ecm_mark_as_test(abort)
target_link_libraries( abort KPim${KF_MAJOR_VERSION}MailTransportAkonadi KF5::AkonadiCore Qt::Widgets)

