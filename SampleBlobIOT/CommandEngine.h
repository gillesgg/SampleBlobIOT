#pragma once
class CCommandEngine
{
public:
	CCommandEngine();
	~CCommandEngine();

	HRESULT ParseCommandLine(int argc,
		_TCHAR* argv[],
		std::vector<std::string>& wvecfiles,
		std::string &wstrfile);
};

