#include <Compression.h>
#include <AssetMan.h>
#include <Debug.h>
#include <zstd.h>
#include <zdict.h>
#include <SFML/Network.hpp>
#include <Instrumentor.h>

namespace fl
{
	namespace Compression
	{
		// ------------------------------------------
		// Contexts for compression and decompression
		ZSTD_CCtx* cContext;
		ZSTD_DCtx* dContext;

		void Init()
		{
			cContext = ZSTD_createCCtx();
			dContext = ZSTD_createDCtx();

			if (cContext == NULL || dContext == NULL)
			{
				std::string errorText = "ZSTD ERROR! Failed to initialize contexts!";
				std::exception except(errorText.c_str());
				Debug::log()->error(errorText);
				throw except;
			}
		}
		void CheckContexts()
		{
			if (cContext == NULL || dContext == NULL)
			{
				Debug::log()->info("Initializing contexts because they are not initialized already...");
				Init();
			}
		}

		// ------------------------------------------
		// Error checking

		void ErrorCheck(const std::size_t code)
		{
			if (ZSTD_isError(code))
			{
				std::string errorText = fmt::format("ZSTD ERROR! CODE {0}: '{1}'", code, ZSTD_getErrorName(code));
				std::exception except(errorText.c_str(), code);
				Debug::log()->error(errorText);
				throw except;
			}
		}

		void VerifyFrameSize(unsigned long long frameSize)
		{
			if (frameSize == ZSTD_CONTENTSIZE_ERROR)
			{
				std::invalid_argument except("ZSTD ERROR: File does not use zstd compression!");
				Debug::log()->error(except.what());
				throw except;
			}
			else if (frameSize == ZSTD_CONTENTSIZE_UNKNOWN)
			{
				std::invalid_argument except("ZSTD ERROR: Cannot determine original size!");
				Debug::log()->error(except.what());
				throw except;
			}
		}

		void DictErrorCheck(const std::size_t code)
		{
			if (ZDICT_isError(code))
			{
				std::string errorText = fmt::format("ZDICT ERROR! CODE {0}: '{1}'", code, ZDICT_getErrorName(code));
				std::exception except(errorText.c_str(), code);
				Debug::log()->error(errorText);
				throw except;
			}
		}

		// ------------------------------------------
		// Compression Training

		void Trainer::Sample(const std::string& sample)
		{
			sampleBuffer += sample;
			sampleSizes.push_back(sample.size());
		}

		void Trainer::Train(std::size_t dictionaryBufferSize)
		{
			dictionaryBuffer.resize(dictionaryBufferSize);
			std::size_t newSize = ZDICT_trainFromBuffer(dictionaryBuffer.data(), dictionaryBufferSize, sampleBuffer.data(), &*sampleSizes.begin(), sampleSizes.size());

			DictErrorCheck(newSize);

			dictionaryBuffer.resize(newSize);
			dictionaryBuffer.shrink_to_fit();
		}

		std::string Trainer::GetDictionaryBuffer()
		{
			return dictionaryBuffer;
		}

		std::size_t Trainer::GetSampleBufferSize()
		{
			return sampleBuffer.size();
		}

		void Trainer::CreateCDict(int compressionLevel)
		{
			cDict = ZSTD_createCDict(dictionaryBuffer.data(), dictionaryBuffer.size(), compressionLevel);
			if (cDict == NULL)
			{
				std::string errorText = "ZDICT ERROR! Failed to create cDictionary!";
				std::exception except(errorText.c_str());
				Debug::log()->error(errorText);
				throw except;
			}
		}

		void Trainer::CreateDDict()
		{
			dDict = ZSTD_createDDict(dictionaryBuffer.data(), dictionaryBuffer.size());
			if (dDict == NULL)
			{
				std::string errorText = "ZDICT ERROR! Failed to create dDictionary!";
				std::exception except(errorText.c_str());
				Debug::log()->error(errorText);
				throw except;
			}
		}

		ZSTD_CDict* Trainer::GetCDict()
		{
			return cDict;
		}

		ZSTD_DDict* Trainer::GetDDict()
		{
			return dDict;
		}

		// ------------------------------------------
		// Compress/Decompress

		void Compress (const std::string& input, std::string& output, int compressionLevel)
		{
			CheckContexts();

			// Check if compression level is within valid range
			if (compressionLevel > ZSTD_maxCLevel() || compressionLevel < ZSTD_minCLevel())
			{
				std::string errorText = fmt::format("Compression level '{0}' exceeds maximum compression level of {1}!", compressionLevel, ZSTD_maxCLevel());
				std::invalid_argument except(errorText);
				Debug::log()->error(errorText);
				throw except;
			}

			std::size_t maxSize = ZSTD_compressBound(input.size()); // Get maximum compressed file size
			output.resize(maxSize); // Resize output accordingly

			// Compress & resize output
			std::size_t newSize = ZSTD_compressCCtx(cContext, output.data(), maxSize, input.c_str(), input.size(), compressionLevel);
			ErrorCheck(newSize);

			output.resize(newSize);
			output.shrink_to_fit();
		}

		void Decompress (const std::string& input, std::string& output)
		{
			// Decompression Frame
			unsigned long long const frameSize = ZSTD_getFrameContentSize(input.c_str(), input.size());
			VerifyFrameSize(frameSize);

			// Return if empty
			if (frameSize == 0)
			{
				std::string errorText = "Empty Frame!";
				std::exception except(errorText.c_str());
				Debug::log()->error(errorText);
				throw except;
			}

			// Resize output for decompressed data
			output.resize((std::size_t)frameSize);

			// Decompress & error check
			std::size_t const newSize = ZSTD_decompress(output.data(), frameSize, input.c_str(), input.size());
			ErrorCheck(newSize);

			// Resize output
			output.resize(newSize);
		}

		void DictCompress(const std::string& input, std::string& output, const ZSTD_CDict* cDict)
		{
			CheckContexts();

			std::size_t maxSize = ZSTD_compressBound(input.size());
			output.resize(maxSize); // Resize output accordingly

			// Compress & resize output
			std::size_t newSize = ZSTD_compress_usingCDict(cContext, output.data(), maxSize, input.c_str(), input.size(), cDict);
			ErrorCheck(newSize);

			output.resize(newSize);
			output.shrink_to_fit();
		}

		void DictDecompress(const std::string& input, std::string& output, const ZSTD_DDict* dDict)
		{
			CheckContexts();

			// Decompression Frame
			unsigned long long const frameSize = ZSTD_getFrameContentSize(input.c_str(), input.size());
			VerifyFrameSize(frameSize);

			// Verify dictionary
			if (ZSTD_getDictID_fromDDict(dDict) != ZSTD_getDictID_fromFrame(input.c_str(), input.size()))
			{
				std::string errorText = "Dictionary ID mismatch!";
				std::exception except(errorText.c_str());
				Debug::log()->error(errorText);
				throw except;
			}

			// Return if empty
			if (frameSize == 0)
			{
				std::string errorText = "Empty Frame!";
				std::exception except(errorText.c_str());
				Debug::log()->error(errorText);
				throw except;
			}

			// Resize output for decompressed data
			output.resize((std::size_t)frameSize);

			// Decompress & error check
			std::size_t newSize = ZSTD_decompress_usingDDict(dContext, output.data(), frameSize, input.c_str(), input.size(), dDict);
			ErrorCheck(newSize);

			// Resize output again
			output.resize(newSize);
		}

		void Test()
		{
			Init();

			std::string a;
			AssetMan::readFile("common/lorem_old.txt", a);
			Debug::log()->info("original ({} bytes):", a.size(), a);

			std::string b;

			{
				PROFILE_SCOPE("Normal Compress");
				Compress(a, b);
			}

			Debug::log()->info("compressed ({} bytes):", b.size(), b);

			std::size_t oldSize = b.size();

			std::string c;

			{
				PROFILE_SCOPE("Normal Decompress");
				Decompress(b, c);
			}

			Debug::log()->info("decompressed ({} bytes):\n", c.size(), c);

			assert(a == c);

			/*
			Trainer trainer;

			sf::Http http("http://asdfast.beobit.net");
			sf::Http::Request request("/api/", sf::Http::Request::Get);
			request.setField("type", "paragraph");
			request.setField("length", "10");

			Debug::log()->info("Requesting...");
			for (int i = 0; i < 200; ++i)
			{
				Debug::log()->info(i);

				sf::Http::Response response = http.sendRequest(request);
				auto status = response.getStatus();
				if (status == sf::Http::Response::Ok)
				{
					std::string data = response.getBody();
					trainer.Sample(data);
				}
			}

			Debug::log()->info("Training...");
			trainer.Train(trainer.GetSampleBufferSize() / 20);
			Debug::log()->info("Complete!");
			AssetMan::writeFile("common/dict3.txt", trainer.GetDictionaryBuffer());
			Debug::log()->info("Saved!");

			auto cDict = trainer.GetCDict();
			auto dDict = trainer.GetDDict();
			*/

			std::string dict;
			AssetMan::readFile("compression_dictionaries/lorem.txt", dict);
			ZSTD_CDict* cDict = ZSTD_createCDict(dict.c_str(), dict.size(), 22);
			ZSTD_DDict* dDict = ZSTD_createDDict(dict.c_str(), dict.size());

			Debug::log()->info("original ({} bytes)", a.size(), a);

			b.clear();

			{
				PROFILE_SCOPE("Dictionary Compress");
				DictCompress(a, b, cDict);
			}

			Debug::log()->info("compressed ({} bytes):", b.size(), b);

			c.clear();

			{
				PROFILE_SCOPE("Dictionary Decompress");
				DictDecompress(b, c, dDict);
			}

			Debug::log()->info("decompressed ({} bytes):", c.size(), c);

			Debug::log()->info("Dictionary trimmed {}% off normal compression!", (1 - ((float)b.size() / oldSize)) * 100);
		}
	}
}
