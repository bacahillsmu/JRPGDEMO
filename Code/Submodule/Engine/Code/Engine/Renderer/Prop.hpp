#pragma once
#include "Engine/Math/Vec3.hpp"

class Model;
class RenderContext;


class Prop
{

public:

	Prop(RenderContext* renderContext);
	Prop(const Prop& copyProp);
	~Prop();

	void Update(float deltaSeconds);
	void Load(const char* filename);
	void Render();


public:

	RenderContext* m_theRenderContext = nullptr;
	Model* m_model = nullptr;

};