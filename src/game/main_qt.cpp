#include <QApplication>
#include "blackjackGUI.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    BlackjackGUI window;
    window.show();
    
    return app.exec();
}

