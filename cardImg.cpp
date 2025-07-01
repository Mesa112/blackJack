#include "cardImg.h"

#include <iostream>


CardImageManager::CardImageManager(const QString& imageDirectory, int width, int height) 
    : imagePath(imageDirectory), cardWidth(width), cardHeight(height) {
    
    // Ensure path ends with slash
    if (!imagePath.endsWith("/")) {
        imagePath += "/";
    }
    
    // Try to load a back card image, or create one if not found
    QString backCardFile = imagePath + "back.jpg";
    if (QFile::exists(backCardFile)) {
        backCard = loadAndScaleImage("back.jpg");
    } else {
        // Create a simple back card if no back.jpg exists
        backCard = QPixmap(cardWidth, cardHeight);
        backCard.fill(QColor(0, 0, 139)); // Dark blue
        QPainter painter(&backCard);
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 12, QFont::Bold));
        painter.drawText(backCard.rect(), Qt::AlignCenter, "CARD\nBACK");
    }
    
    qDebug() << "CardImageManager initialized with path:" << imagePath;
}

QPixmap CardImageManager::getCardImage(const Card& card) {
    QString filename = getCardFilename(card);
    
    if (cardCache.contains(filename)) {
        return cardCache[filename];
    }
    
    // Load the card image from file
    QPixmap cardImage = loadAndScaleImage(filename);
    
    if (cardImage.isNull()) {
        qWarning() << "Failed to load card image:" << filename;
        // Create a fallback text card
        cardImage = QPixmap(cardWidth, cardHeight);
        cardImage.fill(Qt::white);
        QPainter painter(&cardImage);
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        painter.drawText(cardImage.rect(), Qt::AlignCenter, 
                        QString::fromStdString(card.toString()));
    }
    
    cardCache[filename] = cardImage;
    return cardImage;
}

QPixmap CardImageManager::getBackCardImage() {
    return backCard;
}

QLabel* CardImageManager::createCardLabel(const Card& card, QWidget* parent) {
    QLabel* label = new QLabel(parent);
    QPixmap cardImage = getCardImage(card);
    
    label->setFixedSize(cardWidth, cardHeight);
    label->setPixmap(cardImage.scaled(cardWidth, cardHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    label->setStyleSheet("border: 2px solid #333; border-radius: 8px; background: white;");
    label->setScaledContents(false);
    label->setAlignment(Qt::AlignCenter);
    
    return label;
}

QLabel* CardImageManager::createBackCardLabel(QWidget* parent) {
    QLabel* label = new QLabel(parent);
    QPixmap backImage = getBackCardImage();
    
    label->setFixedSize(cardWidth, cardHeight);
    label->setPixmap(backImage.scaled(cardWidth, cardHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    label->setStyleSheet("border: 2px solid #333; border-radius: 8px;");
    label->setScaledContents(false);
    label->setAlignment(Qt::AlignCenter);
    
    return label;
}

QString CardImageManager::getCardFilename(const Card& card) {
    // Based on your example "8H.jpg", the format seems to be: [VALUE][SUIT].jpg
    QString value = getValueString(card.getValue());
    QString suit = getSuitString(card.getSuit());
    
    return QString("%1%2.jpg").arg(value).arg(suit);
}

QString CardImageManager::getValueString(int value) {
    switch(value) {
        case 1: return "A";
        case 11: return "J";
        case 12: return "Q";
        case 13: return "K";
        default: return QString::number(value);
    }
}

QString CardImageManager::getSuitString(Suit suit) {
    switch(suit) {
        case Hearts: return "H";
        case Diamonds: return "D";
        case Clubs: return "C";
        case Spades: return "S";
        default: return "H"; // Default fallback
    }
}

QPixmap CardImageManager::loadAndScaleImage(const QString& filename) {
    QString fullPath = imagePath + filename;
    
    qDebug() << "Attempting to load:" << fullPath;
    
    if (!QFile::exists(fullPath)) {
        qWarning() << "Card image file does not exist:" << fullPath;
        return QPixmap();
    }
    
    QPixmap original(fullPath);
    if (original.isNull()) {
        qWarning() << "Failed to load image from:" << fullPath;
        return QPixmap();
    }
    
    // Scale the image
    return original.scaled(cardWidth, cardHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void CardImageManager::setCardSize(int width, int height) {
    cardWidth = width;
    cardHeight = height;
    cardCache.clear(); 
    
    // Reload back card with new size
    QString backCardFile = imagePath + "gray_black.jpg";
    if (QFile::exists(backCardFile)) {
        backCard = loadAndScaleImage("gray_black.jpg");
    }
}

void CardImageManager::setImagePath(const QString& path) {
    imagePath = path;
    if (!imagePath.endsWith("/")) {
        imagePath += "/";
    }
    cardCache.clear(); 
}

bool CardImageManager::loadCardImage(const QString& filename) {
    QString fullPath = imagePath + filename;
    return QFile::exists(fullPath);
}