// MIT License
//
// Copyright (c) 2025 xmc0211
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "FileSys.h"
#include <sddl.h>
#include <AclAPI.h>
#include <shellapi.h>

#pragma comment(lib, "Shell32.lib")

// 为字符串分割提供支持
#define substr_p(s, t) substr((s), (t) - (s) + 1);

// 判断文件或目录是否存在
BOOL FSObjectExist(_In_ LPCSTR lpFullPath) {
	DWORD dwAttr = GetFileAttributesA(lpFullPath);
	BOOL bRes = (dwAttr != INVALID_FILE_ATTRIBUTES);
	return bRes;
}
// 查看文件或目录属性
DWORD FSGetObjectAttribute(_In_ LPCSTR lpExistFullPath) {
	return GetFileAttributesA(lpExistFullPath);
}
// 设置文件或目录属性
BOOL FSSetObjectAttribute(_In_ LPCSTR lpExistFullPath, _In_opt_ UINT uFileAttribute) {
	BOOL bRes = TRUE;
	bRes = (SetFileAttributesA(lpExistFullPath, uFileAttribute) == INVALID_FILE_ATTRIBUTES);
	return bRes;
}
// 对文件或目录执行操作
// 可用操作可以参考枚举内注释。
HANDLE FSOpenObject(_In_ LPCSTR lpExistFullPath, _In_opt_ FSFILEACTION fAction, _In_opt_ LPCSTR lpParameters, _In_opt_ BOOL bWaitForThread, _In_opt_ UINT uShowFlags) {
	BOOL bRes = TRUE;
	std::string Verb = "open";
	switch (fAction) {
	case FS_OPEN: { Verb = "open"; break; }
	case FS_EDIT: { Verb = "edit"; break; }
	case FS_EXPLORE: { Verb = "explore"; break; }
	//case FS_FIND: { Verb = "find"; break; } // 暂不提供此功能
	case FS_PRINT: { Verb = "print"; break; }
	case FS_PROPERTIES: { Verb = "properties"; break; }
	case FS_RUNASADMIN: { Verb = "runas"; break; }
	}
	if (Verb == "explore" && FSObjectIsFile(lpExistFullPath)) {
		return FSOpenObject("Explorer.EXE", FS_OPEN, (std::string("/SELECT, \"") + lpExistFullPath + "\"").c_str(), bWaitForThread, uShowFlags);
	}
	SHELLEXECUTEINFOA ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFOA);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_INVOKEIDLIST;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = Verb.c_str();
	ShExecInfo.lpFile = lpExistFullPath;
	ShExecInfo.lpParameters = lpParameters;
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = uShowFlags;
	ShExecInfo.hInstApp = NULL;
	ShExecInfo.lpIDList = NULL;
	bRes = ShellExecuteExA(&ShExecInfo);
	if (!bRes || ShExecInfo.hProcess == NULL) return NULL;
	if (bWaitForThread) WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
	return ShExecInfo.hProcess;
}
// 判断路径是否为文件。若不存在则返回FALSE，若都存在则返回TRUE。
BOOL FSObjectIsFile(_In_ LPCSTR lpFullPath) {
	if (!FSObjectExist(lpFullPath)) return FALSE;
	HANDLE hFile = CreateFileA(lpFullPath, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return FALSE;
	CloseHandle(hFile);
	return TRUE;
}

// 获取当前文件名
std::string FSGetCurrentFilePath() {
	CHAR Buffer[MAX_PATH];
	GetModuleFileNameA(NULL, Buffer, MAX_PATH);
	return std::string(Buffer);
}

// 格式化完整路径
std::string FSFormat(_In_ LPCSTR lpFormat, _In_ LPCSTR lpFullPath) {
	std::string op(lpFormat), Path(lpFullPath);
	std::string Res = "";
	size_t fSize = op.size(), pSize = Path.size();

	// 去引号
	if (Path[0] == '\"' && Path[pSize - 1] == '\"') {
		Path = Path.substr(1, pSize - 2);
		pSize -= 2;
	}

	// 获取各标志的分割点
	// 
	// C:\Windows\System32\CMD.EXE
	//  ]                 ]   )
	// D|       P         | N | X
	//
	size_t DrivePoint = Path.find_first_of(':');
	if (DrivePoint == std::string::npos) return "";
	size_t PathPoint = Path.find_last_of('\\');
	if (PathPoint == std::string::npos) return "";
	size_t NamePoint = Path.find_last_of('.');
	if (NamePoint == std::string::npos) return "";

	// 根据标志获得各部分内容
	std::string ResDrive = Path.substr_p(0, DrivePoint);
	std::string ResPath = Path.substr_p(DrivePoint + 1, PathPoint);
	std::string ResName = Path.substr_p(PathPoint + 1, NamePoint - 1);
	std::string ResExt = Path.substr(NamePoint);

	// 逐个字符处理并添加
	for (size_t Pt = 0; Pt < fSize; Pt++) {
		switch (op[Pt]) {
		case 'd': case 'D': {
			Res += ResDrive;
			break;
		}
		case 'p': case 'P': {
			Res += ResPath;
			break;
		}
		case 'n': case 'N': {
			Res += ResName;
			break;
		}
		case 'x': case 'X': {
			Res += ResExt;
			break;
		}
		case 'z': case 'Z': {
			Res += std::to_string(FBLIntToUl(FBGetFileSize(Path.c_str())));
			break;
		}
		default: Res += op[Pt];
		}
	}
	return Res;
}

// 创建空文件
BOOL FSCreateFile(_In_ LPCSTR lpFullPath) {
	HANDLE hFile = CreateFileA(lpFullPath, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return FALSE;
	CloseHandle(hFile);
	return TRUE;
}
// 删除文件
BOOL FSDeleteFile(_In_ LPCSTR lpFullPath) {
	BOOL bRes = DeleteFileA(lpFullPath);
	return bRes;
}
// 移动文件
BOOL FSMoveFile(_In_ LPCSTR lpExistFullPath, _In_ LPCSTR lpNewFullPath, _In_opt_ BOOL bFailIfExists) {
	if (FSObjectExist(lpNewFullPath)) {
		if (bFailIfExists) return FALSE;
		if (!FSDeleteFile(lpNewFullPath)) return FALSE;
	}
	BOOL bRes = MoveFileA(lpExistFullPath, lpNewFullPath);
	return bRes;
}
// 重命名文件
BOOL FSRenameFile(_In_ LPCSTR lpExistFullPath, _In_ LPCSTR lpNewFileName, _In_opt_ BOOL bFailIfExists) {
	BOOL bRes = TRUE;
	std::string ExistPath = FSFormat("dp", lpExistFullPath);
	if (ExistPath == "") return FALSE;
	bRes = FSMoveFile(lpExistFullPath, (ExistPath + lpNewFileName).c_str(), bFailIfExists);
	return bRes;
}
// 复制文件
BOOL FSCopyFile(_In_ LPCSTR lpExistFullPath, _In_ LPCSTR lpNewFullPath, _In_opt_ BOOL bFailIfExists) {
	BOOL bRes = CopyFileA(lpExistFullPath, lpNewFullPath, bFailIfExists);
	return bRes;
}

// 创建空目录
BOOL FSCreateDir(_In_ LPCSTR lpFullPath) {
	BOOL bRes = CreateDirectoryA(lpFullPath, NULL);
	return bRes;
}
// 删除目录
BOOL FSDeleteDir(_In_ LPCSTR lpExistFullPath) {
	std::string EnumPath = std::string(lpExistFullPath) + "\\*";
	WIN32_FIND_DATAA findData;
	HANDLE hFind = FindFirstFileA(EnumPath.c_str(), &findData);
	if (hFind == INVALID_HANDLE_VALUE) return FALSE;
	do {
		std::string NowPath = std::string(lpExistFullPath) + "\\" + findData.cFileName;
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0) continue;
			FSDeleteDir(NowPath.c_str());
		}
		else {
			FSDeleteFile(NowPath.c_str());
		}
	} while (FindNextFileA(hFind, &findData) != ERROR_SUCCESS);
	if (!RemoveDirectoryA(lpExistFullPath)) return FALSE;
	FindClose(hFind);
	return TRUE;
}
// 移动目录
BOOL FSMoveDir(_In_ LPCSTR lpExistFullPath, _In_ LPCSTR lpNewFullPath, _In_opt_ BOOL bFailIfExists) {
	std::string EnumPath = std::string(lpExistFullPath) + "\\*";
	WIN32_FIND_DATAA findData;
	HANDLE hFind = FindFirstFileA(EnumPath.c_str(), &findData);
	if (hFind == INVALID_HANDLE_VALUE) return FALSE;
	if (!FSObjectExist(lpNewFullPath)) {
		if (!FSCreateDir(lpNewFullPath)) return FALSE;
	}
	do {
		std::string NowPath = std::string(lpExistFullPath) + "\\" + findData.cFileName;
		std::string NewPath = std::string(lpNewFullPath) + "\\" + findData.cFileName;
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0) continue;
			FSMoveDir(NowPath.c_str(), NewPath.c_str(), bFailIfExists);
		}
		else {
			FSMoveFile(NowPath.c_str(), NewPath.c_str(), bFailIfExists);
		}
	} while (FindNextFileA(hFind, &findData) != ERROR_SUCCESS);
	if (!RemoveDirectoryA(lpExistFullPath)) return FALSE;
	FindClose(hFind);
	return TRUE;
}
// 重命名目录
BOOL FSRenameDir(_In_ LPCSTR lpExistFullPath, _In_ LPCSTR lpNewFileName, _In_opt_ BOOL bFailIfExists) {
	BOOL bRes = TRUE;
	size_t Pos = std::string(lpExistFullPath).find_last_of('\\');
	if (Pos == std::string::npos) return FALSE;
	std::string ExistPath = std::string(lpExistFullPath).substr(0, Pos + 1);
	bRes = FSMoveDir(lpExistFullPath, (ExistPath + lpNewFileName).c_str(), bFailIfExists);
	return bRes;
}
// 复制目录
BOOL FSCopyDir(_In_ LPCSTR lpExistFullPath, _In_ LPCSTR lpNewFullPath, _In_opt_ BOOL bFailIfExists) {
	std::string EnumPath = std::string(lpExistFullPath) + "\\*";
	WIN32_FIND_DATAA findData;
	HANDLE hFind = FindFirstFileA(EnumPath.c_str(), &findData);
	if (hFind == INVALID_HANDLE_VALUE) return FALSE;
	if (!FSObjectExist(lpNewFullPath)) {
		if (!FSCreateDir(lpNewFullPath)) return FALSE;
	}
	do {
		std::string NowPath = std::string(lpExistFullPath) + "\\" + findData.cFileName;
		std::string NewPath = std::string(lpNewFullPath) + "\\" + findData.cFileName;
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0) continue;
			FSCopyDir(NowPath.c_str(), NewPath.c_str(), bFailIfExists);
		}
		else {
			FSCopyFile(NowPath.c_str(), NewPath.c_str(), bFailIfExists);
		}
	} while (FindNextFileA(hFind, &findData) != ERROR_SUCCESS);
	FindClose(hFind);
	return TRUE;
}

// 枚举目录下一级子目录
BOOL FSEnumDir(_In_ LPCSTR lpExistDirPath, _In_ FS_PATH_CALLBACK cbFunc, _In_opt_ LPVOID lpParam) {
	std::string EnumPath = std::string(lpExistDirPath) + "\\*";
	WIN32_FIND_DATAA findData;
	HANDLE hFind = FindFirstFileA(EnumPath.c_str(), &findData);
	if (hFind == INVALID_HANDLE_VALUE) return FALSE;
	do {
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0) continue;
			if (!cbFunc(std::string(lpExistDirPath) + "\\" + findData.cFileName, lpParam)) break;
		}
	} while (FindNextFileA(hFind, &findData) != ERROR_SUCCESS);
	FindClose(hFind);
	return TRUE;
}
// 枚举目录下一级子文件
BOOL FSEnumFile(_In_ LPCSTR lpExistDirPath, _In_ FS_PATH_CALLBACK cbFunc, _In_opt_ LPVOID lpParam) {
	std::string EnumPath = std::string(lpExistDirPath) + "\\*";
	WIN32_FIND_DATAA findData;
	HANDLE hFind = FindFirstFileA(EnumPath.c_str(), &findData);
	if (hFind == INVALID_HANDLE_VALUE) return FALSE;
	do {
		if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			if (!cbFunc(std::string(lpExistDirPath) + "\\" + findData.cFileName, lpParam)) break;
		}
	} while (FindNextFileA(hFind, &findData) != ERROR_SUCCESS);
	FindClose(hFind);
	return TRUE;
}
// 枚举目录下所有子目录
BOOL FSEnumAllDir(_In_ LPCSTR lpExistDirPath, _In_ FS_PATH_CALLBACK cbFunc, _In_opt_ LPVOID lpParam) {
	std::string EnumPath = std::string(lpExistDirPath) + "\\*";
	WIN32_FIND_DATAA findData;
	HANDLE hFind = FindFirstFileA(EnumPath.c_str(), &findData);
	if (hFind == INVALID_HANDLE_VALUE) return FALSE;
	do {
		std::string NowPath = std::string(lpExistDirPath) + "\\" + findData.cFileName;
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0) continue;
			if (!cbFunc(NowPath, lpParam)) break;
			FSEnumAllDir(NowPath.c_str(), cbFunc, lpParam);
		}
	} while (FindNextFileA(hFind, &findData) != ERROR_SUCCESS);
	FindClose(hFind);
	return TRUE;
}
// 枚举目录下所有子文件
BOOL FSEnumAllFile(_In_ LPCSTR lpExistDirPath, _In_ FS_PATH_CALLBACK cbFunc, _In_opt_ LPVOID lpParam) {
	std::string EnumPath = std::string(lpExistDirPath) + "\\*";
	WIN32_FIND_DATAA findData;
	HANDLE hFind = FindFirstFileA(EnumPath.c_str(), &findData);
	if (hFind == INVALID_HANDLE_VALUE) return FALSE;
	do {
		std::string NowPath = std::string(lpExistDirPath) + "\\" + findData.cFileName;
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0) continue;
			FSEnumAllFile(NowPath.c_str(), cbFunc, lpParam);
		}
		else {
			if (!cbFunc(NowPath, lpParam)) break;
		}
	} while (FindNextFileA(hFind, &findData) != ERROR_SUCCESS);
	FindClose(hFind);
	return TRUE;
}