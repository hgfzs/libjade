/* DrawingItem.h
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
 * DrawingItem provides a lightweight foundation for writing custom items. This includes
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
 * DrawingItem has full support for item styles via DrawingItemStyle.  Item styles affect how an
 * item is rendered within a DrawingWidget.  See DrawingItemStyle for more information.
 *
 * \section itemEvents Events
 *
 * DrawingItem receives events from DrawingWidget through several event handlers:
 *
 * \li mousePressEvent(), mouseMoveEvent(), mouseReleaseEvent(), and mouseDoubleClickEvent() handle
 * mouse events
 * \li keyPressEvent() and keyReleaseEvent() handle keyboard events
 * \li newItemCopyEvent() is called when the item is first created by DrawingWidget in Place mode
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
 * resizeItem() slot is only called if the #CanResize flag is set. The default flags() set on a new
 * item are #CanMove | #CanResize | #CanRotate | #CanFlip | #CanSelect.  Default implementations 
 * are provided for moveItem(), resizeItem(), rotateItem(), rotateItemBack(), and flipItem().
 *
 * For items that support adding item points, implementations should set the #CanInsertPoints flag
 * and provide an implementation for insertItemPoint().  For items that support adding item points,
 * implementations should set the #CanRemovePoints flag and provide an implementation for
 * removeItemPoint().
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
		CanInsertPoints = 0x10,			//!< Indicates that item points can be added
										//!< to the item.  See also insertItemPoint().
		CanRemovePoints = 0x20,			//!< Indicates that item points can be removed
										//!< from the item.  See also removeItemPoint().
		CanSelect = 0x40				//!< Indicates that the item can be selected by the user
										//!< within the scene.
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

	DrawingItemStyle* mStyle;

	// Internal
	QTransform mTransform;
	QTransform mTransformInverse;

public:
	/*! \brief Create a new DrawingItem with default settings.
	 *
	 * The new item is not associated with a DrawingWidget.
	 */
	DrawingItem();

	/*! \brief Create a new DrawingItem as a copy of an existing item.
	 *
	 * Copies the position, flags, and transform from the existing item.  Also creates copies
	 * of the existing item's points for the new item.  Also creates a new item style based upon
	 * the existing item's style.
	 *
	 * The new item is not associated with a DrawingWidget.
	 */
	DrawingItem(const DrawingItem& item);

	/*! \brief Delete an existing DrawingItem object.
	 *
	 * Removes the item from its parent DrawingWidget before deletion.
	 */
	virtual ~DrawingItem();


	/*! \brief Creates a copy of the DrawingItem and return it.
	 *
	 * It is recommended that derived classes implement a copy constructor to ensure that copies are
	 * handled as expected.  Then this function can simply create a new item by calling the
	 * derived class copy constructor.
	 */
	virtual DrawingItem* copy() const = 0;


	/*! \brief Returns the current drawing that this item is a member of, or nullptr if the item
	 * is not associated with a drawing.
	 *
	 * To add an item to a drawing, call DrawingWidget::addItem() or DrawingWidget::insertItem(). To
	 * remove an item from a drawing, call DrawingWidget::removeItem().
	 */
	DrawingWidget* drawing() const;


	/*! \brief Sets the position of the item.
	 *
	 * The position of the item describes its origin (local coordinate (0,0)) in drawing
	 * coordinates.
	 *
	 * To move an item around the drawing as if the user clicked on it, call
	 * DrawingWidget::moveSelection().
	 *
	 * \sa pos()
	 */
	void setPos(const QPointF& pos);

	/*! \brief Sets the position of the item.
	 *
	 * This convenience function is equivalent to calling setPos(QPointF(x,y)).
	 *
	 * \sa pos(), x(), y()
	 */
	void setPos(qreal x, qreal y);

	/*! \brief Sets the x-coordinate of the item's position.
	 *
	 * This convenience function is equivalent to calling setPos(QPointF(x,y())).
	 *
	 * \sa setPos(), x()
	 */
	void setX(qreal x);

	/*! \brief Sets the y-coordinate of the item's position.
	 *
	 * This convenience function is equivalent to calling setPos(QPointF(x(),y)).
	 *
	 * \sa setPos(), y()
	 */
	void setY(qreal y);

	/*! \brief Returns the position of the item in the drawing.
	 *
	 * \sa setPos(), x(), y()
	 */
	QPointF pos() const;

	/*! \brief Returns the x-coordinate of the item's position.
	 *
	 * \sa setX(), y()
	 */
	qreal x() const;

	/*! \brief Returns the y-coordinate of the item's position.
	 *
	 * \sa setX(), y()
	 */
	qreal y() const;


	/*! \brief Sets the type of item through a combination of flags.
	 *
	 * The type of item point is represented by a combination of flags.  Any combination of flags
	 * is valid for DrawingItem objects.  By default, the (#CanMove | #CanResize | #CanRotate |
	 * #CanFlip | #CanSelect) flags are set.
	 *
	 * Items that set the #CanInsertPoints flag should also provide an implementation for
	 * insertItemPoint().  Items that set the #CanRemovePoints flag should also provide an
	 * implementation for and removeItemPoint().
	 *
	 * \sa flags()
	 */
	void setFlags(Flags flags);

	/*! \brief Returns the type of the item as represented as a combination of flags.
	 *
	 * \sa setFlags()
	 */
	Flags flags() const;


	/*! \brief Sets the rotation angle of the item.
	 *
	 * The rotation is counter-clockwise around the z-axis of the item through its origin point
	 * (0,0).  By default, the rotation angle is set to 0 (the item is not rotated).
	 *
	 * Any value outside the range (0.0, 360.0) will be adjusted to fit within this range.
	 *
	 * \sa rotation(), setFlipped()
	 */
	void setRotation(qreal angle);

	/*! \brief Sets whether the item is flipped horizontally or not.
	 *
	 * By default, the flipped status is set to false (this item is not flipped).
	 *
	 * \sa isFlipped(), setRotation()
	 */
	void setFlipped(bool flipped);

	/*! \brief Returns the rotation angle if the item.
	 *
	 * \sa setRotation()
	 */
	qreal rotation() const;

	/*! \brief Returns whether the item is flipped horizontally or not.
	 *
	 * \sa setFlipped()
	 */
	bool isFlipped() const;

	/*! \brief Returns the complete item transformation matrix.
	 *
	 * The transform is calculated by first applying the rotation, then flipping the item if
	 * necessary.  The transform does not include any translation due to the item's position within
	 * the drawing.
	 *
	 * \sa rotation(), isFlipped()
	 */
	QTransform transform() const;


	/*! \brief Sets whether the item is currently selected within the drawing or not.
	 *
	 * Items that are selected can be manipulated within the drawing (i.e. moved, resized, rotated,
	 * flipped, etc).  By default, items are not selected (isSelected() returns false).
	 *
	 * \sa isSelected()
	 */
	void setSelected(bool select);

	/*! \brief Sets whether the item is currently visible within the drawing or not.
	 *
	 * Items that are not visible are not drawn and do not receive events.  By default, items are
	 * visible (isVisible() returns true).
	 *
	 * \sa isVisible()
	 */
	void setVisible(bool visible);

	/*! \brief Returns whether the item is currently selected within the drawing or not.
	 *
	 * \sa setSelected()
	 */
	bool isSelected() const;

	/*! \brief Returns whether the item is currently visible within the drawing or not.
	 *
	 * \sa setVisible()
	 */
	bool isVisible() const;


	/*! \brief Adds an existing item point to the item.
	 *
	 * This convenience function is equivalent to calling #insertPoint(itemPoint, points().size()).
	 *
	 * \sa removePoint()
	 */
	void addPoint(DrawingItemPoint* itemPoint);

	/*! \brief Inserts an existing item point to the item at the specified index.
	 *
	 * If a valid item point is passed to this function, DrawingItem will insert it into its list of
	 * points() at the specified index.  DrawingItem takes ownership of the itemPoint and will
	 * delete it as necessary.
	 *
	 * It is safe to pass a nullptr to this function; if a nullptr is received, this function
	 * does nothing.  This function also does nothing if the itemPoint is already one of the item's
	 * points().
	 *
	 * \sa addPoint(), removePoint()
	 */
	void insertPoint(int index, DrawingItemPoint* itemPoint);

	/*! \brief Removes an existing item point from the item.
	 *
	 * If a valid item point is passed to this function, DrawingItem will remove it from its list of
	 * points().  DrawingItem relinquishes ownership of the itemPoint and does not delete the
	 * point from memory.
	 *
	 * It is safe to pass a nullptr to this function; if a nullptr is received, this function
	 * does nothing.  This function also does nothing if the itemPoint is not one of the item's
	 * points().
	 *
	 * \sa addPoint(), insertPoint(), clearPoints()
	 */
	void removePoint(DrawingItemPoint* itemPoint);

	/*! \brief Removes and deletes all item points from the item.
	 *
	 * This function removes and deletes all of the item's points() from memory.
	 *
	 * \sa removePoint()
	 */
	void clearPoints();

	/*! \brief Returns a list of all item points added to the item.
	 *
	 * \sa addPoint(), insertPoint(), removePoint()
	 */
	QList<DrawingItemPoint*> points() const;

	/*! \brief Returns the item point located at the specified position, or nullptr if no match is
	 * found.
	 *
	 * For each point, this function calculates the bounding rect of the point using
	 * DrawingWidget::pointRect(), then maps it to local item coordinates.  If the specified itemPos
	 * is contained within the point's boundingRect, the point is returned immediately.  If no
	 * match is found after searching through all of the item's points, nullptr is returned.
	 * The itemPos is given in the local item coordinates.
	 *
	 * This function is used by DrawingWidget to determine if any of the item's points were
	 * clicked on by the user.
	 *
	 * \sa pointNearest()
	 */
	DrawingItemPoint* pointAt(const QPointF& itemPos) const;

	/*! \brief Returns the item point nearest to the specified position.
	 *
	 * This function calculates the distance between itemPos and each of the item's points.  It
	 * selects the point that is closest to itemPos and returns it.  The itemPos is given in the
	 * local item coordinates.
	 *
	 * This function may return nullptr if the item does not have any points().
	 *
	 * \sa pointAt()
	 */
	DrawingItemPoint* pointNearest(const QPointF& itemPos) const;


	/*! \brief Sets the item's style.
	 *
	 * The item's style contains settings that affect how the item is rendered within a
	 * DrawingWidget.  By default, the item is a assigned an empty style with no settings applied.
	 *
	 * If a valid item style is passed to this function, DrawingItem will delete the existing item
	 * style from memory and replace it with the specified style.  DrawingItem takes ownership of
	 * the new style and will delete it as necessary.
	 *
	 * It is safe to pass a nullptr to this function; if a nullptr is received, this function
	 * does nothing.
	 *
	 * \sa style()
	 */
	void setStyle(DrawingItemStyle* style);

	/*! \brief Returns the item's style.
	 *
	 * \sa setStyle()
	 */
	DrawingItemStyle* style() const;


	/*! \brief Maps the point from the coordinate system of the DrawingWidget to the item's
	 * coordinate system.
	 *
	 * \sa mapToScene(const QPointF&) const
	 */
	QPointF mapFromScene(const QPointF& point) const;

	/*! \brief Maps the rect from the coordinate system of the DrawingWidget to the item's
	 * coordinate system.
	 *
	 * \sa mapToScene(const QRectF&) const
	 */
	QRectF mapFromScene(const QRectF& rect) const;

	/*! \brief Maps the polygon from the coordinate system of the DrawingWidget to the item's
	 * coordinate system.
	 *
	 * \sa mapToScene(const QPolygonF&) const
	 */
	QPolygonF mapFromScene(const QPolygonF& polygon) const;

	/*! \brief Maps the path from the coordinate system of the DrawingWidget to the item's
	 * coordinate system.
	 *
	 * \sa mapToScene(const QPainterPath&) const
	 */
	QPainterPath mapFromScene(const QPainterPath& path) const;

	/*! \brief Maps the point from the item's coordinate system to the coordinate system of the
	 * DrawingWidget.
	 *
	 * \sa mapFromScene(const QPointF&) const
	 */
	QPointF mapToScene(const QPointF& point) const;

	/*! \brief Maps the rect from the item's coordinate system to the coordinate system of the
	 * DrawingWidget.
	 *
	 * \sa mapFromScene(const QRectF&) const
	 */
	QRectF mapToScene(const QRectF& rect) const;

	/*! \brief Maps the polygon from the item's coordinate system to the coordinate system of the
	 * DrawingWidget.
	 *
	 * \sa mapFromScene(const QPolygonF&) const
	 */
	QPolygonF mapToScene(const QPolygonF& polygon) const;

	/*! \brief Maps the path from the item's coordinate system to the coordinate system of the
	 * DrawingWidget.
	 *
	 * \sa mapFromScene(const QPainterPath&) const
	 */
	QPainterPath mapToScene(const QPainterPath& path) const;


	/*! \brief Returns an estimate of the area painted by an item.
	 *
	 * The function returns a rectangle in local item coordinates.
	 *
	 * The implementation of boundingRect() is meant to be as lightweight as possible.  It is not
	 * recommended to return the boundingRect of the QPainterPath returned by shape() for most
	 * items.  A better approach would be a quick way to estimate this rect without creating
	 * a QPainterPath.
	 *
	 * \sa shape(), centerPos(), isValid()
	 */
	virtual QRectF boundingRect() const = 0;

	/*! \brief Returns an accurate outline of an item's shape.
	 *
	 * The function returns a path in local item coordinates.
	 *
	 * The default implementation calls boundingRect() to return a simple rectangular shape.  Most
	 * derived classes will want to reimplement this behavior to match the actual shape of the item
	 * being represented.
	 *
	 * The outline of a shape can vary depending on the width and style of the pen used when
	 * drawing. To include this outline in the item's shape, create a shape from the stroke using
	 * QPainterPathStroker.
	 *
	 * \sa boundingRect(), centerPos(), isValid()
	 */
	virtual QPainterPath shape() const;

	/*! \brief Returns a position representing the center of the item.
	 *
	 * The function returns a location in local item coordinates.
	 *
	 * This function is used to determine the center of all of the currently selected items in the
	 * scene.
	 *
	 * The default implementation is to return the center of the item's boundingRect().
	 *
	 * \sa boundingRect(), shape(), isValid()
	 */
	virtual QPointF centerPos() const;

	/*! \brief Return false if the item is degenerate, true otherwise.
	 *
	 * This function should return false if the item is not really valid (for example, a line
	 * where the start and end points are the same).  DrawingWidget will prevent invalid items from
	 * being added to the scene
	 *
	 * The default implementation is to return true if the item's boundingRect() is a valid rect.
	 *
	 * \sa boundingRect(), shape(), centerPos()
	 */
	virtual bool isValid() const;


	/*! \brief Paints the contents of the item into the scene.
	 *
	 * This function is typically called by DrawingWidget when rendering the scene.  DrawingWidget
	 * handles all of the necessary transformations, so this function should paint the item in
	 * local item coordinates.
	 */
	virtual void paint(QPainter* painter) = 0;


	/*! \brief Moves the item within the scene.
	 *
	 * This function is called when the item's drawing() wants to move it within the scene. 
	 * This will only be called for items that have the #CanMove flag set as one of
	 * their flags().
	 *
	 * The default implementation simply calls setPos() to update the item's position.
	 *
	 * Derived class implementations can add additional behavior.  These implementations should
	 * call the parent implementation first before adding custom logic.
	 *
	 * \sa resizeItem(), rotateItem(), rotateBackItem(), flipItem()
	 */
	virtual void moveItem(const QPointF& scenePos);
	
	/*! \brief Resizes the item within the scene.
	 *
	 * This function is called when the item's drawing() wants to resize it within the scene. 
	 * This will only be called for items that have the #CanResize flag set as one of
	 * their flags().
	 *
	 * The default implementation calls itemPoint->setPos() to update the point's position.
	 *
	 * If the #MapFirstItemPointToOrigin is set as one of the item's flags(), than after the resize 
	 * this function will adjust the position of the item's points so that the first point is at 
	 * the origin of the local item coordinate system.
	 *
	 * Derived class implementations can add additional behavior.  These implementations should
	 * call the parent implementation first before adding custom logic.
	 *
	 * \sa moveItem(), rotateItem(), rotateBackItem(), flipItem()
	 */
	virtual void resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos);
	
	/*! \brief Rotates the item counter-clockwise within the scene.
	 *
	 * This function is called when the item's drawing() wants to rotate it within the scene. 
	 * This will only be called for items that have the #CanRotate flag set as one of
	 * their flags().
	 *
	 * The default implementation updates the position to rotate the item around the specified 
	 * scenePos and updates the item's rotation().
	 *
	 * Derived class implementations can add additional behavior.  These implementations should
	 * call the parent implementation first before adding custom logic.
	 *
	 * \sa moveItem(), resizeItem(), rotateBackItem(), flipItem()
	 */
	virtual void rotateItem(const QPointF& scenePos);
	
	/*! \brief Rotates the item clockwise within the scene.
	 *
	 * This function is called when the item's drawing() wants to rotate it within the scene. 
	 * This will only be called for items that have the #CanRotate flag set as one of
	 * their flags().
	 *
	 * The default implementation updates the position to rotate the item around the specified 
	 * scenePos and updates the item's rotation().
	 *
	 * Derived class implementations can add additional behavior.  These implementations should
	 * call the parent implementation first before adding custom logic.
	 *
	 * \sa moveItem(), resizeItem(), rotateItem(), flipItem()
	 */
	virtual void rotateBackItem(const QPointF& scenePos);
	
	/*! \brief Flips the item horizontally within the scene.
	 *
	 * This function is called when the item's drawing() wants to flip it within the scene. 
	 * This will only be called for items that have the #CanFlip flag set as one of
	 * their flags().
	 *
	 * The default implementation updates the position to flip the item about the specified 
	 * scenePos and updates the item's isFlipped() field.
	 *
	 * Derived class implementations can add additional behavior.  These implementations should
	 * call the parent implementation first before adding custom logic.
	 *
	 * \sa moveItem(), resizeItem(), rotateBackItem(), flipItem()
	 */
	virtual void flipItem(const QPointF& scenePos);

	/*! \brief Inserts a new DrawingItemPoint into the item at the specified position.
	 *
	 * This function is called when the item's drawing() wants to insert a new item point in the
	 * item.  This will only be called for items that have the #CanInsertPoints flag set as one of
	 * their flags(). 
	 *
	 * The #CanInsertPoints flag is not set by default, and the default implementation of this 
	 * function does nothing.  Derived classes that support inserting item points should set the
	 * #CanInsertPoints flag and provide an implementation for this function.
	 *
	 * \sa removeItemPoint()
	 */
	virtual void insertItemPoint(const QPointF& scenePos);
	
	/*! \brief Removes an existing DrawingItemPoint from the item at the specified position.
	 *
	 * This function is called when the item's drawing() wants to remove an item point from the
	 * item.  This will only be called for items that have the #CanRemovePoints flag set as one of
	 * their flags(). 
	 *
	 * The #CanRemovePoints flag is not set by default, and the default implementation of this 
	 * function does nothing.  Derived classes that support removing item points should set the
	 * #CanRemovePoints flag and provide an implementation for this function.
	 *
	 * \sa insertItemPoint()
	 */
	virtual void removeItemPoint(const QPointF& scenePos);

protected:
	/*! \brief Handles mouse press events for the item.
	 *
	 * This function does nothing.  It can be reimplemented in a derived class to add event
	 * handling behavior to the item.
	 *
	 * \sa mouseMoveEvent(), mouseReleaseEvent(), mouseDoubleClickEvent()
	 */
	virtual void mousePressEvent(DrawingMouseEvent* event);
	
	/*! \brief Handles mouse move events for the item.
	 *
	 * This function does nothing.  It can be reimplemented in a derived class to add event
	 * handling behavior to the item.
	 *
	 * \sa mousePressEvent(), mouseReleaseEvent(), mouseDoubleClickEvent()
	 */
	virtual void mouseMoveEvent(DrawingMouseEvent* event);
	
	/*! \brief Handles mouse release events for the item.
	 *
	 * This function does nothing.  It can be reimplemented in a derived class to add event
	 * handling behavior to the item.
	 *
	 * \sa mousePressEvent(), mouseMoveEvent(), mouseDoubleClickEvent()
	 */
	virtual void mouseReleaseEvent(DrawingMouseEvent* event);
	
	/*! \brief Handles mouse double-click events for the item.
	 *
	 * This function does nothing.  It can be reimplemented in a derived class to add event
	 * handling behavior to the item.
	 *
	 * \sa mousePressEvent(), mouseMoveEvent(), mouseReleaseEvent()
	 */
	virtual void mouseDoubleClickEvent(DrawingMouseEvent* event);
	
	/*! \brief Handles key press events for the item.
	 *
	 * This function does nothing.  It can be reimplemented in a derived class to add event
	 * handling behavior to the item.
	 *
	 * \sa keyReleaseEvent()
	 */
	virtual void keyPressEvent(QKeyEvent* event);
	
	/*! \brief Handles key release events for the item.
	 *
	 * This function does nothing.  It can be reimplemented in a derived class to add event
	 * handling behavior to the item.
	 *
	 * \sa keyReleaseEvent()
	 */
	virtual void keyReleaseEvent(QKeyEvent* event);
	
	
	/*! \brief Handles mouse press events for the item when the parent drawing() is in PlaceMode.
	 *
	 * The default implementation of this function updates the item's position to the
	 * current mouse location.  This event handler can be reimplemented in a derived class to 
	 * modify the default behavior.
	 *
	 * \sa newMouseMoveEvent(), newMouseReleaseEvent(), newMouseDoubleClickEvent()
	 */
	virtual void newMousePressEvent(DrawingMouseEvent* event);
	
	/*! \brief Handles mouse move events for the item when the parent drawing() is in PlaceMode.
	 *
	 * The default implementation of this function updates the item's position to the
	 * current mouse location.  This event handler can be reimplemented in a derived class to 
	 * modify the default behavior.
	 *
	 * \sa newMousePressEvent(), newMouseReleaseEvent(), newMouseDoubleClickEvent()
	 */
	virtual void newMouseMoveEvent(DrawingMouseEvent* event);
	
	/*! \brief Handles mouse release events for the item when the parent drawing() is in PlaceMode.
	 *
	 * The default implementation of this function updates the item's position to the
	 * current mouse location and return true.  This event handler can be reimplemented in 
	 * a derived class to modify the default behavior.
	 *
	 * \sa newMousePressEvent(), newMouseMoveEvent(), newMouseDoubleClickEvent()
	 */
	virtual bool newMouseReleaseEvent(DrawingMouseEvent* event);
	
	/*! \brief Handles mouse double-click events for the item when the parent drawing() is in PlaceMode.
	 *
	 * The default implementation of this function updates the item's position to the
	 * current mouse location and return true.  This event handler can be reimplemented in 
	 * a derived class to modify the default behavior.
	 *
	 * \sa newMousePressEvent(), newMouseMoveEvent(), newMouseReleaseEvent()
	 */
	virtual bool newMouseDoubleClickEvent(DrawingMouseEvent* event);

	/*! \brief Handles copy events for the item when the parent drawing() is in PlaceMode.
	 *
	 * This function is called on the newItem() of the DrawingWidget when it changes.  It can be
	 * used to set up the item to prepare it to be placed in the scene.  Often this is used to
	 * reset the item's geometry for items whose size is changed as they are placed in the scene.
	 *
	 * The default implementation does nothing except return true.
	 *
	 * \sa DrawingWidget::mouseReleaseEvent()
	 */
	virtual bool newItemCopyEvent();

private:
	void recalculateTransform();

public:
	/*! \brief Creates a copy of each of the specified items and returns them as a new list.
	 *
	 * This function iterates through each item and creates a copy using the copy() function.
	 * The copied items are returned in the same order as provided.
	 * 
	 * Any item point connections between items in the original list are maintained in the new
	 * list.  Any item point connections to items not in the original list are broken.
	 */
	static QList<DrawingItem*> copyItems(const QList<DrawingItem*>& items);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DrawingItem::Flags)

#endif
