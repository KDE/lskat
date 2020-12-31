/*
    This file is part of the KDE games lskat program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef LSKAT_GAMEVIEW_H
#define LSKAT_GAMEVIEW_H

// Qt includes
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QHash>
#include <QList>
#include <QPoint>
#include <QResizeEvent>
#include <QSize>
#include <QElapsedTimer>
#include <QWidget>

// Forward declaration
class ThemeManager;

/**
 * The view object which shows the graphics in a
 * Qt graphics view.
 */
class GameView : public QGraphicsView
{
    Q_OBJECT

public:
    /**
     * Constructor for the canvas view.
     * @param size The canvas fixed size
     * @param advancePeriod The canvas advance period
     * @param scene The graphics scene
     * @param theme The theme manager
     * @param parent The parent window
     */
    GameView(const QSize &size, int advancePeriod, QGraphicsScene *scene, ThemeManager *theme, QWidget *parent = nullptr);

    /**
     * Destructor.
     */
    ~GameView();

protected:
    /**
     * React to mouse clicks.
     * @param event The mouse event
     */
    void mouseReleaseEvent(QMouseEvent *event) override;

public slots:
    /**
     * The update and advance for the canvas.
     * This is called by a timer at regular intervals.
     */
    void updateAndAdvance();

    /**
     * Rescale the theme (update theme SVG graphics).
     */
    void rescaleTheme();

signals:
    /**
     * Emit the signal on mouse clicks.
     * @param point The mouse coordinate.
     */
    void signalLeftMousePress(QPoint point);

protected:
    /**
     * Will be called by the Qt View when its contents are resized.
     * We adapt the canvas then.
     * @param e The resize event
     */
    void resizeEvent(QResizeEvent *e) override;

    /**
     * Paint function for the widget, temporary fix while we wait for QGV 4.3
     * @param event The paint event
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * Overwritten Qt function.
     */
    void drawItems(QPainter *painter, int numItems, QGraphicsItem *items[], const QStyleOptionGraphicsItem options[]) override;

private:
    // Theme manager
    ThemeManager *mTheme;
    // Theme queue
    QList<int> mThemeQueue;
    // Theme offset queue
    QList<QPoint> mThemeOffset;
    // Active input offset
    QPoint mInputOffset;
    // Theme queue delay time [ms]
    QElapsedTimer mTimeStart;
    // Debug frame rate sprite
    QGraphicsTextItem *mFrameSprite;
    // Time between updates
    int mDisplayUpdateTime;
    // Average update times
    QList<int> mDrawTimes;
};

#endif // LSKAT_GAMEVIEW_H
