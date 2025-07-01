#ifndef CARDIMG_H
#define CARDIMG_H


#include <QPixmap>
#include <QLabel>
#include <QString>
#include <QMap>
#include <QPainter>
#include <QFont>
#include <QColor>
#include "card.h"

#include <QPixmap>
#include <QLabel>
#include <QString>
#include <QMap>
#include <QDir>
#include <QDebug>
#include "card.h"

class CardImageManager {
private:
    QMap<QString, QPixmap> cardCache;
    QPixmap backCard;
    QString imagePath;
    int cardWidth;
    int cardHeight;
    
public:
    CardImageManager(const QString& imageDirectory = "/Users/estebanm/Desktop/Blackjack/CardImg", 
                     int width = 100, int height = 140);
    
    // Get card image from file
    QPixmap getCardImage(const Card& card);
    QPixmap getBackCardImage();
    
    // Create card labels
    QLabel* createCardLabel(const Card& card, QWidget* parent = nullptr);
    QLabel* createBackCardLabel(QWidget* parent = nullptr);
    
    // Utility methods
    void setCardSize(int width, int height);
    void setImagePath(const QString& path);
    bool loadCardImage(const QString& filename);

    int getCardWidth() const { return cardWidth; }
    int getCardHeight() const { return cardHeight; }
    
private:
    QString getCardFilename(const Card& card);
    QString getValueString(int value);
    QString getSuitString(Suit suit);
    QPixmap loadAndScaleImage(const QString& filename);
};




#endif