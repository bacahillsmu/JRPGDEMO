#pragma once

#include <string>

unsigned int CRC32 (const std::string& buf, unsigned int init = 0xffffffff);
unsigned int CRC32 (const void* buf, int len, unsigned int init = 0xffffffff);

template <typename T>
unsigned int CRC32( T const& obj, unsigned int init = ~0 ) 
{ 
	return CRC32( &obj, sizeof(T), init ); 
}