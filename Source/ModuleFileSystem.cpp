#include "ModuleFileSystem.h"
#include "Globals.h"
#include "Application.h"

#include "physfs.h"
#include <fstream>
#include <filesystem>

#include "cfileio.h"
#include "types.h"

ModuleFileSystem::ModuleFileSystem(Application* app, bool startEnabled) : Module(app, startEnabled)
{
	// needs to be created before Init so other modules can use it
	char* basePath = SDL_GetBasePath();
	systemBasePath = std::string(basePath);
	PHYSFS_init(nullptr);
	SDL_free(basePath);

	AddPath(".");
	AddPath("./Assets");
	AddPath("./Assets/Scenes");
	scenePath = "Assets/Scenes";
	AddPath("./Assets/Resources");
	AddPath("./Assets/Resources/Textures");
	texturePath = "Assets/Resources/Textures";
	AddPath("./Assets/Resources/Models");
	meshPath = "Assets/Resources/Models";
}

// Destructor
ModuleFileSystem::~ModuleFileSystem()
{}

// Called before render is available
bool ModuleFileSystem::Init()
{
	LOG_CONSOLE("Loading File System");
	bool ret = true;

	//Setting the working directory as the writing directory
	if (PHYSFS_setWriteDir(".") == 0) LOG_CONSOLE("File System error while creating write dir: %s\n", PHYSFS_getLastError());
	if (PHYSFS_init(nullptr) == 0) LOG_CONSOLE("PhysFS succesfully loaded | Libs initialized");

	CreateLibraryDirectories();
	return ret;
}

// Called before quitting
bool ModuleFileSystem::CleanUp()
{
	LOG_CONSOLE("Freeing File System subsystem");

	PHYSFS_deinit();

	systemBasePath.clear();
	texturePath.clear();
	meshPath.clear();
	scenePath.clear();

	return true;
}

bool ModuleFileSystem::Read(const std::string& path, void* data, unsigned size) const
{
	PHYSFS_File* file = PHYSFS_openRead(path.c_str());
	PHYSFS_ErrorCode errorCode = PHYSFS_getLastErrorCode();
	if (errorCode == PHYSFS_ERR_BAD_FILENAME) //possibly it's from outside the filesystem -> read as C
	{
		LOG_CONSOLE("Reading outside filesystem.");
		FILE* file = nullptr;
		fopen_s(&file, path.c_str(), "rb");
		if (file == nullptr)
		{
			LOG_CONSOLE("Impossible to read %s", path.c_str());
			return 0;
		}
		fread_s(data, size, 1, size, file);
		fclose(file);
		return true;
	}
	else
	{
		if (file == nullptr)
		{
			LOG_CONSOLE("Error reading %s -> %s", path.c_str(), PHYSFS_getErrorByCode(errorCode));
			return false;
		}

		if (PHYSFS_readBytes(file, data, size) == size)
		{
			PHYSFS_close(file);
			return true;
		}
		PHYSFS_close(file);
		return false;
	}
}

bool ModuleFileSystem::Exists(const std::string& path) const
{
	return PHYSFS_exists(path.c_str()) != 0;
}

unsigned ModuleFileSystem::Size(const std::string& path) const
{
	PHYSFS_File* file = PHYSFS_openRead(path.c_str());
	PHYSFS_ErrorCode errorCode = PHYSFS_getLastErrorCode();
	if (errorCode == PHYSFS_ERR_BAD_FILENAME) //possibly it's from outside the filesystem -> read as C
	{
		LOG_CONSOLE("Reading outside filesystem.");
		FILE* file = nullptr;
		fopen_s(&file, path.c_str(), "rb");
		if (file == nullptr)
		{
			LOG_CONSOLE("Impossible to read %s", path.c_str());
			return 0;
		}
		fseek(file, 0L, SEEK_END);
		return ftell(file);
	}
	if (file == nullptr)
	{
		LOG_CONSOLE("Error reading %s -> %s", path.c_str(), PHYSFS_getErrorByCode(errorCode));
		return 0;
	}
	return PHYSFS_fileLength(file);
}

void ModuleFileSystem::CreateLibraryDirectories()
{
	std::string path = ASSETS_FOLDER;
	path.append(MODELS_FOLDER);
	CreateDir(path.c_str());

	path.clear();
	path = ASSETS_FOLDER;
	path.append(TEXTURES_FOLDER);
	CreateDir(path.c_str());

	path.clear();
	path = SCENES_FOLDER;
	CreateDir(path.c_str());
}

// Add a new zip file or folder
bool ModuleFileSystem::AddPath(const char* pathOrZip)
{
	bool ret = false;

	if (PHYSFS_mount(pathOrZip, nullptr, 1) == 0) LOG_CONSOLE("File System error while adding a path or zip: %s\n", PHYSFS_getLastError())
	else ret = true;

	return ret;
}

// Check if a file exists
bool ModuleFileSystem::Exists(const char* file) const
{
	return PHYSFS_exists(file) != 0;
}

bool ModuleFileSystem::CreateDir(const char* dir)
{
	if (IsDirectory(dir) == false)
	{
		PHYSFS_mkdir(dir);
		AddPath(dir);
		return true;
	}
	return false;
}

// Check if a file is a directory
bool ModuleFileSystem::IsDirectory(const char* file) const
{
	return PHYSFS_isDirectory(file) != 0;
}

const char* ModuleFileSystem::GetWriteDir() const
{
	return PHYSFS_getWriteDir();
}

void ModuleFileSystem::DiscoverFiles(const char* directory, std::vector<std::string>& fileList, std::vector<std::string>& dirList) const
{
	char** rc = PHYSFS_enumerateFiles(directory);
	char** i;

	for (i = rc; *i != nullptr; i++)
	{
		std::string str = std::string(directory) + std::string("/") + std::string(*i);
		if (IsDirectory(str.c_str()))
		{
			dirList.push_back(*i);
		}
		else
		{
			fileList.push_back(*i);
		}
	}

	PHYSFS_freeList(rc);
}

void ModuleFileSystem::GetAllFilesWithExtension(const char* directory, const char* extension, std::vector<std::string>& fileList) const
{
	std::vector<std::string> files;
	std::vector<std::string> dirs;
	DiscoverFiles(directory, files, dirs);

	for (uint i = 0; i < files.size(); i++)
	{
		std::string ext;
		SplitFilePath(files[i].c_str(), nullptr, nullptr, &ext);

		if (ext == extension) fileList.push_back(files[i]);
	}
}

void ModuleFileSystem::GetRealDir(std::string path, std::string& output) const
{
	uint i = 0;

	while (i < path.length() && i < systemBasePath.length() && path[i] == systemBasePath[i])
	{
		i++;
	}

	if (i < path.length()) output = path.substr(i, path.length() - i);
}

std::string ModuleFileSystem::GetPathRelativeToAssets(const char* originalPath) const
{
	std::string ret;
	GetRealDir(originalPath, ret);
	return ret;
}

bool ModuleFileSystem::HasExtension(const char* path) const
{
	std::string ext = "";
	SplitFilePath(path, nullptr, nullptr, &ext);
	return ext != "";
}

bool ModuleFileSystem::HasExtension(const char* path, std::string extension) const
{
	std::string ext = "";
	SplitFilePath(path, nullptr, nullptr, &ext);
	return ext == extension;
}

bool ModuleFileSystem::HasExtension(const char* path, std::vector<std::string> extensions) const
{
	std::string ext = "";
	SplitFilePath(path, nullptr, nullptr, &ext);
	if (ext == "") return true;
	for (uint i = 0; i < extensions.size(); i++)
	{
		if (extensions[i] == ext) return true;
	}
	return false;
}

std::string ModuleFileSystem::NormalizePath(const char* fullPath) const
{
	std::string newPath(fullPath);
	for (int i = 0; i < newPath.size(); ++i)
	{
		if (newPath[i] == '\\') newPath[i] = '/';
	}
	return newPath;
}

void ModuleFileSystem::SplitFilePath(const char* fullPath, std::string* path, std::string* file, std::string* extension) const
{
	if (fullPath != nullptr)
	{
		std::string full(fullPath);
		size_t posSeparator = full.find_last_of("\\/");
		size_t posDot = full.find_last_of(".");

		if (path != nullptr)
		{
			if (posSeparator < full.length())
			{
				*path = full.substr(0, posSeparator + 1);
			}
			else
			{
				path->clear();
			}
		}

		if (file != nullptr)
		{
			if (posSeparator < full.length())
			{
				*file = full.substr(posSeparator + 1, posDot - posSeparator - 1);
			}
			else
			{
				*file = full.substr(0, posDot);
			}
		}

		if (extension != nullptr)
		{
			if (posDot < full.length())
			{
				*extension = full.substr(posDot + 1);
			}
			else
			{
				extension->clear();
			}
		}
	}
}

unsigned int ModuleFileSystem::Load(const char* path, const char* file, char** buffer) const
{
	std::string fullPath(path);
	fullPath += file;
	return Load(fullPath.c_str(), buffer);
}

// Read a whole file and put it in a new buffer
uint ModuleFileSystem::Load(const char* file, char** buffer) const
{
	uint ret = 0;

	PHYSFS_file* fsFile = PHYSFS_openRead(file);

	if (fsFile != nullptr)
	{
		PHYSFS_sint32 size = (PHYSFS_sint32)PHYSFS_fileLength(fsFile);

		if (size > 0)
		{
			*buffer = new char[size + 1];
			uint read = (uint)PHYSFS_read(fsFile, *buffer, 1, size);
			if (read != size)
			{
				LOG_CONSOLE("File System error while reading from file %s: %s\n", file, PHYSFS_getLastError());
				RELEASE_ARRAY(buffer);
			}
			else
			{
				ret = read;
				//Adding end of file at the end of the buffer. Loading a shader file does not add this for some reason
				(*buffer)[size] = '\0';
			}
		}

		if (PHYSFS_close(fsFile) == 0) LOG_CONSOLE("File System error while closing file %s: %s\n", file, PHYSFS_getLastError());
	}
	else
	{
		LOG_CONSOLE("File System error while opening file %s: %s\n", file, PHYSFS_getLastError());
	}

	return ret;
}

bool ModuleFileSystem::DuplicateFile(const char* file, const char* dstFolder, std::string& relativePath)
{
	std::string fileStr, extensionStr;
	SplitFilePath(file, nullptr, &fileStr, &extensionStr);

	relativePath = relativePath.append(dstFolder).append("/") + fileStr.append(".") + extensionStr;
	std::string finalPath = std::string(*PHYSFS_getSearchPath()).append("/") + relativePath;

	return DuplicateFile(file, finalPath.c_str());

}

bool ModuleFileSystem::DuplicateFile(const char* srcFile, const char* dstFile)
{
	std::ifstream src;
	src.open(srcFile, std::ios::binary);
	bool srcOpen = src.is_open();
	std::ofstream  dst(dstFile, std::ios::binary);
	bool dstOpen = dst.is_open();

	dst << src.rdbuf();

	src.close();
	dst.close();

	if (srcOpen && dstOpen)
	{
		LOG_CONSOLE("[success] File Duplicated Correctly");
		return true;
	}
	else
	{
		LOG_CONSOLE("[error] File could not be duplicated");
		return false;
	}
}

int close_sdl_rwops(SDL_RWops* rw)
{
	RELEASE_ARRAY(rw->hidden.mem.base);
	SDL_FreeRW(rw);
	return 0;
}

// Save a whole buffer to disk
uint ModuleFileSystem::Save(const char* file, const void* buffer, unsigned int size, bool append) const
{
	unsigned int ret = 0;

	bool overwrite = PHYSFS_exists(file) != 0;
	PHYSFS_file* fsFile = (append) ? PHYSFS_openAppend(file) : PHYSFS_openWrite(file);

	if (fsFile != nullptr)
	{
		uint written = (uint)PHYSFS_write(fsFile, (const void*)buffer, 1, size);
		if (written != size)
		{
			LOG_CONSOLE("[error] File System error while writing to file %s: %s", file, PHYSFS_getLastError());
		}
		else
		{
			if (append == true)
			{
				LOG("Added %u data to [%s%s]", size, GetWriteDir(), file);
			}
			else if (overwrite == true)
			{
				LOG("File [%s%s] overwritten with %u bytes", GetWriteDir(), file, size);
			}
			else
			{
				LOG("New file created [%s%s] of %u bytes", GetWriteDir(), file, size);
			}

			ret = written;
		}

		if (PHYSFS_close(fsFile) == 0) LOG("[error] File System error while closing file %s: %s", file, PHYSFS_getLastError());
	}
	else
	{
		LOG_CONSOLE("[error] File System error while opening file %s: %s", file, PHYSFS_getLastError());
	}

	return ret;
}

std::string ModuleFileSystem::GetUniqueName(const char* path, const char* name) const
{
	//TODO: modify to distinguix files and dirs?
	std::vector<std::string> files, dirs;
	DiscoverFiles(path, files, dirs);

	std::string finalName(name);
	bool unique = false;

	for (uint i = 0; i < 50 && unique == false; ++i)
	{
		unique = true;

		//Build the compare name (name_i)
		if (i > 0)
		{
			finalName = std::string(name).append("_");
			if (i < 10)
				finalName.append("0");
			finalName.append(std::to_string(i));
		}

		//Iterate through all the files to find a matching name
		for (uint f = 0; f < files.size(); ++f)
		{
			if (finalName == files[f])
			{
				unique = false;
				break;
			}
		}
	}
	return finalName;
}

std::string ModuleFileSystem::SetNormalName(const char* path) {

	std::string name(path);
	std::string newName;
	bool found = false;
	for (size_t i = 0; i < name.size(); i++)
	{
		if (name.at(i) == 0x5c)
		{
			found = true;
		}
	}

	if (found)
	{
		newName = name.substr(name.find_last_of(0x5c) + 1);
	}
	else
	{
		newName = name.substr(name.find_last_of('/') + 1);
	}

	return newName;
}