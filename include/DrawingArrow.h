/* DrawingArrow.h
 *
 * Copyright (C) 2019-2020 Jason Allen
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

#ifndef DRAWINGARROW_H
#define DRAWINGARROW_H

#include <QPainterPath>

/*! \brief Defines properties of an arrow that you might find on the one of a line item or equivalent.
 *
 * The important properties include:
 * \li style(): Defines the style of the arrow, whether its a traditional arrow, a triangle, a circle, etc.
 * \li size(): Defines the size of the arrow, typically given in the local coordinates system of the associated item
 *
 * The supported arrow styles are defined by the DrawingArrow::Style enumeration.
 *
 * The vector of the arrow defines which direction the arrow is pointing, and must be set using
 * setVector().
 */
class DrawingArrow
{
public:
	enum Style
	{
		None,				//!< No arrow. \image html arrow/arrow_none.png
		Normal,				//!< A normal arrow. \image html arrow/arrow_normal.png
		Triangle,			//!< A triangle. \image html arrow/arrow_triangle.png
		TriangleFilled,		//!< A filled triangle. \image html arrow/arrow_triangle_filled.png
		Concave,			//!< A concave arrow. \image html arrow/arrow_concave.png
		ConcaveFilled,		//!< A filled concave arrow. \image html arrow/arrow_concave_filled.png
		Circle,				//!< A circle. \image html arrow/arrow_circle.png
		CircleFilled		//!< A filled circle. \image html arrow/arrow_circle_filled.png
	};

private:
	QPointF mPosition;
	qreal mPenWidth;
	qreal mLength;
	qreal mAngle;
	Style mStyle;
	qreal mSize;

	QPainterPath mShape;
	QPolygonF mPolygon;

public:
	/*! \brief Create a new DrawingRectItem with default settings.
	 *
	 */
	DrawingArrow(Style style = None, qreal size = 0);

	/*! \brief Create a new DrawingArrow as a copy of an existing arrow.
	 *
	 */
	DrawingArrow(const DrawingArrow& arrow);

	/*! \brief Delete an existing DrawingArrow object.
	 *
	 */
	~DrawingArrow();


	/*! \brief Copy this arrow's settings to another arrow.
	 *
	 */
	DrawingArrow& operator=(const DrawingArrow& arrow);


	/*! \brief Set the vector properties of the arrow.
	 *
	 * The position, penWidth, length, and angle affect the location, size, and orientation of
	 * the rendered arrow.
	 *
	 * \sa vectorPosition(), vectorPenWidth(), vectorLength(), vectorAngle()
	 */
	void setVector(const QPointF& pos, qreal penWidth, qreal length, qreal angle);

	/*! \brief Set the style of the arrow.
	 *
	 * \sa setSize(), style()
	 */
	void setStyle(Style style);

	/*! \brief Set the size of the arrow.
	 *
	 * The arrow size is typically given in the local coordinates system of the associated item.
	 *
	 * \sa setStyle(), size()
	 */
	void setSize(qreal size);

	/*! \brief Return the vector position of the arrow.
	 *
	 * \sa setVector(), vectorPenWidth(), vectorLength(), vectorAngle()
	 */
	QPointF vectorPosition() const;

	/*! \brief Return the vector pen width of the arrow.
	 *
	 * \sa setVector(), vectorPosition(), vectorLength(), vectorAngle()
	 */
	qreal vectorPenWidth() const;

	/*! \brief Return the vector length of the arrow.
	 *
	 * \sa setVector(), vectorPosition(), vectorPenWidth(), vectorAngle()
	 */
	qreal vectorLength() const;

	/*! \brief Return the vector angle of the arrow.
	 *
	 * \sa setVector(), vectorPosition(), vectorPenWidth(), vectorLength()
	 */
	qreal vectorAngle() const;

	/*! \brief Return the style of the arrow.
	 *
	 * \sa setStyle(), size()
	 */
	Style style() const;

	/*! \brief Return the size of the arrow.
	 *
	 * \sa setSize(), style()
	 */
	qreal size() const;


	/*! \brief Returns an accurate outline of the arrow's shape.
	 *
	 * Calculates the shape of the rect based on its vector properties, style(), and size().
	 *
	 * \sa setVector(), setStyle(), setSize()
	 */
	QPainterPath shape() const;


	/*! \brief Paints the arrow into the scene.
	 *
	 * The arrow is painted in the scene using its vector properties, style(), and size().
	 *
	 * At the end of this function, the QPainter object is returned to the same state that it was
	 * in when the function started.
	 *
	 * \sa setVector(), setStyle(), setSize()
	 */
	void render(QPainter* painter, const QBrush& backgroundBrush);

private:
	void updateGeometry();
	QPainterPath strokePath(const QPainterPath& path) const;
};

#endif
