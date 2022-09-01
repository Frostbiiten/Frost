#pragma once
#include <string>
#include <vector>
#include <zstd.h>

namespace fl
{
	namespace Compression
	{
		void Init();
		void Compress(const std::string& input, std::string& output, int compressionLevel = 22);
		void Decompress(const std::string& input, std::string& output);

		class Trainer
		{
			// Dictionary data
			std::string dictionaryBuffer;

			// Dictionaries
			ZSTD_CDict* cDict;
			ZSTD_DDict* dDict;

			// Sample data
			std::vector<std::size_t> sampleSizes;
			std::string sampleBuffer;

		public:

			// Constructor
			Trainer() : cDict(NULL), dDict(NULL) {};

			void Sample(const std::string& sample);
			void Train(std::size_t dictionaryBufferSize);

			std::size_t GetSampleBufferSize();
			std::string GetDictionaryBuffer();

			void CreateCDict(int compressionLevel = 22);
			void CreateDDict();

			ZSTD_CDict* GetCDict();
			ZSTD_DDict* GetDDict();
		};

		void Test();
	}
}
