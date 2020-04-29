#pragma once
#include "Engine/Job/Jobs.hpp"

class Texture2D;
class Image;

class MakeImageFromTextureJob : public Job
{

public:

	MakeImageFromTextureJob(Texture2D* texture_, Image* image_);
	virtual ~MakeImageFromTextureJob();

	virtual void Execute() override;

	Texture2D* texture = nullptr;
	Image* image = nullptr;


};