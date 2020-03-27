/* DrawingTextRectItem.h
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

#ifndef DRAWINGTEXTRECTITEM_H
#define DRAWINGTEXTRECTITEM_H

#include <DrawingItem.h>
#include <QFont>

/*! \brief Provides a text rectangle item that can be added to a DrawingScene.
 *
 * To set the item's rect, call the setRect() function.  The rect() function returns the
 * current rect.  Both functions operate in local item coordinates.
 *
 * To set the item's text, call the setCaption() function.  The caption() function returns the
 * current text.
 *
 * DrawingTextRectItem provides a reasonable implementation of boundingRect(), shape(), and isValid().
 * The render() function draws the text rect using the item's pen(), brush(), font(), and textColor().
 */
class DrawingTextRectItem : public DrawingItem
{
private:
	enum PointIndex { TopLeft, BottomRight, TopRight, BottomLeft, TopMiddle, MiddleRight,
					  BottomMiddle, MiddleLeft };

private:
	QRectF mRect;
	qreal mCornerRadius;
	QPen mPen;
	QBrush mBrush;

	QString mCaption;
	QPen mTextPen;
	QFont mFont;

	QRectF mBoundingRect;
	QPainterPath mShape;
	QRectF mRectBoundingRect;
	QPainterPath mRectShape;
	QRectF mTextBoundingRect;

public:
	/*! \brief Create a new DrawingTextRectItem with default settings.
	 *
	 * This function creates eight DrawingItemPoint objects and adds them to the item.  These
	 * item points represent the bounding points of the text rect.
	 */
	DrawingTextRectItem();

	/*! \brief Create a new DrawingTextRectItem as a copy of an existing text rect item.
	 *
	 * Creates copies of all item points to the new rect item, including the points' positions.
	 */
	DrawingTextRectItem(const DrawingTextRectItem& item);

	/*! \brief Delete an existing DrawingTextRectItem object.
	 *
	 * All of the item's points are also deleted.
	 */
	virtual ~DrawingTextRectItem();


	/*! \brief Creates a copy of the DrawingTextRectItem and return it.
	 *
	 * Simply calls the copy constructor.
	 */
	virtual DrawingItem* copy() const;


	/*! \brief Sets the item's rect to rect, which is given in local item coordinates.
	 *
	 * \sa rect()
	 */
	void setRect(const QRectF& rect);

	/*! \brief Sets the item's rect, which is given in local item coordinates.
	 *
	 * This convenience function is equivalent to calling
	 * \link setRect(const QRectF& rect) setRect(QRectF(left, top, width, height)) \endlink .
	 *
	 * \sa rect()
	 */
	void setRect(qreal left, qreal top, qreal width, qreal height);

	/*! \brief Returns the item's rect in local item coordinates.
	 *
	 * \sa setRect(const QRectF&), setRect(qreal, qreal, qreal, qreal)
	 */
	QRectF rect() const;


	/*! \brief Sets the item's corner radius as given in local item coordinates.
	 *
	 * \sa cornerRadius()
	 */
	void setCornerRadius(qreal radius);

	/*! \brief Returns the item's corner radius in local item coordinates.
	 *
	 * \sa setCornerRadius()
	 */
	qreal cornerRadius() const;


	/*! \brief Sets the pen used to draw the border of the rect.
	 *
	 * The pen's width is in local item coordinates.
	 *
	 * \sa pen()
	 */
	void setPen(const QPen& pen);

	/*! \brief Returns the pen used to draw the border of the rect.
	 *
	 * \sa setPen()
	 */
	QPen pen() const;


	/*! \brief Sets the brush used to fill the rect.
	 *
	 * \sa brush()
	 */
	void setBrush(const QBrush& brush);

	/*! \brief Returns the brush used to fill the rect.
	 *
	 * \sa setBrush()
	 */
	QBrush brush() const;


	/*! \brief Sets the item's text to caption.
	 *
	 * \sa caption()
	 */
	void setCaption(const QString& caption);

	/*! \brief Returns the item's text.
	 *
	 * \sa setCaption()
	 */
	QString caption() const;


	/*! \brief Sets the item's text color to color.
	 *
	 * \sa textColor()
	 */
	void setTextColor(const QColor& color);

	/*! \brief Returns the item's text color.
	 *
	 * \sa setTextColor()
	 */
	QColor textColor() const;

	/*! \brief Sets the item's font to font.
	 *
	 * The font's size is assumed to be in local item coordinates.
	 *
	 * \sa font()
	 */
	void setFont(const QFont& font);

	/*! \brief Returns the item's font.
	 *
	 * \sa setFont()
	 */
	QFont font() const;


	/*! \brief Sets the values of all item properties.
	 *
	 * The supported properties are listed below:
	 *
	 * <table>
	 *   <tr>
	 *     <th>Name</th>
	 *     <th>Type</th>
	 *     <th>Description</th>
	 *   </tr>
	 *   <tr>
	 *     <td>pen-color</td>
	 *     <td>QColor</td>
	 *     <td>Color of the item's pen(), including alpha channel</td>
	 *   </tr>
	 *   <tr>
	 *     <td>pen-width</td>
	 *     <td>qreal</td>
	 *     <td>Width of the item's pen()</td>
	 *   </tr>
	 *   <tr>
	 *     <td>pen-style</td>
	 *     <td>unsigned int</td>
	 *     <td>Style of the item's pen(), casted from Qt::PenStyle</td>
	 *   </tr>
	 *   <tr>
	 *     <td>pen-cap-style</td>
	 *     <td>unsigned int</td>
	 *     <td>Cap style of the item's pen(), casted from Qt::PenCapStyle</td>
	 *   </tr>
	 *   <tr>
	 *     <td>pen-join-style</td>
	 *     <td>unsigned int</td>
	 *     <td>Join style of the item's pen(), casted from Qt::PenJoinStyle</td>
	 *   </tr>
	 *   <tr>
	 *     <td>brush-color</td>
	 *     <td>QColor</td>
	 *     <td>Color of the item's brush(), including alpha channel</td>
	 *   </tr>
	 *   <tr>
	 *     <td>corner-radius</td>
	 *     <td>qreal</td>
	 *     <td>The item's cornerRadius()</td>
	 *   </tr>
	 *   <tr>
	 *     <td>caption</td>
	 *     <td>QString</td>
	 *     <td>The item's caption()</td>
	 *   </tr>
	 *   <tr>
	 *     <td>text-color</td>
	 *     <td>QColor</td>
	 *     <td>The item's textColor(), including alpha channel</td>
	 *   </tr>
	 *   <tr>
	 *     <td>font-family</td>
	 *     <td>QString</td>
	 *     <td>The family of the item's font()</td>
	 *   </tr>
	 *   <tr>
	 *     <td>font-size</td>
	 *     <td>qreal</td>
	 *     <td>The size of the item's font()</td>
	 *   </tr>
	 *   <tr>
	 *     <td>font-bold</td>
	 *     <td>bool</td>
	 *     <td>The bold setting of the item's font()</td>
	 *   </tr>
	 *   <tr>
	 *     <td>font-italic</td>
	 *     <td>bool</td>
	 *     <td>The italic setting of the item's font()</td>
	 *   </tr>
	 *   <tr>
	 *     <td>font-underline</td>
	 *     <td>bool</td>
	 *     <td>The underline setting of the item's font()</td>
	 *   </tr>
	 *   <tr>
	 *     <td>font-strike-through</td>
	 *     <td>bool</td>
	 *     <td>The strike-through setting of the item's font()</td>
	 *   </tr>
	 * </table>
	 *
	 * \sa properties()
	 */
	void setProperties(const QHash<QString,QVariant>& properties);

	/*! \brief Returns the values of all item properties.
	 *
	 * See the setProperties() function for the list of properties supported.
	 */
	QHash<QString,QVariant> properties() const;


	/*! \brief Returns an estimate of the area painted by the text rect item.
	 *
	 * Calculates the bounding rect of the text rect based on the position of its points.
	 * The rect includes an adjustment for the size of the item's text.
	 *
	 * \sa shape(), isValid()
	 */
	virtual QRectF boundingRect() const;

	/*! \brief Returns an accurate outline of the item's shape.
	 *
	 * Calculates the shape of the text rect based on the position of its points and the size of
	 * the item's text.
	 *
	 * \sa boundingRect(), isValid()
	 */
	virtual QPainterPath shape() const;

	/*! \brief Return false if the item is invalid, true otherwise.
	 *
	 * A text rect item is considered invalid if the positions of all of its points
	 * are the same and the item's caption is empty.
	 *
	 * \sa boundingRect(), shape()
	 */
	virtual bool isValid() const;


	/*! \brief Paints the contents of the text rect item into the scene.
	 *
	 * The text rect is painted in the scene using the item's pen(), brush(), cornerRadius(),
	 * caption(), textColor(), and font().
	 *
	 * At the end of this function, the QPainter object is returned to the same state that it was
	 * in when the function started.
	 */
	virtual void render(QPainter* painter);


	/*! \brief Resizes the item within the scene.
	 *
	 * This function ensures that whenever the item is resized, all of the item's points are resized
	 * to maintain position on the item's perimeter.
	 */
	virtual void resize(DrawingItemPoint* point, const QPointF& pos);

private:
	void updateGeometry();
	void updateTextRect(const QFont& font);
};

#endif
