#pragma once
#include "Engine/Renderer/RendererTypes.hpp"

#include <map>

struct VertexMaster;

typedef void (*CopyFromMasterCallback)( void* dst, const VertexMaster* src, unsigned int count );


struct BufferAttribute_t
{
	BufferAttribute_t() 
		: name ("NOTHING")
		, dataType (DATA_TYPE_NULL)
		, memberOffset (0) 
	{}
	BufferAttribute_t(std::string name, DataType dataType, size_t memberOffset);

	std::string name;
	DataType dataType;
	size_t memberOffset;

	inline bool IsNull() const { return dataType == DATA_TYPE_NULL; }
};

// Entire point of this class is to fully describe the input into
// the shader pipeline;
class BufferLayout
{

public:

	BufferLayout( const BufferAttribute_t* list, size_t stride, CopyFromMasterCallback copyCallback );
	~BufferLayout();

	inline unsigned int GetAttributeCount() const { return (unsigned int)m_attributes.size(); }
	inline unsigned int GetStride() const { return m_stride; }


public:

	std::vector<BufferAttribute_t> m_attributes;
	unsigned int m_stride;
	CopyFromMasterCallback m_copyFromMaster;


public:

	// Static ways to look for a unique layout pointer based on an attribute list
	static const BufferLayout* For(const BufferAttribute_t* attributeList, size_t stride, CopyFromMasterCallback copyCallback);

	template <typename T>
	static const BufferLayout* For() { return For(T::LAYOUT, sizeof(T), T::CopyFromMaster); }

	static std::map<std::string, BufferLayout*> s_bufferLayouts;
};


