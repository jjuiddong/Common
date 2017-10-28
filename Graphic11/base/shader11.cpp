
#include "stdafx.h"
#include "shader11.h"


using namespace graphic;

cShader11::cShader11()
	: m_technique(NULL)
{
}

cShader11::~cShader11()
{
	Clear();
}


// fileName : *.fxo file
bool cShader11::Create(cRenderer &renderer, const StrPath &fileName
	, const char *techniqueName, const D3D11_INPUT_ELEMENT_DESC layout[], const int numElements)
{
	Clear();

	std::ifstream fin(fileName.c_str(), std::ios::binary);
	if (!fin.is_open())
		return false;

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();

	HRESULT hr = D3DX11CreateEffectFromMemory(&compiledShader[0], size, 0, renderer.GetDevice(), &m_effect);
	if (FAILED(hr))
		return false;

	m_technique = m_effect->GetTechniqueByName(techniqueName);
	RETV(!m_technique, false);

	// Create the input layout
	D3DX11_PASS_DESC passDesc;
	m_technique->GetPassByIndex(0)->GetDesc(&passDesc);

	if (!m_vtxLayout.Create(renderer, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, layout, numElements))
		return false;

	m_name = fileName.GetFileName();
	m_fxoFileName = fileName;

	return true;
}


bool cShader11::CompileAndReload(cRenderer &renderer)
{
	if (m_fxoFileName.empty())
		return false;

	//remove(m_fxoFileName.c_str()); // remove file
	const StrPath fxFileName = m_fxoFileName.GetFileNameExceptExt2() + ".fx";
	if (!Compile(fxFileName.c_str()))
		return false;

	if (!Create(renderer, m_fxoFileName, "Unlit", &m_vtxLayout.m_elements[0], m_vtxLayout.m_elements.size()))
		return false;

	return true;
}


bool cShader11::SetTechnique(const char *id)
{
	assert(m_effect);
	RETV(!m_effect, false);

	ID3DX11EffectTechnique *tech = m_effect->GetTechniqueByName(id);
	RETV(!tech, false);
	m_technique = tech;
	return true;
}


ID3DX11EffectTechnique* cShader11::GetTechnique(const char *id)
{
	return NULL;
}


ID3DX11EffectVariable* cShader11::GetVariable(const char *id)
{
	return NULL;
}


ID3DX11EffectMatrixVariable* cShader11::GetMatrix(const char *id)
{
	//return mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	return NULL;
}


ID3DX11EffectVectorVariable** cShader11::GetVector(const char *id)
{
	return NULL;
}


int cShader11::Begin()
{
	assert(m_effect);
	RETV(!m_technique, 0);

	D3DX11_TECHNIQUE_DESC techDesc;
	m_technique->GetDesc(&techDesc);
	return techDesc.Passes;
}


void cShader11::BeginPass(cRenderer &renderer, const int pass)
{
	assert(m_effect);
	RET(!m_technique);
	m_vtxLayout.Bind(renderer);
	m_technique->GetPassByIndex(pass)->Apply(0, renderer.GetDevContext());

	for (int i = 0; i < cRenderer::MAX_TEXTURE_STAGE; ++i)
	{
		//if (!renderer.m_textureMap[i])
		//	break;
		renderer.GetDevContext()->PSSetShaderResources(i + cRenderer::TEXTURE_OFFSET, 1, &renderer.m_textureMap[i]);
	}
}


void cShader11::Clear()
{
	SAFE_RELEASE(m_effect);
	m_technique = NULL;
}


// Compile Offline using FXC.exe
// https://msdn.microsoft.com/en-us/library/windows/desktop/bb509710(v=vs.85).aspx#using_the_effect-compiler_tool_in_a_subprocess
bool cShader11::Compile(const char *fileName
	, Str512 *outMsg //= NULL 
)
{
	WCHAR curDir[MAX_PATH];
	GetCurrentDirectoryW(sizeof(curDir), curDir);

	HANDLE hReadPipe, hWritePipe;
	HANDLE hErrReadPipe, hErrWritePipe = NULL;

	SECURITY_ATTRIBUTES sa;
	ZeroMemory(&sa, sizeof(sa));
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0))
		return false;

	if (!SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0))
		return false;

	if (!CreatePipe(&hErrReadPipe, &hErrWritePipe, &sa, 0))
		return false;

	if (!SetHandleInformation(hErrReadPipe, HANDLE_FLAG_INHERIT, 0))
		return false;

	PROCESS_INFORMATION ProcessInfo;
	ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));

	STARTUPINFO StartupInfo;
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(StartupInfo);
	StartupInfo.hStdError = hErrWritePipe;
	StartupInfo.hStdOutput = hWritePipe;
	StartupInfo.dwFlags |= STARTF_USESTDHANDLES;

	WStr128 srcFileName(Str128(fileName).wstr());
	WStr128 outputFileName = srcFileName.GetFileNameExceptExt2() + L".fxo";

	WStr128 cmdLine;
	cmdLine = L"/Fc /Od /Zi /T fx_5_0 /Fo ";
	cmdLine += outputFileName.c_str(); // output filename 
	cmdLine += L" "; // space
	cmdLine += srcFileName.c_str(); // shader filename

	const BOOL result = CreateProcess(L"fxc.exe", (LPWSTR)cmdLine.c_str()
		, NULL, NULL, TRUE, 0, NULL, curDir, &StartupInfo, &ProcessInfo);
	if (!result)
		return false;

	const DWORD BUFSIZE = 4096;
	BYTE buff[BUFSIZE];
	HANDLE WaitHandles[] = { ProcessInfo.hProcess, hReadPipe, hErrReadPipe };
	while (1)
	{
		const DWORD dwWaitResult = WaitForMultipleObjects(3, WaitHandles, FALSE, 60000L);

		DWORD dwBytesRead, dwBytesAvailable;
		while (PeekNamedPipe(hReadPipe, NULL, 0, NULL, &dwBytesAvailable, NULL) && dwBytesAvailable)
		{
			ReadFile(hReadPipe, buff, BUFSIZE - 1, &dwBytesRead, 0);
			if (outMsg)
				*outMsg += (char*)std::string((char*)buff, (size_t)dwBytesRead).c_str();
		}
		while (PeekNamedPipe(hErrReadPipe, NULL, 0, NULL, &dwBytesAvailable, NULL) && dwBytesAvailable)
		{
			ReadFile(hErrReadPipe, buff, BUFSIZE - 1, &dwBytesRead, 0);
			if (outMsg)
				*outMsg += (char*)std::string((char*)buff, (size_t)dwBytesRead).c_str();
		}

		// Process is done, or we timed out:
		if (dwWaitResult == WAIT_OBJECT_0 || dwWaitResult == WAIT_TIMEOUT)
			break;
	}

	CloseHandle(hReadPipe);
	CloseHandle(hWritePipe);
	CloseHandle(hErrReadPipe);
	CloseHandle(hErrWritePipe);

	return true;
}
