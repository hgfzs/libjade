/* DrawingItem.cpp
 *
 * Copyright (C) 2013-2016 Jason Allen
 *
 * This file is part of the jade library.
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

#ifndef DRAWINGITEM_H
#define DRAWINGITEM_H

#include <DrawingTypes.h>

class DrawingItemStyle;

/*! \brief Base class for all graphical items in a DrawingWidget.
 *
 * DrawingItem provides a light-weight foundation for writing custom items. This includes
 * defining the item's geometry, painting implementation, and item interaction through event
 * handlers.
 *
 * For convenience, a set of standard graphics items if provided for the most common shapes:
 *
 * \li DrawingArcItem provides an arc item
 * \li DrawingCurveItem provides a Bezier curve item
 * \li DrawingEllipseItem provides an ellipse item
 * \li DrawingLineItem provides a line item
 * \li DrawingPathItem provides an arbitrary path item
 * \li DrawingPolygonItem provides a polygon item
 * \li DrawingRectItem provides a rectangular item
 * \li DrawingTextItem provides a text item
 *
 * All of an item's geometric information is based on its local coordinate system. The item's
 * position, pos(), is the only function that does not operate in local coordinates, as it returns
 * a position in scene coordinates.
 *
 * An item can be set as visible (i.e., drawn and accepting events) by calling setVisible().
 * By default, items are set as visible and enabled.
 *
 * As item can be selected by calling setSelected(). Normally, selection is toggled by the scene
 * as a result of user interaction.
 *
 * \section itemGeometry Item Geometry
 *
 * A complete description of the geometry of an item is given by the boundingRect(), shape(),
 * centerPos(), and isValid() functions.
 *
 * The shape() function returns an accurate outline of an item's shape as a QPainterPath.  The
 * boundingRect() function returns an estimate of the area painted by an item as a QRectF.  The
 * implementation of boundingRect() is meant to be as lightweight as possible.  It is not
 * recommended to return the boundingRect of the QPainterPath returned by shape() for this
 * implementation for most items.
 *
 * The centerPos() is used to determine the center of all of the currently selected items in the
 * scene.  The default implementation is to return the center of the item's boundingRect().
 *
 * The isValid() function should return false if the item is not really valid (for example, a line
 * where the start and end points are the same).  DrawingWidget will prevent invalid items from
 * being added to the scene.  The default implementation is to return true if the item's
 * boundingRect() is a valid rect.
 *
 * \section itemPainting Painting
 *
 * The paint() function is called by DrawingWidget to paint the item's contents. The item has no
 * background or default fill of its own; whatever is behind the item will shine through all
 * areas that are not explicitly painted in this function.
 *
 * All items are drawn in the order of when they were added to scene.  This order also dictates
 * which items will receive mouse input first when clicking within the scene.
 *
 * \section itemEvents Events
 *
 * DrawingItem receives events from DrawingWidget through several event handlers:
 *
 * \li createEvent() is called when the item is first created by DrawingWidget in Place mode
 * \li mousePressEvent(), mouseMoveEvent(), mouseReleaseEvent(), and mouseDoubleClickEvent() handle
 * mouse events
 * \li keyPressEvent() and keyReleaseEvent() handle keyboard events
 *
 * Custom items will likely want to handle these events differently when the DrawingWidget is
 * in Default mode versus when it is in Place mode.  The mode can be checked by calling
 * drawing()->mode().
 *
 * \section itemSlots Slots
 *
 * DrawingWidget will call the following "slots" when manipulating the item within the scene:
 *
 * \li moveItem() is called when moving the item around the scene
 * \li resizeItem() is called when resizing the item within the scene
 * \li rotateItem() and rotateBackItem() are called when rotating the item within the scene
 * \li flipItem() is called when flipping the item horizontally within the scene
 * \li insertItemPoint() and removeItemPoint() are called when adding/removing item points
 *
 * These slots are only called if their corresponding flag is set on the item.  For example, the
 * resizeItem() slot is only called if the CanResize flag is set. The default flags() set on a new
 * item are CanMove | CanResize | CanRotate | CanFlip.  Default implementations are provided for
 * moveItem(), resizeItem(), rotateItem(), rotateItemBack(), and flipItem().
 *
 * For items that support adding/removing item points, implementations should set the
 * CanInsertRemovePoints flag and provide an implementation for insertItemPoint() and removeItemPoint().
 *
 * \section itemCustom Custom Items
 *
 * To write a custom graphics item, first create a subclass of DrawingItem, and then start by
 * implementing its three pure virtual public functions: copy(), boundingRect() and paint().
 * Complex items may need to override shape(), centerPos(), and isValid() for a more complete
 * description of the item's geometry.
 *
 * To alter the behavior of the item within the scene, consider overriding any of the item's slots
 * (such as resizeItem()) or events (such as mouseReleaseEvent()) as well.
 */
class DrawingItem
{
	friend class DrawingWidget;
	friend class DrawingItemGroup;

public:
	//! \brief Enum used to affect the behavior of the DrawingItem within the scene.
	enum Flag
	{
		CanMove = 0x01,					//!< Indicates that the item can be moved around the scene.
										//!< See also moveItem().
		CanResize = 0x02,				//!< Indicates that the item can be resized within the
										//!< scene.  See also resizeItem().
		CanRotate = 0x04,				//!< Indicates that the item can be rotated within the
										//!< scene.  See also rotateItem() and rotateBackItem().
		CanFlip = 0x08,					//!< Indicates that the item can be flipped horizontally
										//!< within the scene.  See also flipItem().
		CanInsertRemovePoints = 0x10	//!< Indicates that item points can be added or removed
										//!< from the item.  See also insertItemPoint() and
										//!< removeItemPoint().
	};
	Q_DECLARE_FLAGS(Flags, Flag)

private:
	DrawingWidget* mDrawing;

	QPointF mPos;
	Flags mFlags;

	qreal mRotation;
	bool mFlipped;

	bool mSelected;
	bool mVisible;

	QList<DrawingItemPoint*> mPoints;
	DrawingItemPoint* mSelectedPoint;

	DrawingItemStyle* mStyle;

	// Internal
	QTransform mTransform;
	QTransform mTransformInverse;

public:
	DrawingItem();
	DrawingItem(const DrawingItem& item);
	virtual ~DrawingItem();

	virtual DrawingItem* copy() const = 0;

	DrawingWidget* drawing() const;


	void setPos(const QPointF& pos);
	void setPos(qreal x, qreal y);
	void setX(qreal x);
	void setY(qreal y);
	QPointF pos() const;
	qreal x() const;
	qreal y() const;

	void setFlags(Flags flags);
	Flags flags() const;

	void setRotation(qreal angle);
	void setFlipped(bool flipped);
	qreal rotation() const;
	bool isFlipped() const;
	QTransform transform() const;

	void setSelected(bool select);
	void setVisible(bool visible);
	bool isSelected() const;
	bool isVisible() const;


	void addPoint(DrawingItemPoint* itemPoint);
	void insertPoint(int index, DrawingItemPoint* itemPoint);
	void removePoint(DrawingItemPoint* itemPoint);
	void clearPoints();
	QList<DrawingItemPoint*> points() const;
	DrawingItemPoint* selectedPoint() const;

	DrawingItemPoint* pointAt(const QPointF& itemPos) const;
	DrawingItemPoint* pointNearest(const QPointF& itemPos) const;


	void setStyle(DrawingItemStyle* style);
	DrawingItemStyle* style() const;


	QPointF mapFromScene(const QPointF& point) const;
	QRectF mapFromScene(const QRectF& rect) const;
	QPolygonF mapFromScene(const QPolygonF& polygon) const;
	QPainterPath mapFromScene(const QPainterPath& path) const;
	QPointF mapToScene(const QPointF& point) const;
	QRectF mapToScene(const QRectF& rect) const;
	QPolygonF mapToScene(const QPolygonF& polygon) const;
	QPainterPath mapToScene(const QPainterPath& path) const;


	virtual QRectF boundingRect() const = 0;
	virtual QPainterPath shape() const;
	virtual QPointF centerPos() const;
	virtual bool isValid() const;

	virtual void paint(QPainter* painter) = 0;


	virtual void moveItem(const QPointF& scenePos);
	virtual void resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos);

	virtual void rotateItem(const QPointF& scenePos);
	virtual void rotateBackItem(const QPointF& scenePos);
	virtual void flipItem(const QPointF& scenePos);

	virtual void insertItemPoint(const QPointF& scenePos);
	virtual void removeItemPoint(const QPointF& scenePos);

protected:
	virtual void createEvent();

	virtual void mousePressEvent(DrawingMouseEvent* event);
	virtual void mouseMoveEvent(DrawingMouseEvent* event);
	virtual void mouseReleaseEvent(DrawingMouseEvent* event);
	virtual void mouseDoubleClickEvent(DrawingMouseEvent* event);

	virtual void keyPressEvent(QKeyEvent* event);
	virtual void keyReleaseEvent(QKeyEvent* event);

private:
	void recalculateTransform();

public:
	static QList<DrawingItem*> copyItems(const QList<DrawingItem*>& items);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DrawingItem::Flags)

#endif
