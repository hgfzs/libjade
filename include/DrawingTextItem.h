/* DrawingTextItem.h
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

#ifndef DRAWINGTEXTITEM_H
#define DRAWINGTEXTITEM_H

#include <DrawingItem.h>

/*! \brief Provides a text item that can be added to a DrawingScene.
 *
 * To set the item's text, call the setCaption() function.  The caption() function returns the
 * current text.
 *
 * Rendering options for the item can be controlled through properties of the item's style().
 * The text item supports all of the font, text brush, and text alignment style properties.
 *
 * DrawingTextItem provides a reasonable implementation of boundingRect(), shape(), and isValid().
 * The render() function draws the text using the item's associated pen and brush.
 */
class DrawingTextItem : public DrawingItem
{
private:
	QString mCaption;

public:
	/*! \brief Create a new DrawingTextItem with default settings.
	 *
	 * This function creates one DrawingItemPoint object to serve as an anchor point for the text.
	 *
	 * This function fills in the item's style() with default values for the following properties.
	 * The default values are pulled from the style's DrawingItemStyle::defaultValues() if present,
	 * otherwise DrawingRectItem attempts to use reasonable initial values for each property:
	 * \li DrawingItemStyle::TextColor
	 * \li DrawingItemStyle::TextOpacity
	 * \li DrawingItemStyle::FontName
	 * \li DrawingItemStyle::FontSize
	 * \li DrawingItemStyle::FontBold
	 * \li DrawingItemStyle::FontItalic
	 * \li DrawingItemStyle::FontUnderline
	 * \li DrawingItemStyle::FontOverline
	 * \li DrawingItemStyle::FontStrikeThrough
	 * \li DrawingItemStyle::TextHorizontalAlignment
	 * \li DrawingItemStyle::TextVerticalAlignment
	 */
	DrawingTextItem();

	/*! \brief Create a new DrawingTextItem as a copy of an existing text item.
	 *
	 * Creates a new item style with all of the same properties as the existing item's style.
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


	/*! \brief Returns an estimate of the area painted by the text item.
	 *
	 * Calculates the bounding rect of the text based on the caption and properties of the
	 * item's style.
	 *
	 * \sa shape(), isValid()
	 */
	virtual QRectF boundingRect() const;

	/*! \brief Returns an accurate outline of the item's shape.
	 *
	 * Calculates the shape of the text based on the caption and properties of the
	 * item's style.  For text items, the shape is the same as the item's boundingRect().
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

	/*! \brief Return false if the item is degenerate, true otherwise.
	 *
	 * A text item is considered degenerate if its caption is empty.
	 *
	 * \sa boundingRect(), shape()
	 */
	virtual bool isValid() const;


	/*! \brief Paints the contents of the text item into the scene.
	 *
	 * The text is painted in the scene based on properties set by the item's style().
	 *
	 * At the end of this function, the QPainter object is returned to the same state that it was
	 * in when the function started.
	 */
	virtual void render(QPainter* painter);

private:
	QRectF calculateTextRect(const QString& caption, const QFont& font,
		Qt::Alignment textAlignment) const;
};

#endif
