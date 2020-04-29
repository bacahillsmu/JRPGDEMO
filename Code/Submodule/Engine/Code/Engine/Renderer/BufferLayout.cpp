#include "Engine/Renderer/BufferLayout.hpp"

std::map<std::string, BufferLayout*> BufferLayout::s_bufferLayouts;


BufferAttribute_t::BufferAttribute_t( std::string name, DataType dataType, size_t memberOffset )
	: name(name)
	, dataType(dataType)
	, memberOffset(memberOffset)
{
}

BufferLayout::BufferLayout( const BufferAttribute_t* attributes, size_t stride, CopyFromMasterCallback copyCallback )
{
	int counter = 0;
	while(!attributes[counter].IsNull())
	{
		m_attributes.push_back(attributes[counter]);
		counter++;
	}


	m_stride = (unsigned int)stride;
	m_copyFromMaster = copyCallback;
}

BufferLayout::~BufferLayout()
{

}

STATIC const BufferLayout* BufferLayout::For( const BufferAttribute_t* attributeList, size_t stride, CopyFromMasterCallback copyCallback )
{
	// Make the key;
	std::string keyName = "";
	int counter = 0;
	while(!attributeList[counter].IsNull())
	{
		keyName += attributeList[counter].name;
		counter++;
	}

	// Does the key exists in our map?
	std::map<std::string, BufferLayout*>::const_iterator mapPair = s_bufferLayouts.find(keyName);
	if(mapPair == s_bufferLayouts.end())
	{
		// If not, create a buffer layout;
		BufferLayout* bufferLayout = new BufferLayout( attributeList, stride, copyCallback );
		s_bufferLayouts[keyName] = bufferLayout;
	}

	return s_bufferLayouts[keyName];
}
