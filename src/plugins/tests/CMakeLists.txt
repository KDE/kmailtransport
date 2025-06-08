include(ECMMarkAsTest)
add_executable(
    transportmanagertest
    transportmanagertest.cpp
    transportmanagertest.h
)
ecm_mark_as_test(transportmanagertest)
target_link_libraries(transportmanagertest KPim6MailTransport)
