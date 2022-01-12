
add_subdirectory(kmailtransport)
if (TARGET KF5::AkonadiCore)
    add_subdirectory(kmailtransportakonadi)
endif()
add_subdirectory(kcm)

