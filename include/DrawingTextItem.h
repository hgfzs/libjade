/* DrawingTextItem.h
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

#ifndef DRAWINGTEXTITEM_H
#define DRAWINGTEXTITEM_H

#include <DrawingItem.h>
#include <QFont>

/*! \brief Provides a text item that can be added to a DrawingScene.
 *
 * To set the item's text, call the setCaption() function.  The caption() function returns the
 * current text.
 *
 * DrawingTextItem provides a reasonable implementation of boundingRect(), shape(), and isValid().
 * The render() function draws the text using the item's font() and textColor().
 */
class DrawingTextItem : public DrawingItem
{
private:
	QString mCaption;
	QPen mPen;
	QFont mFont;
	Qt::Alignment mAlignment;

	QRectF mTextRect;
	QPainterPath mTextShape;

public:
	/*! \brief Create a new DrawingTextItem with default settings.
	 *
	 * This function creates one DrawingItemPoint object to serve as an anchor point for the text.
	 */
	DrawingTextItem();

	/*! \brief Create a new DrawingTextItem as a copy of an existing text item.
	 *
	 * Creates a copy of the item's point in the new item.
	 */
	DrawingTextItem(const DrawingTextItem& item);

	/*! \brief Delete an existing DrawingTextItem object.
	 *
	 * The item's point is also deleted.
	 */
	virtual ~DrawingTextItem();


	/*! \brief Creates a copy of the DrawingTextItem and return it.
	 *
	 * Simply calls the copy constructor.
	 */
	virtual DrawingItem* copy() const;


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


	/*! \brief Sets the item's text alignment to alignment.
	 *
	 * This parameter affects where the item text is drawn relative to the anchor point.
	 * For example, if the alignment is Qt::AlignLeft | Qt::AlignBottom, then the text is
	 * drawn to the right and above the origin.
	 *
	 * \sa alignment()
	 */
	void setAlignment(Qt::Alignment alignment);

	/*! \brief Returns the item's text alignment.
	 *
	 * \sa setAlignment()
	 */
	Qt::Alignment alignment() const;


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
	 *   <tr>
	 *     <td>text-alignment-horizontal</td>
	 *     <td>unsigned int</td>
	 *     <td>Horizontal component of the item's alignment(), casted from Qt::Alignment</td>
	 *   </tr>
	 *   <tr>
	 *     <td>text-alignment-vertical</td>
	 *     <td>unsigned int</td>
	 *     <td>Vertical component of the item's alignment(), casted from Qt::Alignment</td>
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


	/*! \brief Returns an estimate of the area painted by the text item.
	 *
	 * Calculates the bounding rect of the text based on the caption and properties of the
	 * item's font().
	 *
	 * \sa shape(), isValid()
	 */
	virtual QRectF boundingRect() const;

	/*! \brief Returns an accurate outline of the item's shape.
	 *
	 * Calculates the shape of the text based on the caption and properties of the
	 * item's font().  For text items, the shape is the same as the item's boundingRect().
	 *
	 * \sa boundingRect(), isValid()
	 */
	virtual QPainterPath shape() const;

	/*! \brief Returns a position representing the center of the item.
	 *
	 * For text items, the center position is the item's origin.
	 *
	 * \sa boundingRect(), shape(), isValid()
	 */
	virtual QPointF centerPos() const;

	/*! \brief Return false if the item is invalid, true otherwise.
	 *
	 * A text item is considered invalid if its caption is empty.
	 *
	 * \sa boundingRect(), shape()
	 */
	virtual bool isValid() const;


	/*! \brief Paints the contents of the text item into the scene.
	 *
	 * The text is painted in the scene using the item's caption(), textColor(), font(), and
	 * alignment().
	 *
	 * At the end of this function, the QPainter object is returned to the same state that it was
	 * in when the function started.
	 */
	virtual void render(QPainter* painter);

protected:
	void updateTextRect(const QFont& font);
};

#endif
