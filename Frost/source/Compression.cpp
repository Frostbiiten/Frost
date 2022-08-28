#include <Compression.h>
#include <AssetMan.h>
#include <Debug.h>
#include <zstd.h>

namespace fl
{
	namespace Compression
	{
		void ErrorCheck(const int code)
		{
			if (ZSTD_isError(code))
			{
				std::string errorText = fmt::format("ZSTD ERROR! CODE {0}: '{1}'", code, ZSTD_getErrorName(code));
				std::exception except(errorText.c_str(), code);
				Debug::log()->error(errorText);
				throw except;
			}
		}

		void Compress (const std::string& input, std::string& output, int compressionLevel)
		{
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
			std::size_t newSize = ZSTD_compress(output.data(), maxSize, input.c_str(), input.size(), compressionLevel);
			output.resize(newSize);

			// Check if compression returned an error
			ErrorCheck(newSize);
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

		void Decompress (const std::string& input, std::string& output)
		{
			unsigned long long const frameSize = ZSTD_getFrameContentSize(input.c_str(), input.size());
			VerifyFrameSize(frameSize); // Verify the framesize

			// Return if empty
			if (frameSize == 0) return;

			// Resize output for decompressed data
			output.resize((std::size_t)frameSize);

			// Decompress & error check
			std::size_t const newSize = ZSTD_decompress(output.data(), frameSize, input.c_str(), input.size());
			ErrorCheck(newSize);

			// Resize output
			output.resize(newSize);
		}

		void Test()
		{
			std::string a = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras neque erat, eleifend sed sapien vel, consequat molestie quam. Vestibulum vel sapien a ipsum bibendum accumsan imperdiet eu nibh. Praesent euismod porttitor gravida. Interdum et malesuada fames ac ante ipsum primis in faucibus. Praesent ex ex, viverra ac ante vitae, tincidunt tristique dui. Nam nec enim non elit bibendum pellentesque. Mauris enim velit, semper in mauris eu, semper finibus justo. Proin accumsan nunc nibh, vitae maximus lorem accumsan vitae.";
			Debug::log()->info("original ({} bytes): {}\n", a.size(), a);

			std::string b;
			Compress(a, b);
			Debug::log()->info("compressed ({} bytes): {}\n", b.size(), b);

			std::string c;
			Decompress(b, c);
			Debug::log()->info("decompressed ({} bytes): {}\n", c.size(), c);

			assert(a == c);
		}
	}
}
