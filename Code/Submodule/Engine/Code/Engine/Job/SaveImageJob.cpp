#include "Engine/Job/SaveImageJob.hpp"




SaveImageJob::SaveImageJob(Image* image_, std::string path_)
{
	m_image = image_;
	m_path = path_;
}

SaveImageJob::~SaveImageJob()
{
	delete m_image;
	m_image = nullptr;
}

void SaveImageJob::Execute()
{
	m_image->SaveImageToDisc(m_path.c_str());
}
