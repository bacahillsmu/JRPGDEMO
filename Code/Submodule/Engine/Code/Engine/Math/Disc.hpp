#include "MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"

//-----------------------------------------------------------------------------------------------
struct Disc
{
public:
	Vec2 discCenter;
	float discRadius;

public:

	Disc() {}
	~Disc() {}
	explicit Disc( const Vec2& initialDiscCenter, float initialDiscRadius );

	void SetFromText(const char* text);
};

