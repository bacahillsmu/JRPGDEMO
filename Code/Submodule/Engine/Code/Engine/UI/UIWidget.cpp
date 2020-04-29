#include "Engine/UI/UIWidget.hpp"






// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// UIWidget;
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
UIWidget::UIWidget()
{
}

// -----------------------------------------------------------------------
UIWidget::UIWidget(const AABB2 boundingBox, const Vec4 virtualSize, const Vec4 virtualPosition)
{
	m_boundingBox = boundingBox;
	m_virtualSize = virtualSize;
	m_virtualPosition = virtualPosition;

	CalculateActualValues();
}

// -----------------------------------------------------------------------
UIWidget::~UIWidget()
{

}

// -----------------------------------------------------------------------
// Flow;
// -----------------------------------------------------------------------
void UIWidget::Render()
{
	if (m_shouldDisplay)
	{
		RenderSelf();
		for (auto& child : m_children)
		{
			child->Render();
		}
	}
}

// -----------------------------------------------------------------------
void UIWidget::RenderSelf()
{

}

// -----------------------------------------------------------------------
// Setting Virtual Options;
// -----------------------------------------------------------------------
void UIWidget::SetBoundingBox(const AABB2 boundingBox)
{
	m_boundingBox = boundingBox;
}

// -----------------------------------------------------------------------
void UIWidget::SetVirtualSize(const Vec4 virtualSize)
{
	m_virtualSize = virtualSize;
}

// -----------------------------------------------------------------------
void UIWidget::SetVirtualPosition(const Vec4 virtualPosition)
{
	m_virtualPosition = virtualPosition;
}

// -----------------------------------------------------------------------
// Calculating Actual Values;
// -----------------------------------------------------------------------
void UIWidget::CalculateActualValues()
{
	CalculateActualPosition();
	CalculateActualBounds();
}

// -----------------------------------------------------------------------
void UIWidget::CalculateActualPosition()
{
	// x, y represent percentage of our respect to bounding box "world", z, w is pixels on x-axies and y-axis respective;
	Vec2 newCenter = Vec2((m_boundingBox.maxs.x * m_virtualPosition.x) + m_virtualPosition.z,
						  (m_boundingBox.maxs.y * m_virtualPosition.y) + m_virtualPosition.w);

	m_actualPosition = newCenter;
}

// -----------------------------------------------------------------------
void UIWidget::CalculateActualBounds()
{
	// The original half extents of our initial bounding box before virtual size is applied;
	Vec2 originalHalfExtents = Vec2((m_boundingBox.maxs.x - m_boundingBox.mins.x) * 0.5f,
									(m_boundingBox.maxs.y - m_boundingBox.mins.y) * 0.5f);

	// x, y represent percentage of our respect to bounding box "world", z, w is pixels on x-axies and y-axis respective;
	Vec2 virtualHalfExtents = Vec2((originalHalfExtents.x * m_virtualSize.x) + m_virtualSize.z,
								   (originalHalfExtents.y * m_virtualSize.y) + m_virtualSize.w);

	Vec2 mins = Vec2(m_actualPosition.x - virtualHalfExtents.x, m_actualPosition.y - virtualHalfExtents.y);
	Vec2 maxs = Vec2(m_actualPosition.x + virtualHalfExtents.x, m_actualPosition.y + virtualHalfExtents.y);

	m_actualBounds = AABB2::MakeFromMinsMaxs(mins, maxs);
}

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// UILabel;
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
UILabel::UILabel()
{

}
