# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
include(ECMAddTests)

macro(add_mailtransport_test _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} ${_name}.h
        TEST_NAME ${_name}
        LINK_LIBRARIES KPim6MailTransport
        Qt::Test Qt::Widgets
)
endmacro()

add_mailtransport_test(transporttreeviewtest.cpp)
