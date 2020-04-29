#pragma once
#include "Engine/Job/Jobs.hpp"
#include "Engine/Core/Image.hpp"

class Image;

class SaveImageJob : public Job
{

public:

	SaveImageJob(Image* image_, std::string path_);
	virtual ~SaveImageJob();

	virtual void Execute() override;


	std::string m_path;
	Image* m_image;

};