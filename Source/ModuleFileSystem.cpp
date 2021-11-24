#include "ModuleFileSystem.h"
#include "Globals.h"
#include "Application.h"

#include "physfs.h"
#include <fstream>
#include <filesystem>

#include "cfileio.h"
#include "types.h"

ModuleFileSystem::ModuleFileSystem(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	// needs to be created before Init so other modules can use it
	char* basePath = SDL_GetBasePath();
	systemBasePath = std::string(basePath);
	PHYSFS_init(nullptr);
	SDL_free(basePath);

	AddPath(".");
	AddPath("./Assets");
	AddPath("./Assets/Resources");
	AddPath("./Assets/Resources/Textures");
	AddPath("./Assets/Resources/Models");
}

// Destructor
ModuleFileSystem::~ModuleFileSystem()
{
	PHYSFS_deinit();
}

// Called before render is available
bool ModuleFileSystem::Init()
{
	LOG_CONSOLE("Loading File System");
	bool ret = true;

	//Setting the working directory as the writing directory
	if (PHYSFS_setWriteDir(".") == 0) { LOG_CONSOLE("File System error while creating write dir: %s\n", PHYSFS_getLastError()); }
	if (PHYSFS_init(nullptr) == 0) {

		LOG_CONSOLE("PhysFS succesfully loaded | Libs initialized");

	}

	CreateLibraryDirectories();


	return ret;
}

// Called before quitting
bool ModuleFileSystem::CleanUp()
{
	LOG_CONSOLE("Freeing File System subsystem");

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
}

// Add a new zip file or folder
bool ModuleFileSystem::AddPath(const char* path_or_zip)
{
	bool ret = false;

	if (PHYSFS_mount(path_or_zip, nullptr, 1) == 0)
	{
		LOG_CONSOLE("File System error while adding a path or zip: %s\n", PHYSFS_getLastError());
	}
	else
	{
		ret = true;
	}

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
	//TODO: erase first annoying dot (".")
	return PHYSFS_getWriteDir();
}

void ModuleFileSystem::DiscoverFiles(const char* directory, std::vector<std::string>& file_list, std::vector<std::string>& dir_list) const
{
	char** rc = PHYSFS_enumerateFiles(directory);
	char** i;

	for (i = rc; *i != nullptr; i++)
	{
		std::string str = std::string(directory) + std::string("/") + std::string(*i);
		if (IsDirectory(str.c_str()))
			dir_list.push_back(*i);
		else
			file_list.push_back(*i);
	}

	PHYSFS_freeList(rc);
}

void ModuleFileSystem::GetAllFilesWithExtension(const char* directory, const char* extension, std::vector<std::string>& file_list) const
{
	std::vector<std::string> files;
	std::vector<std::string> dirs;
	DiscoverFiles(directory, files, dirs);

	for (uint i = 0; i < files.size(); i++)
	{
		std::string ext;
		SplitFilePath(files[i].c_str(), nullptr, nullptr, &ext);

		if (ext == extension)
			file_list.push_back(files[i]);
	}
}
/*
PathNode ModuleFileSystem::GetAllFiles(const char* directory, std::vector<std::string>* filter_ext, std::vector<std::string>* ignore_ext) const
{
	PathNode root;
	if (Exists(directory))
	{
		root.path = directory;
		App->file_system->SplitFilePath(directory, nullptr, &root.localPath);
		if (root.localPath == "")
			root.localPath = directory;
		std::vector<std::string> file_list, dir_list;
		DiscoverFiles(directory, file_list, dir_list);

		//Adding all child directories
		for (uint i = 0; i < dir_list.size(); i++)
		{
			std::string str = directory;
			str.append("/").append(dir_list[i]);
			root.children.push_back(GetAllFiles(str.c_str(), filter_ext, ignore_ext));
		}
		//Adding all child files
		for (uint i = 0; i < file_list.size(); i++)
		{
			//Filtering extensions
			bool filter = true, discard = false;
			if (filter_ext != nullptr)
			{
				filter = HasExtension(file_list[i].c_str(), *filter_ext);
			}
			if (ignore_ext != nullptr)
			{
				discard = HasExtension(file_list[i].c_str(), *ignore_ext);
			}
			if (filter == true && discard == false)
			{
				std::string str = directory;
				str.append("/").append(file_list[i]);
				root.children.push_back(GetAllFiles(str.c_str(), filter_ext, ignore_ext));
			}
		}
		root.isFile = HasExtension(root.path.c_str());
		root.isLeaf = root.children.empty() == true;
	}
	return root;
}
*/
void ModuleFileSystem::GetRealDir(std::string path, std::string& output) const
{
	uint i = 0;
	while (i < path.length() && i < systemBasePath.length() && path[i] == systemBasePath[i])
	{
		i++;
	}
	if (i < path.length())
	{
		output = path.substr(i, path.length() - i);
	}
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
	if (ext == "")
		return true;
	for (uint i = 0; i < extensions.size(); i++)
	{
		if (extensions[i] == ext)
			return true;
	}
	return false;
}

std::string ModuleFileSystem::NormalizePath(const char* full_path) const
{
	std::string newPath(full_path);
	for (int i = 0; i < newPath.size(); ++i)
	{
		if (newPath[i] == '\\')
			newPath[i] = '/';
	}
	return newPath;
}

void ModuleFileSystem::SplitFilePath(const char* full_path, std::string* path, std::string* file, std::string* extension) const
{
	if (full_path != nullptr)
	{
		std::string full(full_path);
		size_t pos_separator = full.find_last_of("\\/");
		size_t pos_dot = full.find_last_of(".");

		if (path != nullptr)
		{
			if (pos_separator < full.length())
				*path = full.substr(0, pos_separator + 1);
			else
				path->clear();
		}

		if (file != nullptr)
		{
			if (pos_separator < full.length())
				*file = full.substr(pos_separator + 1, pos_dot - pos_separator - 1);
			else
				*file = full.substr(0, pos_dot);
		}

		if (extension != nullptr)
		{
			if (pos_dot < full.length())
				*extension = full.substr(pos_dot + 1);
			else
				extension->clear();
		}
	}
}

unsigned int ModuleFileSystem::Load(const char* path, const char* file, char** buffer) const
{
	std::string full_path(path);
	full_path += file;
	return Load(full_path.c_str(), buffer);
}

// Read a whole file and put it in a new buffer
uint ModuleFileSystem::Load(const char* file, char** buffer) const
{
	uint ret = 0;

	PHYSFS_file* fs_file = PHYSFS_openRead(file);

	if (fs_file != nullptr)
	{
		PHYSFS_sint32 size = (PHYSFS_sint32)PHYSFS_fileLength(fs_file);

		if (size > 0)
		{
			*buffer = new char[size + 1];
			uint readed = (uint)PHYSFS_read(fs_file, *buffer, 1, size);
			if (readed != size)
			{
				LOG_CONSOLE("File System error while reading from file %s: %s\n", file, PHYSFS_getLastError());
				RELEASE_ARRAY(buffer);
			}
			else
			{
				ret = readed;
				//Adding end of file at the end of the buffer. Loading a shader file does not add this for some reason
				(*buffer)[size] = '\0';
			}
		}

		if (PHYSFS_close(fs_file) == 0)
			LOG_CONSOLE("File System error while closing file %s: %s\n", file, PHYSFS_getLastError());
	}
	else
		LOG_CONSOLE("File System error while opening file %s: %s\n", file, PHYSFS_getLastError());

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
	//TODO: Compare performance to calling Load(srcFile) and then Save(dstFile)
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
	PHYSFS_file* fs_file = (append) ? PHYSFS_openAppend(file) : PHYSFS_openWrite(file);

	if (fs_file != nullptr)
	{
		uint written = (uint)PHYSFS_write(fs_file, (const void*)buffer, 1, size);
		if (written != size)
		{
			LOG_CONSOLE("[error] File System error while writing to file %s: %s", file, PHYSFS_getLastError());
		}
		else
		{
			if (append == true) { LOG("Added %u data to [%s%s]", size, GetWriteDir(), file); }
			else if (overwrite == true) { LOG("File [%s%s] overwritten with %u bytes", GetWriteDir(), file, size); }
			else { LOG("New file created [%s%s] of %u bytes", GetWriteDir(), file, size); }

			ret = written;
		}

		if (PHYSFS_close(fs_file) == 0) { LOG("[error] File System error while closing file %s: %s", file, PHYSFS_getLastError()); }
	}
	else { LOG_CONSOLE("[error] File System error while opening file %s: %s", file, PHYSFS_getLastError()); }

	return ret;
}
/*
bool ModuleFileSystem::Remove(const char * file)
{
	bool ret = false;
	if (file != nullptr)
	{
		//If it is a directory, we need to recursively remove all the files inside
		if (IsDirectory(file))
		{
			std::vector<std::string> containedFiles, containedDirs;
			PathNode rootDirectory = GetAllFiles(file);

			for (uint i = 0; i < rootDirectory.children.size(); ++i)
				Remove(rootDirectory.children[i].path.c_str());
		}

		if (PHYSFS_delete(file) != 0)
		{
			LOG("File deleted: [%s]", file);
			ret = true;
		}
		else
			LOG("File System error while trying to delete [%s]: %s", file, PHYSFS_getLastError());
	}
	return ret;
}
*/
/*
uint64 ModuleFileSystem::GetLastModTime(const char* filename)
{
	return PHYSFS_getLastModTime(filename);
}
*/
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
	std::string new_name;
	bool found = false;
	for (size_t i = 0; i < name.size(); i++)
	{
		if (name.at(i) == 0x5c) {
			found = true;
		}
	}

	if (found) {
		new_name = name.substr(name.find_last_of(0x5c) + 1);
	}
	else {
		new_name = name.substr(name.find_last_of('/') + 1);
	}
	return new_name;
}