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

// �ļ�����ö��
enum FSFILEACTION {
	FS_OPEN,			// ��Ĭ�Ϸ�ʽ��
	FS_PRINT,			// ��ӡ�������ĵ���
	FS_EDIT,			// ��Ĭ�ϱ༭���༭������У�
	FS_EXPLORE,			// ���ļ���Դ��������
//	FS_FIND,			// �����������ĵ��� // �ݲ��ṩ�˹���
	FS_PROPERTIES,		// �����ԶԻ���
	FS_RUNASADMIN,		// �ù���Ա������У����޿�ִ���ļ���
};

// ��ӻ�ȥ���ļ�����
#define FSAddObjectAttribute(f, a) FSSetObjectAttribute((f), FSGetObjectAttribute((f)) | (a))
#define FSRemoveObjectAttribute(f, a) FSSetObjectAttribute((f), FSGetObjectAttribute((f)) & ~(a))

// ·���ص�����������TRUE����ö�٣�����FALSEֹͣö�١�
typedef BOOL(CALLBACK* FS_PATH_CALLBACK)(std::string ExistFullPath, LPVOID lpParam);

// �ж��ļ���Ŀ¼�Ƿ����
BOOL FSObjectExist(_In_ LPCSTR lpFullPath);
// �鿴�ļ���Ŀ¼����
DWORD FSGetObjectAttribute(_In_ LPCSTR lpExistFullPath);
// �����ļ���Ŀ¼����
BOOL FSSetObjectAttribute(_In_ LPCSTR lpExistFullPath, _In_opt_ UINT uFileAttribute);
// ���ļ�ִ�в���
// ���ò������Բο�ö����ע�͡�
HANDLE FSOpenObject(_In_ LPCSTR lpExistFullPath, _In_opt_ FSFILEACTION fAction = FS_OPEN, _In_opt_ LPCSTR lpParameters = NULL, _In_opt_ BOOL bWaitForThread = TRUE, _In_opt_ UINT uShowFlags = SW_SHOWNORMAL);
// �ж�·���Ƿ�Ϊ�ļ������������򷵻�FALSE�����������򷵻�bDefault��
BOOL FSObjectIsFile(_In_ LPCSTR lpFullPath);

// ��ȡ��ǰ�ļ���
std::string FSGetCurrentFilePath();

// ��ʽ������·��
std::string FSFormat(_In_ LPCSTR lpFormat, _In_ LPCSTR lpFullPath);

// �������ļ�
BOOL FSCreateFile(_In_ LPCSTR lpFullPath);
// ɾ���ļ�
BOOL FSDeleteFile(_In_ LPCSTR lpExistFullPath);
// �ƶ��ļ�
BOOL FSMoveFile(_In_ LPCSTR lpExistFullPath, _In_ LPCSTR lpNewFullPath, _In_opt_ BOOL bFailIfExists = TRUE);
// �������ļ�
BOOL FSRenameFile(_In_ LPCSTR lpExistFullPath, _In_ LPCSTR lpNewFileName, _In_opt_ BOOL bFailIfExists = TRUE);
// �����ļ�
BOOL FSCopyFile(_In_ LPCSTR lpExistFullPath, _In_ LPCSTR lpNewFullPath, _In_opt_ BOOL bFailIfExists = TRUE);

// ������Ŀ¼
BOOL FSCreateDir(_In_ LPCSTR lpFullPath);
// ɾ��Ŀ¼
BOOL FSDeleteDir(_In_ LPCSTR lpFullPath);
// �ƶ�Ŀ¼
BOOL FSMoveDir(_In_ LPCSTR lpExistFullPath, _In_ LPCSTR lpNewFullPath, _In_opt_ BOOL bFailIfExists = TRUE);
// ������Ŀ¼
BOOL FSRenameDir(_In_ LPCSTR lpExistFullPath, _In_ LPCSTR lpNewFileName, _In_opt_ BOOL bFailIfExists = TRUE);
// ����Ŀ¼
BOOL FSCopyDir(_In_ LPCSTR lpExistFullPath, _In_ LPCSTR lpNewFullPath, _In_opt_ BOOL bFailIfExists = TRUE);

// ö��Ŀ¼��һ����Ŀ¼
BOOL FSEnumDir(_In_ LPCSTR lpExistDirPath, _In_ FS_PATH_CALLBACK cbFunc, _In_opt_ LPVOID lpParam);
// ö��Ŀ¼��һ�����ļ�
BOOL FSEnumFile(_In_ LPCSTR lpExistDirPath, _In_ FS_PATH_CALLBACK cbFunc, _In_opt_ LPVOID lpParam);
// ö��Ŀ¼��������Ŀ¼
BOOL FSEnumAllDir(_In_ LPCSTR lpExistDirPath, _In_ FS_PATH_CALLBACK cbFunc, _In_opt_ LPVOID lpParam);
// ö��Ŀ¼���������ļ�
BOOL FSEnumAllFile(_In_ LPCSTR lpExistDirPath, _In_ FS_PATH_CALLBACK cbFunc, _In_opt_ LPVOID lpParam);

#endif

