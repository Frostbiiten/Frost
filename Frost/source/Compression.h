#pragma once
#include <string>

namespace fl
{
	namespace Compression
	{
		void Compress(const std::string& input, std::string& output, int compressionLevel = 22);
		void Decompress(const std::string& input, std::string& output);
		void Test();
	}
}
