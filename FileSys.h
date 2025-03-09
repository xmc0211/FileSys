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

#ifndef FILESYS_H
#define FILESYS_H

#include <Windows.h>
#include <string>
#include "FileBinIO.h"

// 文件操作枚举
enum FSFILEACTION {
	FS_OPEN,			// 用默认方式打开
	FS_PRINT,			// 打印（仅限文档）
	FS_EDIT,			// 用默认编辑器编辑（如果有）
	FS_EXPLORE,			// 用文件资源管理器打开
//	FS_FIND,			// 搜索（仅限文档） // 暂不提供此功能
	FS_PROPERTIES,		// 打开属性对话框
	FS_RUNASADMIN,		// 用管理员身份运行（仅限可执行文件）
};

// 添加或去除文件属性
#define FSAddObjectAttribute(f, a) FSSetObjectAttribute((f), FSGetObjectAttribute((f)) | (a))
#define FSRemoveObjectAttribute(f, a) FSSetObjectAttribute((f), FSGetObjectAttribute((f)) & ~(a))

// 路径回调函数。返回TRUE继续枚举，返回FALSE停止枚举。
typedef BOOL(CALLBACK* FS_PATH_CALLBACK)(std::string ExistFullPath, LPVOID lpParam);

// 判断文件或目录是否存在
BOOL FSObjectExist(_In_ LPCSTR lpFullPath);
// 查看文件或目录属性
DWORD FSGetObjectAttribute(_In_ LPCSTR lpExistFullPath);
// 设置文件或目录属性
BOOL FSSetObjectAttribute(_In_ LPCSTR lpExistFullPath, _In_opt_ UINT uFileAttribute);
// 对文件执行操作
// 可用操作可以参考枚举内注释。
HANDLE FSOpenObject(_In_ LPCSTR lpExistFullPath, _In_opt_ FSFILEACTION fAction = FS_OPEN, _In_opt_ LPCSTR lpParameters = NULL, _In_opt_ BOOL bWaitForThread = TRUE, _In_opt_ UINT uShowFlags = SW_SHOWNORMAL);
// 判断路径是否为文件。若不存在则返回FALSE，若都存在则返回bDefault。
BOOL FSObjectIsFile(_In_ LPCSTR lpFullPath);

// 获取当前文件名
std::string FSGetCurrentFilePath();

// 格式化完整路径
std::string FSFormat(_In_ LPCSTR lpFormat, _In_ LPCSTR lpFullPath);

// 创建空文件
BOOL FSCreateFile(_In_ LPCSTR lpFullPath);
// 删除文件
BOOL FSDeleteFile(_In_ LPCSTR lpExistFullPath);
// 移动文件
BOOL FSMoveFile(_In_ LPCSTR lpExistFullPath, _In_ LPCSTR lpNewFullPath, _In_opt_ BOOL bFailIfExists = TRUE);
// 重命名文件
BOOL FSRenameFile(_In_ LPCSTR lpExistFullPath, _In_ LPCSTR lpNewFileName, _In_opt_ BOOL bFailIfExists = TRUE);
// 复制文件
BOOL FSCopyFile(_In_ LPCSTR lpExistFullPath, _In_ LPCSTR lpNewFullPath, _In_opt_ BOOL bFailIfExists = TRUE);

// 创建空目录
BOOL FSCreateDir(_In_ LPCSTR lpFullPath);
// 删除目录
BOOL FSDeleteDir(_In_ LPCSTR lpFullPath);
// 移动目录
BOOL FSMoveDir(_In_ LPCSTR lpExistFullPath, _In_ LPCSTR lpNewFullPath, _In_opt_ BOOL bFailIfExists = TRUE);
// 重命名目录
BOOL FSRenameDir(_In_ LPCSTR lpExistFullPath, _In_ LPCSTR lpNewFileName, _In_opt_ BOOL bFailIfExists = TRUE);
// 复制目录
BOOL FSCopyDir(_In_ LPCSTR lpExistFullPath, _In_ LPCSTR lpNewFullPath, _In_opt_ BOOL bFailIfExists = TRUE);

// 枚举目录下一级子目录
BOOL FSEnumDir(_In_ LPCSTR lpExistDirPath, _In_ FS_PATH_CALLBACK cbFunc, _In_opt_ LPVOID lpParam);
// 枚举目录下一级子文件
BOOL FSEnumFile(_In_ LPCSTR lpExistDirPath, _In_ FS_PATH_CALLBACK cbFunc, _In_opt_ LPVOID lpParam);
// 枚举目录下所有子目录
BOOL FSEnumAllDir(_In_ LPCSTR lpExistDirPath, _In_ FS_PATH_CALLBACK cbFunc, _In_opt_ LPVOID lpParam);
// 枚举目录下所有子文件
BOOL FSEnumAllFile(_In_ LPCSTR lpExistDirPath, _In_ FS_PATH_CALLBACK cbFunc, _In_opt_ LPVOID lpParam);

#endif

