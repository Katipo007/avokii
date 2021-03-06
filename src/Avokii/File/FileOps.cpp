#include "FileOps.hpp"

namespace
{
	static std::streamoff StreamSize( std::istream& file )
	{
		std::istream::pos_type current_pos{ file.tellg() };

		if (current_pos == std::istream::pos_type( -1 ))
			return -1;

		file.seekg( 0, std::istream::end );
		std::istream::pos_type end_pos{ file.tellg() };
		file.seekg( current_pos );
		return end_pos - current_pos;
	}
}

namespace Avokii::FileOps
{
	bool StreamReadString( std::istream& file, String& out_file_contents )
	{
		std::streamoff len = StreamSize( file );
		if (len == -1)
			return false;

		out_file_contents.resize( static_cast<String::size_type>(len) );

		file.read( &out_file_contents[0], out_file_contents.length() );
		return true;
	}

	bool ReadFile( const Filepath& filename, String& file_contents )
	{
		std::ifstream file( filename, std::ios::binary );

		if (file.is_open())
			return StreamReadString( file, file_contents );

		return false;
	}

	std::vector<String> GetFilesInFolder( const Filepath& path )
	{
		std::vector<String> filenames{};
		for (const auto& entry : std::filesystem::directory_iterator( path ))
			filenames.emplace_back( std::move( entry.path().string() ) );

		return filenames;
	}

	Filepath GetFileDirectory( const Filepath& filepath, bool relative_to_working_directory )
	{
		const auto working_directory{ std::filesystem::current_path() };
		if( relative_to_working_directory )
		{
			Filepath path{ filepath };
			if( path.has_filename() )
				path = path.parent_path();

			const auto final_path = ( working_directory / path ).lexically_normal();

			const auto[ root_end, discard_ ] = std::mismatch( working_directory.begin(), working_directory.end(), final_path.begin() );

			if( root_end != working_directory.end() )
				throw std::runtime_error( "Given path is not relative to the working directory" );
			
			return std::filesystem::relative( final_path, working_directory );
		}
		else
		{
			const Filepath path{ filepath };
			return path.has_filename() ? path.parent_path() : path;
		}
	}
}
