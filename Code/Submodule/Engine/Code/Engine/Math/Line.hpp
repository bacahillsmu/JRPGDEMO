#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"

//-----------------------------------------------------------------------------------------------
class Line
{

public:

	Line() {}
	~Line() {}
	explicit Line( const Vec2& initialLineStart, const Vec2& initialLineEnd, float initialLineThickness = 0.0f );

	void SetFromText(const char* text);

public:

	Vec2 lineStart;
	Vec2 lineEnd;
	float lineThickness;
	Vec2 m_surfaceNormal;
};

