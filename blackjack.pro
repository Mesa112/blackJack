QT += core widgets
CONFIG += c++17

TARGET = Blackjack
TEMPLATE = app


SOURCES += basicStrag.cpp \
           blackjackGUI.cpp \
           card.cpp \
           cardImg.cpp \
           counting.cpp \
           Dealer.cpp \
           deck.cpp \
           GameEngine.cpp \
           hand.cpp \
           main_qt.cpp \
           player.cpp \
           SplitHand.cpp \
           Stats.cpp

HEADERS += basicStrag.h \
           blackjackGUI.h \
           card.h \
           cardImg.h \
           counting.h \
           Dealer.h \
           deck.h \
           GameEngine.h \
           hand.h \
           player.h \
           SplitHand.h \
           Stats.h