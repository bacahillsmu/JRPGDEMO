#include "Engine/Buffer/BufferUtilities.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"



// ------------------------------------------------------------------
// Buffer Parser;
// ------------------------------------------------------------------
BufferParser::BufferParser(const unsigned char* bufferData_, size_t bufferSize_, BufferEndian endianMode_ /*= BufferEndian::NATIVE*/)
	: m_scanStart(bufferData_)
	, m_scanPosition(bufferData_)
	, m_scanEnd(bufferData_ + bufferSize_ - 1)
	, m_bufferSize(bufferSize_)
{
	SetEndianMode(endianMode_);
}

// ------------------------------------------------------------------
BufferParser::BufferParser(const Buffer& buffer_, BufferEndian endianMode_ /*= BufferEndian::NATIVE*/)
	: BufferParser(buffer_.data(), buffer_.size(), endianMode_)
{
}

// ------------------------------------------------------------------
BufferParser::~BufferParser()
{

}

void BufferParser::SetEndianMode(BufferEndian endianModeToStartUsing_)
{
	m_endianMode = endianModeToStartUsing_;

	bool isBufferBigEndian = IsCurrentlyBigEndian();
	bool isNativeBigEndian = NATIVE_IS_BIG_ENDIAN;

	m_isOppositeEndian = isBufferBigEndian != isNativeBigEndian;
}

// ------------------------------------------------------------------
bool BufferParser::IsCurrentlyBigEndian() const
{
	return (m_endianMode == BufferEndian::BIG) 
		|| (m_endianMode == BufferEndian::NATIVE && NATIVE_IS_BIG_ENDIAN);
}

// ------------------------------------------------------------------
bool BufferParser::IsEndianModeOppositeNative() const
{
	return m_isOppositeEndian;
}

// ------------------------------------------------------------------
unsigned char BufferParser::ParseByte()
{
	// We are about to attempt to read a unsigned char's worth of data, make sure that does not push use past our end;
	GUARANTEE_OR_DIE(IsBufferDataAvailable(sizeof(unsigned char)), "Not enough Buffer Data Avaible to Parse Unsigned Char.");

	// Create a ptr to our data;
	// We cast m_scanPosition to a unsigned char ptr, it is currently an unsigned char ptr...
	// ...but we promise it should be a unsgined char ptr;
	unsigned char* uc = (unsigned char*)(m_scanPosition);

	// Increment our unsigned char ptr forward by the size of a unsigned char, since we just read that much data in;
	m_scanPosition += sizeof(unsigned char);

	return *uc;
}

// ------------------------------------------------------------------
char BufferParser::ParseChar()
{
	// We are about to attempt to read a char's worth of data, make sure that does not push use past our end;
	GUARANTEE_OR_DIE(IsBufferDataAvailable(sizeof(char)), "Not enough Buffer Data Avaible to Parse Char.");

	// Create a ptr to our data;
	// We cast m_scanPosition to a char ptr, it is currently an unsigned char ptr...
	// ...but we promise it should be a char ptr;
	char* c = (char*)(m_scanPosition);

	// Increment our unsigned char ptr forward by the size of a char, since we just read that much data in;
	m_scanPosition += sizeof(char);

	return *c;
}

// ------------------------------------------------------------------
int BufferParser::ParseInt32()
{
	// We are about to attempt to read a int's worth of data, make sure that does not push use past our end;
	GUARANTEE_OR_DIE(IsBufferDataAvailable(sizeof(int)), "Not enough Buffer Data Avaible to Parse Int32.");

	// Create a ptr to our data;
	// We cast m_scanPosition to an int ptr, it is currently an unsigned char ptr...
	// ...but we promise it should be an int ptr;
	int* i = (int*)(m_scanPosition);	

	// Increment our unsigned char ptr forward by the size of a int, since we just read that much data in;
	m_scanPosition += sizeof(int);

	if (IsEndianModeOppositeNative())
	{
		Reverse4BytesInPlace(i);
	}

	return *i;
}

// ------------------------------------------------------------------
unsigned int BufferParser::ParseUInt32()
{
	// We are about to attempt to read a uint's worth of data, make sure that does not push use past our end;
	GUARANTEE_OR_DIE(IsBufferDataAvailable(sizeof(unsigned int)), "Not enough Buffer Data Avaible to Parse UInt32.");

	// Create a ptr to our data;
	// We cast m_scanPosition to a uint ptr, it is currently an unsigned char ptr...
	// ...but we promise it should be a uint ptr;
	unsigned int* u = (unsigned int*)(m_scanPosition);

	// Increment our unsigned char ptr forward by the size of a unsigned int, since we just read that much data in;
	m_scanPosition += sizeof(unsigned int);

	if (IsEndianModeOppositeNative())
	{
		Reverse4BytesInPlace(u);
	}

	return *u;
}

// ------------------------------------------------------------------
float BufferParser::ParseFloat()
{	
	// We are about to attempt to read a float's worth of data, make sure that does not push use past our end;
	GUARANTEE_OR_DIE(IsBufferDataAvailable(sizeof(float)), "Not enough Buffer Data Avaible to Parse Float.");
	
	// Create a ptr to our data;
	// We cast m_scanPosition to a float ptr, it is currently an unsigned char ptr...
	// ...but we promise it should be a float ptr;
	float* f = (float*)(m_scanPosition);

	// Increment our unsigned char ptr forward by the size of a float, since we just read that much data in;
	m_scanPosition += sizeof(float);

	if (IsEndianModeOppositeNative())
	{
		Reverse4BytesInPlace(f);
	}

	return *f;
}

// ------------------------------------------------------------------
double BufferParser::ParseDouble()
{
	// We are about to attempt to read a double's worth of data, make sure that does not push use past our end;
	GUARANTEE_OR_DIE(IsBufferDataAvailable(sizeof(double)), "Not enough Buffer Data Avaible to Parse Double.");

	// Create a ptr to our data;
	// We cast m_scanPosition to a double ptr, it is currently an unsigned char ptr...
	// ...but we promise it should be a double ptr;
	double* d = (double*)(m_scanPosition);

	// Increment our unsigned char ptr forward by the size of a double, since we just read that much data in;
	m_scanPosition += sizeof(double);

	if (IsEndianModeOppositeNative())
	{
		Reverse8BytesInPlace(d);
	}

	return *d;
}

// ------------------------------------------------------------------
bool BufferParser::ParseBool()
{
	unsigned char b = ParseByte();

	if(b == 0x01)
	{
		return true;
	}
	else if(b == 0x00)
	{
		return false;
	}
	else
	{
		ERROR_AND_DIE("Attempted to Parse Bool, did not get expected values.");
	}
}

// ------------------------------------------------------------------
const unsigned char* BufferParser::ParseBytes(size_t numBytes)
{
	// We are about to attempt to read a double's worth of data, make sure that does not push use past our end;
	GUARANTEE_OR_DIE(IsBufferDataAvailable(numBytes), Stringf("Not enough Buffer Data Avaible to Parse Bytes of size %d.", numBytes));

	// Create a ptr to our data;
	// We cast m_scanPosition to a unsigned char ptr, it is currently an unsigned char ptr...
	// ...but we promise it should be a unsigned char ptr;
	unsigned char* c = (unsigned char*)(m_scanPosition);

	// Increment our unsigned char ptr forward by the size of a unsigned char, since we just read that much data in;
	m_scanPosition += numBytes;

	return c;
}

// ------------------------------------------------------------------
void BufferParser::ParseByteArray(std::vector<unsigned char>& out_bytes, size_t numBytesToParse)
{
	const unsigned char* c = ParseBytes(numBytesToParse);

	out_bytes.reserve(numBytesToParse);
	memcpy(&out_bytes, c, numBytesToParse);
}

// ------------------------------------------------------------------
void BufferParser::ParseByteArray(unsigned char* out_destArray, size_t numBytesToParse)
{
	out_destArray = (unsigned char*)ParseBytes(numBytesToParse);
}

// ------------------------------------------------------------------
void BufferParser::ParseStringZeroTerminated(std::string& out_string)
{
	bool foundZeroTerminated = false;
	while(!foundZeroTerminated)
	{
		char c = ParseChar();
		if(c != 0)
		{
			out_string += c;
		}
		else
		{
			foundZeroTerminated = true;
		}
	}
}

// ------------------------------------------------------------------
void BufferParser::ParseStringAfter32BitLength(std::string& out_string)
{
	unsigned int length = ParseUInt32();
	const unsigned char* c = ParseBytes(length);

	out_string = std::string((const char*)c, length);
}

// ------------------------------------------------------------------
void BufferParser::ParseStringAfter8BitLength(std::string& out_string)
{
	unsigned int length = (int)ParseByte();
	const unsigned char* c = ParseBytes(length);

	out_string = std::string((const char*)c, length);
}

// ------------------------------------------------------------------
void BufferParser::ParseStringOfLength(std::string& out_string, unsigned int stringLength)
{
	const unsigned char* c = ParseBytes(stringLength);

	out_string = std::string((const char*)c, stringLength);
}

// ------------------------------------------------------------------
Rgba BufferParser::ParseRgb()
{
	Rgba rgba;

	rgba.r = ParseFloat();
	rgba.g = ParseFloat();
	rgba.b = ParseFloat();
	rgba.a = 1.0f;

	return rgba;
}

// ------------------------------------------------------------------
Rgba BufferParser::ParseRgba()
{
	Rgba rgba;

	rgba.r = ParseFloat();
	rgba.g = ParseFloat();
	rgba.b = ParseFloat();
	rgba.a = ParseFloat();

	return rgba;
}

// ------------------------------------------------------------------
Vec2 BufferParser::ParseVec2()
{
	Vec2 vec2;

	vec2.x = ParseFloat();
	vec2.y = ParseFloat();

	return vec2;
}

// ------------------------------------------------------------------
Vec3 BufferParser::ParseVec3()
{
	Vec3 vec3;

	vec3.x = ParseFloat();
	vec3.y = ParseFloat();
	vec3.z = ParseFloat();

	return vec3;
}

// ------------------------------------------------------------------
IntVec2 BufferParser::ParseIntVec2()
{
	IntVec2 ivec2;

	ivec2.x = ParseInt32();
	ivec2.y = ParseInt32();

	return ivec2;
}

// ------------------------------------------------------------------
Vertex_PCU BufferParser::ParseVertexPCU()
{
	Vertex_PCU vertex;

	vertex.position = ParseVec3();
	vertex.color = ParseRgba();
	vertex.uvTexCoords = ParseVec2();

	return vertex;
}

// ------------------------------------------------------------------
size_t BufferParser::GetTotalSize() const
{
	return m_bufferSize;
}

// ------------------------------------------------------------------
size_t BufferParser::GetRemainingSize() const
{
	return (m_scanEnd - m_scanPosition) + 1;
}

// ------------------------------------------------------------------
bool BufferParser::IsAtEnd() const
{
	return m_scanPosition > m_scanEnd;
}

// ------------------------------------------------------------------
bool BufferParser::IsBufferDataAvailable(size_t numBytes_) const
{
	return m_scanPosition + (numBytes_ - 1) <= m_scanEnd;
}

// ------------------------------------------------------------------
void BufferParser::GuarenteeBufferDataAvailable(size_t numBytes_) const
{
	GUARANTEE_OR_DIE(m_scanPosition + (numBytes_ - 1) <= m_scanEnd, Stringf("Insufficient data in buffer to read %i bytes", numBytes_));
}

// ------------------------------------------------------------------
// Buffer Writer;
// ------------------------------------------------------------------
BufferWriter::BufferWriter(Buffer& buffer_, BufferEndian endianMode_ /*= BufferEndian::NATIVE*/)
	: m_buffer(buffer_)
{
	SetEndianMode(endianMode_);
}

// ------------------------------------------------------------------
BufferWriter::~BufferWriter()
{

}

// ------------------------------------------------------------------
bool BufferWriter::LoadBinaryFileToExistingBuffer(const std::string& filepath_, Buffer* outBuff_)
{
	long size;
	size_t result;
	FILE* file = fopen(filepath_.c_str(), "rb");
	if (file != nullptr)
	{
		// Obtain file size;
		fseek(file, 0, SEEK_END);
		size = ftell(file);
		rewind(file);

		outBuff_->resize(sizeof(char)*size);

		//char* buffer = (char*)malloc(sizeof(char)*size);

		result = fread(outBuff_->data(), 1, size, file);
		if (result != (long)size)
		{
			return false;
		}

		//memcpy(outBuff_, buffer, sizeof(char)*size);

		fclose(file);
		//free(buffer);
		return true;
	}

	return false;
}

// ------------------------------------------------------------------
bool BufferWriter::SaveBinaryFromBuffer(const std::string& filepath_, const Buffer& buff_)
{
	filepath_;
	buff_;
	return false;
}

// ------------------------------------------------------------------
void BufferWriter::SetEndianMode(BufferEndian endianModeToStartUsing_)
{
	m_endianMode = endianModeToStartUsing_;
}

// ------------------------------------------------------------------
bool BufferWriter::IsEndianModeBigEndian() const
{
	return m_endianMode == BufferEndian::BIG;
}

// ------------------------------------------------------------------
bool BufferWriter::IsEndianModeOppositeNative() const
{
	return m_isOppositeEndian;
}

// ------------------------------------------------------------------
void BufferWriter::AppendByte(unsigned char b)
{
	m_buffer.push_back(b);
}

// ------------------------------------------------------------------
void BufferWriter::AppendChar(char c)
{
	m_buffer.push_back(c);
}

// ------------------------------------------------------------------
void BufferWriter::AppendInt32(int i)
{
	unsigned char* data = (unsigned char*)(&i);

	// We are writing in Big Endian;
	if(IsEndianModeBigEndian())
	{
		// Are we Native Big Endian, if we are opposite (not Big) then we do need to convert;
		if(IsEndianModeOppositeNative())
		{			
			Reverse4BytesInPlace(data);
		}
	}
	// We are writing in Little Endian;
	else
	{
		// Are we Native Little Endian, if we are opposite (not Little) then we do need to convert;
		if (IsEndianModeOppositeNative())
		{
			Reverse4BytesInPlace(data);
		}
	}

	size_t originalOffset = m_buffer.size();
	ReserveAdditional(sizeof(int));
	memcpy(&m_buffer[originalOffset], data, sizeof(int));
}

// ------------------------------------------------------------------
void BufferWriter::AppenedUInt32(unsigned int u)
{
	unsigned char* data = (unsigned char*)(&u);

	// We are writing in Big Endian;
	if (IsEndianModeBigEndian())
	{
		// Are we Native Big Endian, if we are opposite (not Big) then we do need to convert;
		if (IsEndianModeOppositeNative())
		{
			Reverse4BytesInPlace(data);
		}
	}
	// We are writing in Little Endian;
	else
	{
		// Are we Native Little Endian, if we are opposite (not Little) then we do need to convert;
		if (IsEndianModeOppositeNative())
		{
			Reverse4BytesInPlace(data);
		}
	}

	size_t originalOffset = m_buffer.size();
	ReserveAdditional(sizeof(unsigned int));
	memcpy(&m_buffer[originalOffset], data, sizeof(unsigned int));
}

// ------------------------------------------------------------------
void BufferWriter::AppendFloat(float f)
{
	unsigned char* data = (unsigned char*)(&f);

	// We are writing in Big Endian;
	if (IsEndianModeBigEndian())
	{
		// Are we Native Big Endian, if we are opposite (not Big) then we do need to convert;
		if (IsEndianModeOppositeNative())
		{
			Reverse4BytesInPlace(data);
		}
	}
	// We are writing in Little Endian;
	else
	{
		// Are we Native Little Endian, if we are opposite (not Little) then we do need to convert;
		if (IsEndianModeOppositeNative())
		{
			Reverse4BytesInPlace(data);
		}
	}

	size_t originalOffset = m_buffer.size();
	ReserveAdditional(sizeof(float));
	memcpy(&m_buffer[originalOffset], data, sizeof(float));
}

// ------------------------------------------------------------------
void BufferWriter::AppendDouble(double d)
{
	unsigned char* data = (unsigned char*)(&d);

	if (IsEndianModeOppositeNative())
	{
		Reverse8BytesInPlace(data);
	}

	size_t originalOffset = m_buffer.size();
	ReserveAdditional(sizeof(double));
	memcpy(&m_buffer[originalOffset], data, sizeof(double));
}

// ------------------------------------------------------------------
void BufferWriter::AppendBool(bool b)
{	
	if(b)
	{
		AppendByte(0x01);
	}
	else
	{
		AppendByte(0x00);
	}
}

// ------------------------------------------------------------------
void BufferWriter::AppendByteArray(const unsigned char* byteArray, size_t numBytesToWrite)
{
	unsigned char* data = (unsigned char*)(byteArray);

	size_t originalOffset = m_buffer.size();
	ReserveAdditional(numBytesToWrite);
	memcpy(&m_buffer[originalOffset], data, numBytesToWrite);
}

// ------------------------------------------------------------------
void BufferWriter::AppendByteArray(const std::vector<unsigned char>& byteArray)
{
	GUARANTEE_OR_DIE(byteArray.size() > 0, "Tried to Append Byte Array with a vector of Size 0.");
	
	AppendByteArray(&byteArray[0], byteArray.size());
}

// ------------------------------------------------------------------
void BufferWriter::AppendStringZeroTerminated(const char* s)
{
	unsigned char* byteArray = (unsigned char*)(s);
	size_t length = strlen(s);

	AppendByteArray(byteArray, length);
	AppendChar(0);
}

// ------------------------------------------------------------------
void BufferWriter::AppendStringZeroTerminated(const std::string& s)
{
	unsigned char* byteArray = (unsigned char*)(s.c_str());
	size_t length = strlen(s.c_str());

	AppendByteArray(byteArray, length);
	AppendChar(0);
}

// ------------------------------------------------------------------
void BufferWriter::AppendStringAfter32BitLength(const char* s, unsigned int knownLength /*= 0*/)
{
	if(knownLength == 0)
	{
		knownLength = (unsigned int)strlen(s);
	}

	AppenedUInt32(knownLength);
	AppendByteArray((unsigned char*)(s), knownLength);
}

// ------------------------------------------------------------------
void BufferWriter::AppendStringAfter32BitLength(const std::string& s)
{
	AppendStringAfter32BitLength(s.c_str());
}

// ------------------------------------------------------------------
void BufferWriter::AppendStringAfter8BitLength(const char* s, unsigned char knownLength /*= 0*/)
{
	if (knownLength == 0)
	{
		knownLength = (unsigned char)strlen(s);
	}

	GUARANTEE_OR_DIE(knownLength <= 255, "Trying to Append String with 8 Bit Length and it is greater than 255 length");
	
	AppendByte(knownLength);
	unsigned char* byteArray = (unsigned char*)(s);

	AppendByteArray(byteArray, knownLength);
}

// ------------------------------------------------------------------
void BufferWriter::AppendStringAfter8BitLength(const std::string& s)
{
	AppendStringAfter8BitLength(s.c_str());
}

// ------------------------------------------------------------------
void BufferWriter::AppendRgb(const Rgba& rgb_ignoreAlpha)
{
	AppendFloat(rgb_ignoreAlpha.r);
	AppendFloat(rgb_ignoreAlpha.g);
	AppendFloat(rgb_ignoreAlpha.b);
}

// ------------------------------------------------------------------
void BufferWriter::AppendRgba(const Rgba& rgba)
{
	AppendFloat(rgba.r);
	AppendFloat(rgba.g);
	AppendFloat(rgba.b);
	AppendFloat(rgba.a);
}

// ------------------------------------------------------------------
void BufferWriter::AppendVec2(const Vec2& vec2)
{
	AppendFloat(vec2.x);
	AppendFloat(vec2.y);
}

// ------------------------------------------------------------------
void BufferWriter::AppendVec3(const Vec3& vec3)
{
	AppendFloat(vec3.x);
	AppendFloat(vec3.y);
	AppendFloat(vec3.z);
}

// ------------------------------------------------------------------
void BufferWriter::AppendIntVec2(const IntVec2& ivec2)
{
	AppendInt32(ivec2.x);
	AppendInt32(ivec2.y);
}

// ------------------------------------------------------------------
void BufferWriter::AppendVertexPCU(const Vertex_PCU& vertex)
{
	AppendVec3(vertex.position);
	AppendRgba(vertex.color);
	AppendVec2(vertex.uvTexCoords);
}

// ------------------------------------------------------------------
void BufferWriter::AppendZeros(size_t howManyBytesWorthOfZerosToAppend)
{
	for(size_t i = 0; i < howManyBytesWorthOfZerosToAppend; ++i)
	{
		AppendByte(0);
	}
}

// ------------------------------------------------------------------
unsigned char* BufferWriter::AppendUninitializedBytes(size_t howManyJunkBytesToAppend)
{
	for (size_t i = 0; i < howManyJunkBytesToAppend; ++i)
	{
		AppendByte(0);
	}

	size_t bufferSize = GetTotalSize();
	return &m_buffer[0] + bufferSize;
}

// ------------------------------------------------------------------
Buffer& BufferWriter::GetBuffer() const
{
	return m_buffer;
}

// ------------------------------------------------------------------
size_t BufferWriter::GetTotalSize() const
{
	return m_buffer.size();
}

// ------------------------------------------------------------------
size_t BufferWriter::GetAppendedSize() const
{
	return m_buffer.size() - m_initialSize;
}

// ------------------------------------------------------------------
void BufferWriter::ReserveAdditional(size_t additionalBytes_)
{
	m_buffer.resize(GetTotalSize() + additionalBytes_);
}

// ------------------------------------------------------------------
// Some basic buffer-related utility functions;
// ------------------------------------------------------------------
// For Short;
void Reverse2BytesInPlace(void* ptrTo16BitWord_)
{
	unsigned short u = *(unsigned short*)ptrTo16BitWord_;

	*(unsigned short*)ptrTo16BitWord_ = ((u & 0x00ff) << 8) 
									  | ((u & 0xff00) >> 8);
}

// ------------------------------------------------------------------
// For Int, UInt, Float;
void Reverse4BytesInPlace(void* ptrTo32BitDword_)
{
	unsigned int u = *(unsigned int*)ptrTo32BitDword_;

	*(unsigned int*)ptrTo32BitDword_ = ((u & 0x000000ff) << 24)
									 | ((u & 0x0000ff00) << 8)
									 | ((u & 0x00ff0000) >> 8)
									 | ((u & 0xff000000) >> 24);
}

// ------------------------------------------------------------------
// For Double, UInt64;
void Reverse8BytesInPlace(void* ptrTo64BitQword_)
{
	int64_t u = *(int64_t*)ptrTo64BitQword_;

	*(int64_t*)ptrTo64BitQword_ = ((u & 0x00000000000000ff) << 56)
								| ((u & 0x000000000000ff00) << 40)
								| ((u & 0x0000000000ff0000) << 24)
								| ((u & 0x00000000ff000000) << 8)
								| ((u & 0x000000ff00000000) >> 8)
								| ((u & 0x0000ff0000000000) >> 24)
								| ((u & 0x00ff000000000000) >> 40)
								| ((u & 0xff00000000000000) >> 56);
}

// ------------------------------------------------------------------
// For "pointer-sized" objects (e.g. size_t), either 4 or 8 bytes (in x86 and x64, respectively);
void ReverseBytesInPlacePtrSizeT(void* ptrToPtrOrSizeT_)
{
	if constexpr (sizeof(void*) == 8)
	{
		Reverse8BytesInPlace(ptrToPtrOrSizeT_);
	}
	else
	{
		Reverse4BytesInPlace(ptrToPtrOrSizeT_);
	}
}
