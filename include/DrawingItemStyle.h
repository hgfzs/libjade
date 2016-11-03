/* DrawingItemStyle.h
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

#ifndef DRAWINGITEMSTYLE_H
#define DRAWINGITEMSTYLE_H

#include <DrawingTypes.h>

/*! \brief Class for managing common item style properties.
 *
 * Each DrawingItem object is created with an empty DrawingItemStyle object, which can be
 * accessed using DrawingItem::style().  Properties can be added to this style using setValue() and
 * retrieved using value().  Some derived classes of DrawingItem may have filled in some style
 * properties already.
 *
 * The list of supported item style properties is given by the DrawingItemStyle::Property enum.
 * Properties are stored as QVariant objects.  Each property is associated with a variant of a 
 * specific data type.  DrawingItemStyle assumes that properties are set using the specific data
 * type; use of other data types will result in undefined behavior.
 *
 * DrawingItemStyle also supports a set of default style properties.  If a property is not set on 
 * a particular style, DrawingItemStyle will attempt to use the default property value.
 *
 * DrawingItem classes should use the valueLookup() functions to determine the property value using
 * default style properties.  If the style has a value() for the specified property, that value is
 * returned.  If not, then if the DrawingItemStyle has a defaultValue() for the specified property,
 * then that default value is returned.  It is recommended that each supported #Property have a 
 * default value stored using setDefaultValue().  If neither a local value or default value is found
 * for the specified property, the behavior is not well defined.
 *
 * DrawingItemStyle provides several convenience functions to look up common properties: pen(),
 * brush(), font(), textBrush(), textAlignment(), startArrowStyle(), startArrowSize(),
 * endArrowStyle(), and endArrowSize().  These functions use valueLookup() to find each related
 * property for that function.
 */
class DrawingItemStyle
{
public:
	//! \brief Enum represents the supported item style properties.
	enum Property 
	{ 
		PenStyle,					//!< (Qt::PenStyle) Represents the style of the pen stroke
		PenColor,					//!< (QColor) Represents the color of the pen stroke
		PenOpacity,					//!< (qreal) Represents the opacity of the pen stroke (allowed range is 0.0 - 1.0)
		PenWidth,					//!< (qreal) Represents the width of the pen stroke
		PenCapStyle,				//!< (Qt::PenCapStyle) Represents the cap style of the pen stroke
		PenJoinStyle,				//!< (Qt::PenJoinStyle) Represents the join style of the pen stroke
		BrushStyle,					//!< (Qt::BrushStyle) Represents the style of brush fill
		BrushColor,					//!< (QColor) Represents the color of the brush fill
		BrushOpacity,				//!< (qreal) Represents the opacity of the brush fill (allowed range is 0.0 - 1.0)
		FontName,					//!< (QString) Represents the font name
		FontSize,					//!< (qreal) Represents the font size, given in scene coordinates, not point size
		FontBold,					//!< (bool) Represents whether of not the font is bold
		FontItalic,					//!< (bool) Represents whether of not the font is bold
		FontUnderline,				//!< (bool) Represents whether of not the font is bold
		FontOverline,				//!< (bool) Represents whether of not the font is bold
		FontStrikeThrough,			//!< (bool) Represents whether of not the font is bold
		TextColor,					//!< (QColor) Represents the text color
		TextOpacity,				//!< (qreal) Represents the opacity of the text color (allowed range is 0.0 - 1.0)
		TextHorizontalAlignment,	//!< (Qt::Alignment) Represents the horizontal alignment of the text
		TextVerticalAlignment,		//!< (Qt::Alignment) Represents the vertical alignment of the text
		StartArrowStyle,			//!< (DrawingItemStyle::ArrowStyle) Represents the style of the arrow at the start point of a line item
		StartArrowSize,				//!< (qreal) Represents the size of the arrow at the start point of a line item
		EndArrowStyle,				//!< (DrawingItemStyle::ArrowStyle) Represents the style of the arrow at the end point of a line item
		EndArrowSize,				//!< (qreal) Represents the size of the arrow at the end point of a line item
		NumberOfProperties
	};
	
	//! \brief Enum represents the supported arrow styles.
	enum ArrowStyle
	{
		ArrowNone,					//!< No arrow is set on the item
		ArrowNormal,				//!< A normal arrow
		ArrowReverse,				//!< An inverted arrow (points the opposite direction of the normal arrow)
		ArrowTriangle,				//!< A triangle arrow
		ArrowTriangleFilled,		//!< A triangle arrow filled with the pen color
		ArrowConcave,				//!< A concave arrow
		ArrowConcaveFilled,			//!< A concave arrow filled with the pen color
		ArrowCircle,				//!< A circle centered on the start or end point
		ArrowCircleFilled,			//!< A circle centered on the start or end point filled with the pen color
		ArrowDiamond,				//!< A diamond centered on the start or end point
		ArrowDiamondFilled,			//!< A diamond centered on the start or end point filled with the pen color
		ArrowHarpoon,				//!< A harpoon arrow
		ArrowHarpoonMirrored,		//!< A harpoon arrow (opposite of the normal harpoon)
		ArrowX						//!< An X centered on the start or end point
	};

private:
	QHash<Property,QVariant> mProperties;

public:
	/*! \brief Create a new DrawingItemStyle.
	 *
	 * The new style does not have any property values set on it.
	 */
	DrawingItemStyle();
	
	/*! \brief Create a new DrawingItemStyle based on the specified style.
	 *
	 * The new style has the same properties as the existing style.
	 */
	DrawingItemStyle(const DrawingItemStyle& style);
	
	/*! \brief Delete an existing DrawingItemStyle object.
	 *
	 * All style properties are cleared.
	 */
	~DrawingItemStyle();

	
	/*! \brief Set the style's properties and values.
	 *
	 * Existing properties and values are cleared to make way for the new values.
	 *
	 * \sa setValue(), values()
	 */
	void setValues(const QHash<Property,QVariant>& values);
	
	/*! \brief Return all of the style's properties and values.
	 *
	 * \sa setValues(), value()
	 */
	QHash<Property,QVariant> values() const;

	
	/*! \brief Set the value of the specified property to value.
	 *
	 * \sa setValues(), unsetValue(), value()
	 */
	void setValue(Property index, const QVariant& value);
	
	/*! \brief Unset the value of the specified property.
	 *
	 * \sa setValue(), clearValues()
	 */
	void unsetValue(Property index);
	
	/*! \brief Clear all property values from the style.
	 *
	 * \sa unsetValue()
	 */
	void clearValues();	
	
	/*! \brief Returns true if the style has a value set for the specific property, false otherwise.
	 *
	 * \sa value()
	 */
	bool hasValue(Property index) const;
	
	/*! \brief Return the style's value for a specific property.
	 *
	 * If the style does not have a value for the property, an empty QVariant object is returned.
	 *
	 * \sa setValue(), valueLookup()
	 */
	QVariant value(Property index) const;
	
	
	/*! \brief Return the value for a specific property based on the style's value or a default value.
	 *
	 * If the style has a value for the property, that value is returned.  If not, and the 
	 * DrawingItemStyle has a default value for the property, that value is returned.  If no
	 * value is found in either the style's local properties or the class's default properties,
	 * an empty QVariant object is returned.
	 *
	 * \sa value(), valueLookup(Property, const QVariant&)
	 */
	QVariant valueLookup(Property index) const;
	
	/*! \brief Return the value for a specific property based on the style's value or a default value.
	 *
	 * If the style has a value for the property, that value is returned.  If not, and the 
	 * DrawingItemStyle has a default value for the property, that value is returned.  If no
	 * value is found in either the style's local properties or the class's default properties,
	 * the specified fallback value is returned.
	 *
	 * \sa value(), valueLookup(Property, const QVariant&)
	 */
	QVariant valueLookup(Property index, const QVariant& fallbackValue) const;
	
	
	/*! \brief Convenience function that creates a QPen object based upon the style's pen properties.
	 *
	 * This function uses valueLookup() to get the values of the #PenStyle, #PenColor, #PenOpacity,
	 * #PenWidth, #PenCapStyle, and #PenJoinStyle.  These values are combined in a QPen object
	 * and returned.
	 */
	QPen pen() const;
	
	/*! \brief Convenience function that creates a QBrush object based upon the style's brush properties.
	 *
	 * This function uses valueLookup() to get the values of the #BrushStyle, #BrushColor, and 
	 * #BrushOpacity.  These values are combined in a QBrush object and returned.
	 */
	QBrush brush() const;
	
	/*! \brief Convenience function that creates a QFont object based upon the style's font properties.
	 *
	 * This function uses valueLookup() to get the values of the #FontName, #FontSize, #FontBold, 
	 * #FontItalic, #FontUnderline, #FontOverline, and #FontStrikeThrough.  These values are 
	 * combined in a QFont object and returned.
	 */
	QFont font() const;
	
	/*! \brief Convenience function that creates a QBrush object based upon the style's text 
	 * brush properties.
	 *
	 * This function uses valueLookup() to get the values of the #TextColor and #TextOpacity.
	 * These values are combined in a QBrush object and returned.
	 */
	QBrush textBrush() const;
	
	/*! \brief Convenience function that creates a Qt::Alignment variable based upon the style's 
	 * text alignment properties.
	 *
	 * This function uses valueLookup() to get the values of the #TextHorizontalAlignment and 
	 * #TextVerticalAlignment. These values are combined and returned as a Qt::Alignment variable.
	 */
	Qt::Alignment textAlignment() const;
	
	/*! \brief Convenience function that determines the start arrow style based upon the 
	 * style's properties.
	 *
	 * This function uses valueLookup() to get the value of the #StartArrowStyle property. 
	 * This value is returned as an #ArrowStyle variable.
	 */
	ArrowStyle startArrowStyle() const;
	
	/*! \brief Convenience function that determines the start arrow size based upon the 
	 * style's properties.
	 *
	 * This function uses valueLookup() to get the value of the #StartArrowSize property. 
	 * This value is returned as a double-precision floating-point variable.
	 */
	qreal startArrowSize() const;
	
	/*! \brief Convenience function that determines the end arrow style based upon the 
	 * style's properties.
	 *
	 * This function uses valueLookup() to get the value of the #EndArrowStyle property. 
	 * This value is returned as an #ArrowStyle variable.
	 */
	ArrowStyle endArrowStyle() const;
	
	/*! \brief Convenience function that determines the end arrow size based upon the 
	 * style's properties.
	 *
	 * This function uses valueLookup() to get the value of the #EndArrowSize property. 
	 * This value is returned as a double-precision floating-point variable.
	 */
	qreal endArrowSize() const;


	/*! \brief Convenience function that draws an arrow using the specified painter.
	 *
	 * The arrow is drawn from the given position in the specified direction.  When this function
	 * returns, the painter is in the same state as when the function was entered.
	 *
	 * \sa arrowShape()
	 */
	void drawArrow(QPainter* painter, ArrowStyle style, qreal size,
		const QPointF& pos, qreal direction, const QPen& pen, const QBrush& backgroundBrush);

	/*! \brief Convenience function that returns the shape of a particular arrow style as a
	 * QPainterPath.
	 *
	 * The arrow's path is drawn from the given position in the specified direction.
	 *
	 * \sa arrowShape()
	 */
	QPainterPath arrowShape(ArrowStyle style, qreal size,
		const QPointF& pos, qreal direction) const;

private:
	QPolygonF calculateArrowPoints(ArrowStyle style, qreal size,
		const QPointF& pos, qreal direction) const;


private:
	static QHash<Property,QVariant> mDefaultProperties;
	
public:
	/*! \brief Set the default properties and values for all DrawingItemStyle objects.
	 *
	 * Existing default properties and values are cleared to make way for the new values.
	 *
	 * \sa setDefaultValue(), defaultValues()
	 */
	static void setDefaultValues(const QHash<Property,QVariant>& values);
	
	/*! \brief Return all of the DrawingItemStyle default properties and values.
	 *
	 * \sa setDefaultValues(), defaultValue()
	 */
	static QHash<Property,QVariant> defaultValues();

	
	/*! \brief Set the value of the specified default property to value.
	 *
	 * \sa setDefaultValue(), unsetDefaultValue(), defaultValue()
	 */
	static void setDefaultValue(Property index, const QVariant& value);
	
	/*! \brief Unset the value of the specified default property.
	 *
	 * \sa setDefaultValue(), clearDefaultValues()
	 */
	static void unsetDefaultValue(Property index);
	
	/*! \brief Clear all default property values for all DrawingItemStyle objects.
	 *
	 * \sa unsetDefaultValue()
	 */
	static void clearDefaultValues();	
	
	/*! \brief Returns true if the DrawingItemStyle has a default value set for the specific 
	* property, false otherwise.
	 *
	 * \sa defaultValue()
	 */
	static bool hasDefaultValue(Property index);
	
	/*! \brief Return the default value for a specific property used for all DrawingItemStyle 
	 * objects.
	 *
	 * If DrawingItemStyle does not have a default value for the property, an empty QVariant 
	 * object is returned.
	 *
	 * \sa setValue(), valueLookup()
	 */
	static QVariant defaultValue(Property index);
};

#endif
