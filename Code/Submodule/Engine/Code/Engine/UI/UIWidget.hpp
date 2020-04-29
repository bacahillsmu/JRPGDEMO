#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec4.hpp"

#include <vector>

// -----------------------------------------------------------------------
// UIWidget;
// -----------------------------------------------------------------------
class UIWidget
{

public:

	UIWidget();
	UIWidget(const AABB2 boundingBox, const Vec4 virtualSize, const Vec4 virtualPosition);
	virtual ~UIWidget();

	// Flow;
	void Render();

	// Setting Virtual Options;
	void SetBoundingBox(const AABB2 boundingBox);
	void SetVirtualSize(const Vec4 virtualSize);
	void SetVirtualPosition(const Vec4 virtualPosition);

	// Calculating Actual Values; This is what should be called outside of code UIWidget;
	void CalculateActualValues();

private:

	// Calculating Actual Values; This is what should be called inside of code UIWidget;
	void CalculateActualPosition();
	void CalculateActualBounds();

protected:

	virtual void RenderSelf();

protected:

	bool m_shouldDisplay = true;

	// Heirarchy Information;
	UIWidget* m_parent = nullptr;
	UIWidget* m_sibling = nullptr;
	std::vector<UIWidget*> m_children;

	// The bounding box that we are constrained to, we will use this as our "world";
	AABB2 m_boundingBox;

	// Virtual Sizes with respect to our bounding box "world";
	// x, y represent percentage of our respect to bounding box "world", z, w is pixels on x-axies and y-axis respective;
	Vec4 m_virtualPosition;
	Vec4 m_virtualSize;

	// Actual Values;
	Vec2 m_actualPosition;
	AABB2 m_actualBounds;

public:

	// Create a child of Type T and add it to Children;
	template <typename T>
	T* CreateChild()
	{


	}


};

// -----------------------------------------------------------------------
// UILabel
// -----------------------------------------------------------------------
class UILabel : public UIWidget
{

public:

	UILabel();

};
