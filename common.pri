CONFIG += c++11
CONFIG += warn_on
CONFIG += debug

INCLUDEPATH += $$PWD/include

# Enable console on Windows so that we can actually see debug prints.
win32 {
    CONFIG += console
}

*-clang*|*-g++* {
    message("Enabling additional warnings")
    CONFIG -= warn_on
    QMAKE_CXXFLAGS += -Wall -Wextra -pedantic -Winit-self
    QMAKE_CXXFLAGS += -Wno-strict-aliasing
    QMAKE_CXXFLAGS += -Wno-unused-parameter
    QMAKE_CXXFLAGS += -fno-omit-frame-pointer
}
*-clang* {
    QMAKE_CXXFLAGS += -Wno-unneeded-internal-declaration
}

linux-clang*|linux-g++*|macx-clang*|macx-g++* {
    message("Enabling stack protector")
    QMAKE_CXXFLAGS += -fstack-protector-all
}

address_sanitizer {
    message("Enabling Address Sanitizer")
    QMAKE_CXXFLAGS += -fsanitize=address
    QMAKE_LFLAGS += -fsanitize=address
}

HEADERS +=

SOURCES +=
