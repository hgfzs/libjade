/* DrawingScene.h
 *
 * Copyright (C) 2013-2020 Jason Allen
 *
 * This file is part of the libjade library.
 *
 * libjade is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libjade is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libjade.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef DRAWINGSCENE_H
#define DRAWINGSCENE_H

#include <QBrush>
#include <QList>
#include <QObject>
#include <QPainterPath>
#include <QPointF>
#include <QRectF>

class DrawingItem;

/*! \brief Container for DrawingItem objects.
 *
 * DrawingScene serves as a container for DrawingItem objects.  Items can be added to the scene
 * using addItem() or insertItem() and removed using removeItem().
 *
 * A scene has bounds that are defined by its sceneRect().  It also has a background brush property
 * that can be accessed via backgroundBrush().
 */
class DrawingScene : public QObject
{
	Q_OBJECT

private:
	QRectF mSceneRect;
	QBrush mBackgroundBrush;

	QList<DrawingItem*> mItems;

public:
	/*! \brief Create a new DrawingScene with default settings.
	 *
	 * The new scene does not contain any items.
	 */
	DrawingScene();

	//! \brief Delete an existing DrawingScene object.
	virtual ~DrawingScene();


	/*! \brief Sets the bounding rectangle of the scene.
	 *
	 * The scene rectangle defines the extent of the scene.
	 *
	 * The default scene rectangle is set to QRectF(0, 0, 11000, 8500).
	 *
	 * \sa sceneRect()
	 */
	void setSceneRect(const QRectF& rect);

	/*! \brief Sets the bounding rectangle of the scene.
	 *
	 * This convenience function is equivalent to calling
	 * \link setSceneRect(const QRectF& rect) setSceneRect(QRectF(left, top, width, height)) \endlink .
	 *
	 * \sa sceneRect()
	 */
	void setSceneRect(qreal left, qreal top, qreal width, qreal height);

	/*! \brief Returns the scene's bounding rectangle.
	 *
	 * \sa setSceneRect(const QRectF&), setSceneRect(qreal, qreal, qreal, qreal)
	 */
	QRectF sceneRect() const;


	/*! \brief Sets the scene's background brush.
	 *
	 * The default background brush is set to white.
	 *
	 * \sa backgroundBrush()
	 */
	void setBackgroundBrush(const QBrush& brush);

	/*! \brief Returns the scene's background brush.
	 *
	 * \sa setBackgroundBrush()
	 */
	QBrush backgroundBrush() const;


	/*! \brief Adds an existing item to the scene.
	 *
	 * This convenience function is equivalent to calling #insertItem(items().size(), item).
	 *
	 * \sa removeItem()
	 */
	void addItem(DrawingItem* item);

	/*! \brief Inserts an existing item to the scene at the specified index.
	 *
	 * If a valid item is passed to this function, DrawingScene will insert it into its list of
	 * items() at the specified index.  DrawingScene takes ownership of the item and will
	 * delete it as necessary.
	 *
	 * It is safe to pass a nullptr to this function; if a nullptr is received, this function
	 * does nothing.  This function also does nothing if the item is already one of the scene's
	 * items().
	 *
	 * \sa addItem(), removeItem()
	 */
	void insertItem(int index, DrawingItem* item);

	/*! \brief Removes an existing item from the scene.
	 *
	 * If a valid item is passed to this function, DrawingScene will remove it from its list of
	 * items().  DrawingScene relinquishes ownership of the item and does not delete the
	 * item from memory.
	 *
	 * It is safe to pass a nullptr to this function; if a nullptr is received, this function
	 * does nothing.  This function also does nothing if the item is not one of the scene's
	 * items().
	 *
	 * \sa addPoint(), insertPoint(), clearPoints()
	 */
	void removeItem(DrawingItem* item);

	/*! \brief Removes and deletes all items from the scene.
	 *
	 * This function removes and deletes all of the scene's items() from memory.
	 *
	 * \sa removeItem()
	 */
	void clearItems();

	/*! \brief Returns a list of all items added to the scene.
	 *
	 * \sa addItem(), insertItem(), removeItem()
	 */
	QList<DrawingItem*> items() const;
};

#endif
