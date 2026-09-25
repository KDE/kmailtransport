# SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-3-Clause

ecm_add_qml_module(kmailtransport_quick_plugin
    URI org.kde.mailtransport
    GENERATE_PLUGIN_SOURCE
)

target_sources(
    kmailtransport_quick_plugin
    PRIVATE
        types.cpp
        types.h
)

target_link_libraries(
    kmailtransport_quick_plugin
    PRIVATE
        KPim6::MailTransport
        Qt::Qml
)

ecm_finalize_qml_module(kmailtransport_quick_plugin
    DESTINATION ${KDE_INSTALL_QMLDIR}
    BUILD_SHARED_LIBS OFF
)
