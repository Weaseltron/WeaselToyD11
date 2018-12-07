#include "Shader.h"

#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <vector>
#include <string>
#include <fstream>

#include "HelperFunction.h"
#include "type/ConstantBuffer.h"

HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut, std::vector<std::string>& errorList)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	// Disable optimizations to further improve shader debugging
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* pErrorBlob = nullptr;

	hr = D3DCompileFromFile(szFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			std::string str = std::string(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
			OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
			SplitString(str, errorList, '\n');
			pErrorBlob->Release();
		}

		D3DCompileFromFile(L"shaders/PixelShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, szEntryPoint, szShaderModel,
			dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}

HRESULT ScanShaderForCustomizable(const char* strProj, std::vector<CustomizableBuffer>& vCustomizableBuffer)
{
	// Open file
	std::string path = std::string("../../ShaderToyLibrary/") + strProj + std::string("/shaders/PixelShader.hlsl");
	std::ifstream inputFile(path.c_str(), std::ios::in);

	// Make sure the file was opened
	if (!inputFile)
	{
		return false;
	}

	vCustomizableBuffer.clear();

	// Initialize variables
	char line[MAX_PATH] = "";
	char dummy[MAX_PATH] = "";
	char dummy2[MAX_PATH] = "";
	char dummy3[MAX_PATH] = "";
	char bufferName[MAX_PATH] = "";

	do
	{
		// Finding the line where the constant buffer is set
		sscanf(line, "%s %s\n", &dummy, &bufferName);
	} while (inputFile.getline(line, MAX_PATH) && strcmp(bufferName, "cbCustomizable") != 0);

	// Skipping the line with the curly brackets
	inputFile.getline(line, MAX_PATH);

	if (strcmp(bufferName, "cbCustomizable") != 0)
		return S_OK;

	do
	{
		char strCommand[MAX_PATH] = "";
		float min = -1.0f;
		float max = -1.0f;
		float step = -1.0f;
		char strVarName[MAX_PATH] = "";
		char strMin[MAX_PATH] = "";
		char strMax[MAX_PATH] = "";
		char strStep[MAX_PATH] = "";

		CustomizableBuffer cb;

		inputFile.getline(line, MAX_PATH);
		int check = sscanf(line, "%s %s", &dummy, &strCommand);

		if (strcmp(strCommand, "slider") == 0)
		{
			sscanf(line, "%s %s %s %s %f %s %f \n", &dummy, &strCommand, &strVarName, &strMin, &min, &strMax, &max);
			cb.min = min;
			cb.max = max;
			cb.step = step;

			strcpy(cb.strCommand, strCommand);
			strcpy(cb.strVariable, strVarName);

			vCustomizableBuffer.push_back(cb);
		}
		else if (strcmp(strCommand, "input") == 0)
		{
			sscanf(line, "%s %s %s %s %f \n", &dummy, &strCommand, &strVarName, &strStep, &step);
			cb.min = min;
			cb.max = max;
			cb.step = step;

			strcpy(cb.strCommand, strCommand);
			strcpy(cb.strVariable, strVarName);

			vCustomizableBuffer.push_back(cb);
		}
		else if (strcmp(strCommand, "colorEdit") == 0
			|| strcmp(strCommand, "colourEdit") == 0)
		{
			sscanf(line, "%s %s %s \n", &dummy, &strCommand, &strVarName);
			cb.min = min;
			cb.max = max;
			cb.step = step;

			strcpy(cb.strCommand, strCommand);
			strcpy(cb.strVariable, strVarName);

			vCustomizableBuffer.push_back(cb);
		}

	} while (strcmp(dummy, "};") != 0);

	return S_OK;
}