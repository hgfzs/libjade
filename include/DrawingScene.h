/* DrawingScene.h
 *
 * Copyright (C) 2013-2017 Jason Allen
 *
 * This file is part of the jade application.
 *
 * jade is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * jade is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with jade.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef DRAWINGSCENE_H
#define DRAWINGSCENE_H

#include <QtGui>

class DrawingView;
class DrawingItem;

/*! \brief Surface for managing a large number of two-dimensional DrawingItem objects.
 *
 * DrawingScene serves as a container for DrawingItem objects.  Items can be added to the widget
 * using addItem() or insertItem() and removed using removeItem().
 *
 * DrawingScene provides several ways to search for items within the scene.
 * \li The items() function returns a list of all the top-level items contained within the widget
 * in the order they were added.
 * \li The visibleItems(const DrawingView*, const QPointF&) const function searches recursively to
 * find all items whose shape intersects with the given location.  The items may be top-level items,
 * or may be child items.
 * \li The visibleItems(const DrawingView*, const QRectF&, Qt::ItemSelectionMode) const function
 * searches recursively to find all items whose shape intersects with or is contained in the given
 * rect, depending on the item selection mode.  The items may be top-level items, or may be child
 * items.
 * \li The visibleItems(const DrawingView*, const QPainterPath&, Qt::ItemSelectionMode) const function
 * searches recursively to find all items whose shape intersects with or is contained in the given
 * path, depending on the item selection mode.  The items may be top-level items, or may be child
 * items.
 * \li The visibleItemAt(const DrawingView*, const QPointF&) const function is used to determine which
 * item (if any) was clicked on by the user.
 *
 * The contents of the scene are painted using the render() function.
 */
class DrawingScene : public QObject
{
	Q_OBJECT

	friend class DrawingView;

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
	 * The default scene rectangle is set to QRectF(-5000, -3750, 10000, 7500).
	 *
	 * \sa sceneRect()
	 */
	void setSceneRect(const QRectF& rect);

	/*! \brief Sets the bounding rectangle of the scene.
	 *
	 * This convenience function is equivalent to calling setSceneRect(QRectF(left, top, width, height)).
	 *
	 * \sa sceneRect(), width(), height()
	 */
	void setSceneRect(qreal left, qreal top, qreal width, qreal height);

	/*! \brief Returns the scene's bounding rectangle.
	 *
	 * \sa setSceneRect(), width(), height()
	 */
	virtual QRectF sceneRect() const;


	/*! \brief Sets the scene's background brush.
	 *
	 * The background brush is used to fill the background of the scene by the default
	 * implementation of render().
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

	/*! \brief Returns a list of all top-level items added to the scene.
	 *
	 * \sa addItem(), insertItem(), removeItem()
	 */
	QList<DrawingItem*> items() const;


	/*! \brief Returns a list of all currently visible items in the scene.
	 *
	 * Unlike the items() function, this functions searches recursively and may include items and
	 * their children.
	 *
	 * \sa visibleItems(const QPointF&) const, visibleItems(const QRectF&, Qt::ItemSelectionMode) const
	 */
	virtual QList<DrawingItem*> visibleItems() const;

	/*! \brief Returns a list of all currently visible items in the scene that are at the specified
	 * position.
	 *
	 * This function uses DrawingItem::shape() to determine the exact shape of each item to test
	 * against the specified position.
	 *
	 * This function searches recursively through all top-level items in the scene.  Therefore it
	 * may include items and their children that match the specified pos.
	 *
	 * \sa visibleItems(const QRectF&, Qt::ItemSelectionMode) const, visibleItemAt()
	 */
	virtual QList<DrawingItem*> visibleItems(const DrawingView* view, const QPointF& pos) const;

	/*! \brief Returns a list of all visible items in the scene that are inside the
	 * specified rectangle.
	 *
	 * This function uses the selectMode parameter to affect how it matches items to the rect:
	 * \li Qt::ContainsItemBoundingRect - only items whose bounding rectangle is fully contained
	 * inside the specified rect are included in the list
	 * \li Qt::ContainsItemShape - only items whose shape is fully contained inside the
	 * specified rect are included in the list
	 * \li Qt::IntersectsItemBoundingRect - all items whose bounding rectangle intersects with the
	 * specified rect are included in the list
	 * \li Qt::IntersectsItemShape - all items whose shape intersects with the specified rect are
	 * included in the list
	 *
	 * This function searches recursively through all top-level items in the scene.  Therefore it
	 * may include items and their children that match the specified pos.
	 *
	 * \sa visibleItems(const QPointF&) const
	 */
	virtual QList<DrawingItem*> visibleItems(const DrawingView* view, const QRectF& rect, Qt::ItemSelectionMode selectMode) const;

	/*! \brief Returns a list of all visible items in the scene that are inside the
	 * specified path.
	 *
	 * This function uses the selectMode parameter to affect how it matches items to the rect:
	 * \li Qt::ContainsItemBoundingRect - only items whose bounding rectangle is fully contained
	 * inside the specified path are included in the list
	 * \li Qt::ContainsItemShape - only items whose shape is fully contained inside the
	 * specified path are included in the list
	 * \li Qt::IntersectsItemBoundingRect - all items whose bounding rectangle intersects with the
	 * specified path are included in the list
	 * \li Qt::IntersectsItemShape - all items whose shape intersects with the specified path are
	 * included in the list
	 *
	 * This function searches recursively through all top-level items in the scene.  Therefore it
	 * may include items and their children that match the specified pos.
	 *
	 * \sa visibleItems(const QPointF&) const
	 */
	virtual QList<DrawingItem*> visibleItems(const DrawingView* view, const QPainterPath& path, Qt::ItemSelectionMode selectMode) const;

	/*! \brief Returns the topmost visible item at the specified position, or nullptr if there are
	 * no items at this position.
	 *
	 * To get the topmost item, this function searches backwards through the scene's items.
	 *
	 * This function uses DrawingItem::shape() to determine the exact shape of each item to test
	 * against the specified position.  It returns immediately once it finds the first item that
	 * matches the specified position.
	 *
	 * This function searches recursively through all top-level items in the scene.  Therefore it
	 * will stop at the first item or child item that matches the specified pos.
	 *
	 * \sa visibleItems(const QPointF&) const
	 */
	virtual DrawingItem* visibleItemAt(const DrawingView* view, const QPointF& scenePos) const;


	/*! \brief Paints the scene using the specified painter object.
	 *
	 * The default implementation is to first paint the sceneRect() using the scene's
	 * backgroundBrush().  Then, all visible items are painted by calling DrawingItem::render() on
	 * each visible item in the scene.
	 */
	virtual void render(QPainter* painter);

private:
	void findItems(const QList<DrawingItem*>& items, QList<DrawingItem*>& foundItems) const;
	bool itemMatchesPoint(const DrawingView* view, DrawingItem* item, const QPointF& scenePos) const;
	bool itemMatchesRect(const DrawingView* view, DrawingItem* item, const QRectF& rect, Qt::ItemSelectionMode mode) const;
	bool itemMatchesPath(const DrawingView* view, DrawingItem* item, const QPainterPath& path, Qt::ItemSelectionMode mode) const;
	QPainterPath itemAdjustedShape(const DrawingView* view, DrawingItem* item) const;
};

#endif
