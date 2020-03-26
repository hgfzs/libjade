/* DrawingItem.h
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

#ifndef DRAWINGITEM_H
#define DRAWINGITEM_H

#include <QtGui>

class DrawingScene;
class DrawingItemPoint;
class DrawingItemStyle;

/*! \brief Base class for all graphical items in a DrawingScene.
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
 * \li DrawingTextRectItem provides a rectangular item with text
 * \li DrawingTextEllipseItem provides an ellipse item with text
 * \li DrawingTextPolygonItem provides a polygon item with text
 *
 * All of an item's geometric information is based on its local coordinate system. The item's
 * position() is the only property that does not operate in local coordinates.
 *
 * Items can contain other items, and also be contained by other items.  All items can have a
 * parent item and a list of children.  Unless the item has no parent, its position is in
 * parent coordinates (i.e., the parent's local coordinates).  (If the item has no parent, its
 * position is in scene coordinates.)  Parent items propagate both their position and their
 * transformation to all children.
 *
 * An item can be set as visible (i.e., drawn and accepting events) by calling setVisible().
 * By default, items are set as visible and enabled.
 *
 * Normally, selection is toggled by the scene as a result of user interaction.  To determine if an
 * item is selected, call isSelected().
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
 * scene.  The default implementation is to return the center of the item's boundingRect().  For
 * most items this implementation is expected to be sufficient.
 *
 * The isValid() function should return false if the item is not really valid (for example, a line
 * where the start and end points are the same).  DrawingView will prevent invalid items from
 * being added to the scene.  The default implementation is to return true if the item's
 * boundingRect() is a valid rect.
 *
 * \section itemPoints Item Points
 *
 * An item's geometry may be represented by a series of item points.  For example, a line item may
 * have two points; one point represents the start point and the other represents the end point.
 *
 * Representing the item's geometry based on the location of its item points provides a convenient
 * method for the user to be able to resize an item.  When the user of a DrawingView clicks on one
 * of the items points(), then if the item is a Control point, the view will move just the point
 * around the scene rather than the whole item.
 *
 * Item points can be added to an item using addPoint() or insertPoint().  Points may be removed
 * from an item using removePoint().  A list of all the item's points is available using points().
 *
 * \section itemPainting Painting
 *
 * The render() function is called by DrawingScene to paint the item's contents. The item has no
 * background or default fill of its own; whatever is behind the item will show through all
 * areas that are not explicitly painted in this function.
 *
 * All items are drawn in the order of when they were added to scene.  This order also dictates
 * which items will receive mouse input first when clicking within the scene.
 *
 * DrawingItem has support for item styles via DrawingItemStyle.  Item styles affect how an
 * item is rendered within a DrawingScene.  See DrawingItemStyle for more information.
 *
 * \section itemEvents Events
 *
 * DrawingItem receives events from DrawingView through several event handlers:
 *
 * \li moveEvent() is called when moving the item around the scene
 * \li resizeEvent() is called when resizing the item within the scene
 * \li rotateEvent() and rotateBackEvent() are called when rotating the item within the scene
 * \li flipHorizontalEvent() and flipVerticalEvent() are called when flipping the item within the scene
 * \li keyPressEvent() and keyReleaseEvent() are called for keyboard events when the item is the focus item of the view
 *
 * These functions may be overloaded in a derived class to extend the behavior of each type of
 * event.
 *
 * These events are only called if their corresponding flag is set on the item.  For example, the
 * resizeEvent() slot is only called if the #CanResize flag is set. The default flags() set on a new
 * item are #CanMove | #CanResize | #CanRotate | #CanFlip | #CanSelect.
 *
 * \section itemCustom Custom Items
 *
 * To write a custom graphics item, first create a subclass of DrawingItem, and then start by
 * implementing its three pure virtual public functions: copy(), boundingRect() and render().
 * Complex items may need to override shape(), centerPos(), and isValid() for a more complete
 * description of the item's geometry.
 *
 * Consider using DrawingItemPoint objects to define the item's geometry to allow the user to
 * resize the item through DrawingView mouse events.
 *
 * To extend the behavior of the item interaction events within the scene, consider overriding any
 * of the item's event handlers such as moveEvent() or resizeEvent() as well.
 *
 * For items that support adding item points, implementations should set the #CanInsertPoints flag
 * and provide an implementation for itemPointToInsert().  For items that support removing item
 * points, implementations should set the #CanRemovePoints flag and provide an implementation for
 * itemPointToRemove().  By default itemPointToInsert() and itemPointToRemove() return nullptr.
 */
class DrawingItem
{
	friend class DrawingView;
	friend class DrawingScene;

public:
	//! \brief Enum used to affect the behavior of the DrawingItem within the scene.
	enum Flag
	{
		CanMove = 0x0001,						//!< Indicates that the item can be moved around the scene.
												//!< See also moveItem().
		CanResize = 0x0002,						//!< Indicates that the item can be resized within the
												//!< scene.  See also resizeItem().
		CanRotate = 0x0004,						//!< Indicates that the item can be rotated within the
												//!< scene.  See also rotateItem() and rotateBackItem().
		CanFlip = 0x0008,						//!< Indicates that the item can be flipped horizontally
												//!< within the scene.  See also flipItem().
		CanSelect = 0x0010,						//!< Indicates that the item can be selected by the user
												//!< within the scene.
		CanHide = 0x0020,
		CanDelete = 0x0040,
		CanInsertPoints = 0x0080,				//!< Indicates that item points can be added
												//!< to the item.  See also insertItemPoint().
		CanRemovePoints = 0x0100				//!< Indicates that item points can be removed
												//!< from the item.  See also removeItemPoint().
	};

	/*! \brief Logical OR of various #Flag values.
	 *
	 * \sa setFlags(), flags()
	 */
	Q_DECLARE_FLAGS(Flags, Flag)

private:
	DrawingScene* mScene;

	QPointF mPosition;
	QTransform mTransform, mTransformInverse;
	Flags mFlags;

	DrawingItemStyle* mStyle;

	QList<DrawingItemPoint*> mPoints;

	bool mVisible;
	bool mSelected;

public:
	/*! \brief Create a new DrawingItem with default settings.
	 *
	 * The new item is not associated with a DrawingScene and does not have a parent.
	 */
	DrawingItem();

	/*! \brief Create a new DrawingItem as a copy of an existing item.
	 *
	 * Copies the position, transform, and flags from the existing item.  Also creates copies
	 * of the existing item's points and the existing item's children for the new item.  Also
	 * creates a new item style based upon the existing item's style.
	 *
	 * The new item is not associated with a DrawingScene and does not have a parent.
	 */
	DrawingItem(const DrawingItem& item);

	//! \brief Delete an existing DrawingItem object.
	virtual ~DrawingItem();


	/*! \brief Creates a copy of the DrawingItem and returns it.
	 *
	 * It is recommended that derived classes implement a copy constructor to ensure that item
	 * copies are handled as expected.  Then this function can simply create a new item by calling
	 * the derived class copy constructor.
	 */
	virtual DrawingItem* copy() const = 0;


	/*! \brief Returns the current scene that this item is a member of, or nullptr if the item
	 * is not associated with a scene.
	 *
	 * To add an item to a scene, call DrawingScene::addItem() or DrawingScene::insertItem(). To
	 * remove an item from a scene, call DrawingScene::removeItem().
	 */
	DrawingScene* scene() const;


	/*! \brief Sets the position of the item.
	 *
	 * The position of the item describes its origin (local coordinate (0,0)) in scene
	 * coordinates.
	 *
	 * To move an item around the scene as if the user clicked on it, call
	 * DrawingView::moveItems() or DrawingView::moveSelection().
	 *
	 * \sa position()
	 */
	void setPosition(const QPointF& pos);

	/*! \brief Sets the position of the item.
	 *
	 * This convenience function is equivalent to calling setPosition(QPointF(x,y)).
	 *
	 * \sa position(), x(), y()
	 */
	void setPosition(qreal x, qreal y);

	/*! \brief Sets the x-coordinate of the item's position.
	 *
	 * This convenience function is equivalent to calling setPos(QPointF(x,y())).
	 *
	 * \sa setPosition(), x()
	 */
	void setX(qreal x);

	/*! \brief Sets the y-coordinate of the item's position.
	 *
	 * This convenience function is equivalent to calling setPos(QPointF(x(),y)).
	 *
	 * \sa setPosition(), y()
	 */
	void setY(qreal y);

	/*! \brief Returns the position of the item in the scene.
	 *
	 * \sa setPosition(), x(), y()
	 */
	QPointF position() const;

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


	/*! \brief Sets the item's current transformation matrix.
	 *
	 * This transform allows items to be rotated, scaled, or flipped within the scene.
	 *
	 * If combine is true, then matrix is combined with the current matrix; otherwise, matrix
	 * replaces the current matrix.
	 *
	 * To simplify interation with items using a transformed view, QGraphicsItem provides
	 * mapToParent(), mapFromParent(), mapToScene(), and mapFromScene() functions that can
	 * translate between item, parent, and scene coordinates.
	 *
	 * \sa transform(), transformInverted()
	 */
	void setTransform(const QTransform& transform, bool combine = false);

	/*! \brief Returns the item's current transformation matrix.
	 *
	 * \sa setTransform(), transformInverted()
	 */
	QTransform transform() const;

	/*! \brief Returns the inverse of the item's current transformation matrix.
	 *
	 * \sa setTransform(), transform()
	 */
	QTransform transformInverted() const;


	/*! \brief Sets the type of item through a combination of flags.
	 *
	 * The type of item is represented by a combination of flags.  Any combination of flags
	 * is valid for DrawingItem objects.  By default, the (#CanMove | #CanResize | #CanRotate |
	 * #CanFlip | #CanSelect) flags are set.
	 *
	 * Items that set the #CanInsertPoints flag should also provide an implementation for
	 * itemPointToInsert().  Items that set the #CanRemovePoints flag should also provide an
	 * implementation for and itemPointToRemove().
	 *
	 * \sa flags()
	 */
	void setFlags(Flags flags);

	/*! \brief Returns the type of the item as represented as a combination of flags.
	 *
	 * \sa setFlags()
	 */
	Flags flags() const;


	/*! \brief Sets the item's style.
	 *
	 * The item's style contains settings that affect how the item is rendered within a
	 * DrawingScene.  By default, the item is a assigned an empty style with no settings applied.
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
	 * The itemPos is given in local item coordinates.
	 *
	 * This function may return nullptr if no matching item point is found.
	 *
	 * \sa pointNearest()
	 */
	DrawingItemPoint* pointAt(const QPointF& itemPos) const;

	/*! \brief Returns the item point nearest to the specified position.
	 *
	 * This function calculates the distance between itemPos and each of the item's points.  It
	 * selects the point that is closest to itemPos and returns it.  The itemPos is given in
	 * local item coordinates.
	 *
	 * This function may return nullptr if the item does not have any points().
	 *
	 * \sa pointAt()
	 */
	DrawingItemPoint* pointNearest(const QPointF& itemPos) const;

	/*! \brief Called when the view wants to insert a new item point in the item at the
	 * specified position.
	 *
	 * This will only be called for items that have the #CanInsertPoints flag set as one of
	 * their flags().
	 *
	 * The #CanInsertPoints flag is not set by default, and the default implementation of this
	 * function does nothing except return nullptr.  Derived classes that support inserting item
	 * points should set the #CanInsertPoints flag and provide an implementation for this function.
	 *
	 * \sa itemPointToRemove()
	 */
	virtual DrawingItemPoint* itemPointToInsert(const QPointF& itemPos, int& index);

	/*! \brief Called when the view wants to remove an existing item point in the item at the
	 * specified position.
	 *
	 * This will only be called for items that have the #CanRemovePoints flag set as one of
	 * their flags().
	 *
	 * The #CanRemovePoints flag is not set by default, and the default implementation of this
	 * function does nothing except return nullptr.  Derived classes that support removing item
	 * points should set the #CanRemovePoints flag and provide an implementation for this function.
	 *
	 * \sa itemPointToInsert()
	 */
	virtual DrawingItemPoint* itemPointToRemove(const QPointF& itemPos);


	/*! \brief Sets whether the item is currently visible within the scene or not.
	 *
	 * Items that are not visible are not drawn and do not receive events.  By default, items are
	 * visible (isVisible() returns true).
	 *
	 * \sa isVisible()
	 */
	void setVisible(bool visible);

	/*! \brief Sets whether the item is currently selected within the scene or not.
	 *
	 * Items that are selected can be manipulated within the scene (i.e. moved, resized, rotated,
	 * flipped, etc).  By default, items are not selected (isSelected() returns false).
	 *
	 * \sa isSelected()
	 */
	void setSelected(bool select);

	/*! \brief Returns whether the item is currently visible within the scene or not.
	 *
	 * \sa setVisible()
	 */
	bool isVisible() const;

	/*! \brief Returns whether the item is currently selected within the scene or not.
	 *
	 * \sa setSelected()
	 */
	bool isSelected() const;


	/*! \brief Maps the point from the coordinate system of the scene to the item's
	 * coordinate system.
	 *
	 * \sa mapToScene(const QPointF&) const
	 */
	QPointF mapFromScene(const QPointF& point) const;

	/*! \brief Maps the rect from the coordinate system of the scene to the item's
	 * coordinate system.
	 *
	 * \sa mapToScene(const QRectF&) const
	 */
	QPolygonF mapFromScene(const QRectF& rect) const;

	/*! \brief Maps the polygon from the coordinate system of the scene to the item's
	 * coordinate system.
	 *
	 * \sa mapToScene(const QPolygonF&) const
	 */
	QPolygonF mapFromScene(const QPolygonF& polygon) const;

	/*! \brief Maps the path from the coordinate system of the scene to the item's
	 * coordinate system.
	 *
	 * \sa mapToScene(const QPainterPath&) const
	 */
	QPainterPath mapFromScene(const QPainterPath& path) const;

	/*! \brief Maps the point from the item's coordinate system to the coordinate system of the
	 * scene.
	 *
	 * \sa mapFromScene(const QPointF&) const
	 */
	QPointF mapToScene(const QPointF& point) const;

	/*! \brief Maps the rect from the item's coordinate system to the coordinate system of the
	 * scene.
	 *
	 * \sa mapFromScene(const QRectF&) const
	 */
	QPolygonF mapToScene(const QRectF& rect) const;

	/*! \brief Maps the polygon from the item's coordinate system to the coordinate system of the
	 * scene.
	 *
	 * \sa mapFromScene(const QPolygonF&) const
	 */
	QPolygonF mapToScene(const QPolygonF& polygon) const;

	/*! \brief Maps the path from the item's coordinate system to the coordinate system of the
	 * scene.
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
	 * items.  A better approach would be to determine a quick way to estimate this rect without
	 * creating a QPainterPath.
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
	 * where the start and end points are the same).  DrawingView will prevent invalid items from
	 * being added to the scene.
	 *
	 * The default implementation is to return true if the item's boundingRect() is a valid rect.
	 *
	 * \sa boundingRect(), shape(), centerPos()
	 */
	virtual bool isValid() const;


	/*! \brief Paints the contents of the item into the scene.
	 *
	 * This function is typically called by DrawingScene when rendering the scene.  DrawingScene
	 * handles all of the necessary transformations, so this function should paint the item in
	 * local item coordinates.
	 */
	virtual void render(QPainter* painter) = 0;

protected:
	/*! \brief Moves the item within the scene.
	 *
	 * This function is called when the item is to be moved within the scene.  This will only be
	 * called for items that have the #CanMove flag set as one of their flags().  Note that the
	 * parentPos parameter is in the coordinate system of the parent(), or the scene() if no
	 * parent is set.
	 *
	 * The default implementation simply calls setPosition() to update the item's position.
	 *
	 * Derived class implementations can add additional behavior.  These implementations should
	 * call the parent implementation first before adding custom logic.
	 *
	 * \sa resizeEvent(), rotateEvent(), rotateBackEvent(), flipHorizontalEvent(), flipVerticalEvent()
	 */
	virtual void moveEvent(const QPointF& scenePos);

	/*! \brief Resizes the item within the scene.
	 *
	 * This function is called when the item is to be resized within the scene.  This will only be
	 * called for items that have the #CanResize flag set as one of their flags().  Note that the
	 * parentPos parameter is in the coordinate system of the parent(), or the scene() if no
	 * parent is set.
	 *
	 * The default implementation calls itemPoint->setPosition() to update the point's position.
	 *
	 * Derived class implementations can add additional behavior.  These implementations should
	 * call the parent implementation first before adding custom logic.
	 *
	 * \sa moveEvent(), rotateEvent(), rotateBackEvent(), flipHorizontalEvent(), flipVerticalEvent()
	 */
	virtual void resizeEvent(DrawingItemPoint* itemPoint, const QPointF& scenePos);

	/*! \brief Rotates the item counter-clockwise within the scene.
	 *
	 * This function is called when the item is to be rotated within the scene.  This will only be
	 * called for items that have the #CanRotate flag set as one of their flags().  Note that the
	 * parentPos parameter is in the coordinate system of the parent(), or the scene() if no
	 * parent is set.
	 *
	 * The default implementation updates item's position() to rotate it around the specified
	 * parentPos and updates the item's transform().
	 *
	 * Derived class implementations can add additional behavior.  These implementations should
	 * call the parent implementation first before adding custom logic.
	 *
	 * \sa moveEvent(), resizeEvent(), rotateBackEvent(), flipHorizontalEvent(), flipVerticalEvent()
	 */
	virtual void rotateEvent(const QPointF& scenePos);

	/*! \brief Rotates the item clockwise within the scene.
	 *
	 * This function is called when the item is to be rotated back within the scene.  This will
	 * only be called for items that have the #CanRotate flag set as one of their flags().  Note
	 * that the parentPos parameter is in the coordinate system of the parent(), or the scene()
	 * if no parent is set.
	 *
	 * The default implementation updates item's position() to rotate it around the specified
	 * parentPos and updates the item's transform().
	 *
	 * Derived class implementations can add additional behavior.  These implementations should
	 * call the parent implementation first before adding custom logic.
	 *
	 * \sa moveEvent(), resizeEvent(), rotateEvent(), flipHorizontalEvent(), flipVerticalEvent()
	 */
	virtual void rotateBackEvent(const QPointF& scenePos);

	/*! \brief Flips the item horizontally within the scene.
	 *
	 * This function is called when the item is to be flipped horizontally within the scene.
	 * This will only be called for items that have the #CanFlip flag set as one of
	 * their flags().  Note that the parentPos parameter is in the coordinate system of the
	 * parent(), or the scene() if no parent is set.
	 *
	 * The default implementation updates item's position() to flip it about the specified
	 * parentPos and updates the item's transform().
	 *
	 * Derived class implementations can add additional behavior.  These implementations should
	 * call the parent implementation first before adding custom logic.
	 *
	 * \sa moveEvent(), resizeEvent(), rotateEvent(), rotateBackEvent(), flipVerticalEvent()
	 */
	virtual void flipHorizontalEvent(const QPointF& scenePos);

	/*! \brief Flips the item vertically within the scene.
	 *
	 * This function is called when the item is to be flipped vertically within the scene.
	 * This will only be called for items that have the #CanFlip flag set as one of
	 * their flags().  Note that the parentPos parameter is in the coordinate system of the
	 * parent(), or the scene() if no parent is set.
	 *
	 * The default implementation updates item's position() to flip it about the specified
	 * parentPos and updates the item's transform().
	 *
	 * Derived class implementations can add additional behavior.  These implementations should
	 * call the parent implementation first before adding custom logic.
	 *
	 * \sa moveEvent(), resizeEvent(), rotateEvent(), rotateBackEvent(), flipHorizontalEvent()
	 */
	virtual void flipVerticalEvent(const QPointF& scenePos);

	/*! \brief Receives key press events when the item is the focus item of the view.
	 *
	 * The default implementation does nothing.
	 *
	 * \sa keyReleaseEvent()
	 */
	virtual void keyPressEvent(QKeyEvent* event);

	/*! \brief Receives key release events when the item is the focus item of the view.
	 *
	 * The default implementation does nothing.
	 *
	 * \sa keyPressEvent()
	 */
	virtual void keyReleaseEvent(QKeyEvent* event);

protected:
	QPainterPath strokePath(const QPainterPath& path, const QPen& pen) const;

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
