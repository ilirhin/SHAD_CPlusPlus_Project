#include <QtGui>
#include <QWidget>

#include "game_objects.h"

#pragma once

class Notifier : public QObject {
    Q_OBJECT
public:
    Notifier() {}

    ~Notifier() {}

    void updateWorld(const World& new_world) {
        // update world
        emit notifyUpdateWorld(new_world);
    }

    void startShowing(const World& new_world) {
        // start working
        emit notifyStartShowing(new_world);
    }

    void finishShowing() {
        // finish working
        emit notifyFinishShowing();
    }

signals:
    void notifyUpdateWorld(const World& new_world);

    void notifyStartShowing(const World& new_world);

    void notifyFinishShowing();
};

class Field : public QWidget {
    Q_OBJECT
public:
    Field(Notifier* notifier, int window_size = 768) : notifier(notifier),
                                                        window_size_(window_size) {
        {
            qRegisterMetaType<World>("World");
            connect(notifier, SIGNAL(notifyUpdateWorld(const World&)), this,
                SLOT(updateWorld(const World&)));
            connect(notifier, SIGNAL(notifyStartShowing(const World&)), this,
                SLOT(startShowing(const World&)));
            connect(notifier, SIGNAL(notifyFinishShowing()), this, SLOT(close()));
        }
        resize(window_size_ , window_size_);
    }

    void drawBall(QPainter &paint, const Ball &ball) {
        paint.setBrush(Qt::red);
        paint.drawEllipse(QPointF(ball.position_.x_, ball.position_.y_),
                                world_.ball_radius,
                                world_.ball_radius);
        QFont font = paint.font();
        font.setBold(true);
        paint.setFont(font);
        paint.setBrush(Qt::blue);
        paint.drawText(QRectF(ball.position_.x_ - 5,
                                ball.position_.y_ - 10.0,
                                100.0, 100.0),
                        QString::number(ball.id_));
        font.setBold(false);
        paint.setFont(font);
        paint.setBrush(Qt::black);
        paint.drawText(QRectF(ball.position_.x_ - world_.ball_radius,
                                ball.position_.y_ + world_.ball_radius,
                                100.0, 100.0),
                        QString::number(ball.score_, 'd', 1));
    }

    void drawCoin(QPainter &paint, const Coin &coin) {
        paint.setBrush(Qt::yellow);
        paint.drawEllipse(QPointF(coin.position_.x_, coin.position_.y_),
                                world_.coin_radius,
                                world_.coin_radius);
        paint.setBrush(Qt::black);
        paint.drawText(QRectF(coin.position_.x_ - world_.coin_radius,
                                coin.position_.y_ + world_.coin_radius,
                                100.0, 100.0),
                        QString::number(coin.value_, 'd', 1));
    }

    void drawWorld(QPainter &paint) {
        paint.setBrush(Qt::gray);
        paint.drawEllipse(QPoint(window_size_ / 2, window_size_ / 2),
                                static_cast<int>(world_.field_radius),
                                static_cast<int>(world_.field_radius));
        { // drawing balls
            for (Ball &ball : world_.balls) {
                drawBall(paint, ball);
            }
        }
        { // drawing coins
            for (Coin &coin : world_.coins) {
                drawCoin(paint, coin);
            }
        }
    }
    
    void paintEvent(QPaintEvent *) {
        QPainter paint(this);
        clear(paint);
        drawWorld(paint);
    }

    void clear(QPainter &paint) {
        paint.eraseRect(0, 0, window_size_, window_size_);
    }

public slots:
    void updateWorld(const World& new_world) {
        world_ = new_world;
        update();
    }

    void startShowing(const World& new_world) {
        world_ = new_world;
        show();
    }

private:
    World world_;
    Notifier* notifier;
    int window_size_;
};
