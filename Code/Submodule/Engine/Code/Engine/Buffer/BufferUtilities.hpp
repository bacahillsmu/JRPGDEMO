#pragma once

#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

#include <string.h>
#include <vector>

struct Rgba;
struct Vec2;
struct Vec3;
struct IntVec2;
struct Vertex_PCU;

typedef std::vector<unsigned char> Buffer;

enum class BufferEndian
{
	NATIVE,
	LITTLE,
	BIG
};



// Some basic buffer-related utility functions
void Reverse2BytesInPlace(void* ptrTo16BitWord_); // for short
void Reverse4BytesInPlace(void* ptrTo32BitDword_); // for int, uint, float
void Reverse8BytesInPlace(void* ptrTo64BitQword_); // for double, uint64
void ReverseBytesInPlacePtrSizeT(void* ptrToPtrOrSizeT_); // For "pointer-sized" objects (e.g. size_t), either 4 or 8 bytes (in x86 and x64, respectively)

class BufferParser
{

public:

	BufferParser(const unsigned char* bufferData_, size_t bufferSize_, BufferEndian endianMode_ = BufferEndian::NATIVE);
	BufferParser(const Buffer& buffer_, BufferEndian endianMode_ = BufferEndian::NATIVE);
	~BufferParser();

	// Endian;
	void SetEndianMode(BufferEndian endianModeToStartUsing_);
	bool IsCurrentlyBigEndian() const;
	bool IsEndianModeOppositeNative() const;

	// Parsing;
	unsigned char ParseByte();
	char ParseChar();
	int ParseInt32();
	unsigned int ParseUInt32();
	float ParseFloat();
	double ParseDouble();
	bool ParseBool();
	const unsigned char* ParseBytes(size_t numBytes);
	void ParseByteArray(std::vector<unsigned char>& out_bytes, size_t numBytesToParse);
	void ParseByteArray(unsigned char* out_destArray, size_t numBytesToParse);
	void ParseStringZeroTerminated(std::string& out_string);
	void ParseStringAfter32BitLength(std::string& out_string);
	void ParseStringAfter8BitLength(std::string& out_string);
	void ParseStringOfLength(std::string& out_string, unsigned int stringLength);
	Rgba ParseRgb();
	Rgba ParseRgba();
	Vec2 ParseVec2();
	Vec3 ParseVec3();
	IntVec2 ParseIntVec2();
	Vertex_PCU ParseVertexPCU();

	// Helpers;
	size_t GetTotalSize() const;
	size_t GetRemainingSize() const;
	bool IsAtEnd() const;
	bool IsBufferDataAvailable(size_t numBytes_) const;
	void GuarenteeBufferDataAvailable(size_t numBytes_) const;


private:

	BufferEndian m_endianMode = BufferEndian::NATIVE;
	bool m_isOppositeEndian = false;
	size_t m_bufferSize = 0;

	// Start of the data being passed
	const unsigned char* m_scanStart	= nullptr;
	const unsigned char* m_scanPosition = nullptr;
	const unsigned char* m_scanEnd		= nullptr;

};

class BufferWriter
{

public:

	BufferWriter(Buffer& buffer_, BufferEndian endianMode_ = BufferEndian::NATIVE);
	~BufferWriter();

	static bool LoadBinaryFileToExistingBuffer(const std::string& filepath_, Buffer* outBuff_);
	static bool SaveBinaryFromBuffer(const std::string& filepath_, const Buffer& buff_);

	// Endian;
	void SetEndianMode(BufferEndian endianModeToStartUsing_);
	bool IsEndianModeBigEndian() const;
	bool IsEndianModeOppositeNative() const;

	// Appending;
	void AppendByte(unsigned char b);
	void AppendChar(char c);
	void AppendInt32(int i);
	void AppenedUInt32(unsigned int u);
	void AppendFloat(float f);
	void AppendDouble(double d);
	void AppendBool(bool b);
	void AppendByteArray(const unsigned char* byteArray, size_t numBytesToWrite);
	void AppendByteArray(const std::vector<unsigned char>& byteArray);
	void AppendStringZeroTerminated(const char* s);
	void AppendStringZeroTerminated(const std::string& s);
	void AppendStringAfter32BitLength(const char* s, unsigned int knownLength = 0);
	void AppendStringAfter32BitLength(const std::string& s);
	void AppendStringAfter8BitLength(const char* s, unsigned char knownLength = 0);
	void AppendStringAfter8BitLength(const std::string& s);
	void AppendRgb(const Rgba& rgb_ignoreAlpha);
	void AppendRgba(const Rgba& rgba);
	void AppendVec2(const Vec2& vec2);
	void AppendVec3(const Vec3& vec3);
	void AppendIntVec2(const IntVec2& ivec2);
	void AppendVertexPCU(const Vertex_PCU& vertex);
	void AppendZeros(size_t howManyBytesWorthOfZerosToAppend);
	unsigned char* AppendUninitializedBytes(size_t howManyJunkBytesToAppend);

	// Helpers;
	Buffer& GetBuffer() const;
	size_t GetTotalSize() const;
	size_t GetAppendedSize() const;
	void ReserveAdditional(size_t additionalBytes_);


private:

	BufferEndian m_endianMode = BufferEndian::NATIVE;
	bool m_isOppositeEndian = false;
	Buffer& m_buffer;
	size_t m_initialSize = 0; // A starting offset;

};