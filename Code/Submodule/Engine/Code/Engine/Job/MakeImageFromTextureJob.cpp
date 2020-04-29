#include "Engine/Job/MakeImageFromTextureJob.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/Image.hpp"

MakeImageFromTextureJob::MakeImageFromTextureJob(Texture2D* texture_, Image* image_)
{
	texture = texture_;
	image = image_;
	m_jobCategory = JOBCATEGORY_RENDER;
}

MakeImageFromTextureJob::~MakeImageFromTextureJob()
{
	delete texture;
	texture = nullptr;
}

void MakeImageFromTextureJob::Execute()
{
	texture->MakeImage(*image);
}

