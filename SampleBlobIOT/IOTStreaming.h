#pragma once
class IOTStreaming
{
public:
	IOTStreaming();
	~IOTStreaming();
public:
	HRESULT SetConnectionString(std::string strConnection);
	HRESULT Authenticate();
	std::string GetConnectionString();
	std::string GetHostName();
	std::string GetSharedAccessKey();
	std::string GetDeviceId();
	pplx::task<HRESULT>  SendToBlobAsync(std::string);
	HRESULT Save(std::string wstrFileName);

private:
	HRESULT DisplayJSONValue(web::json::value v);
	HRESULT Extract_Connection(std::string);
	void Extract_String(std::string , std::string& );
private:
	
	std::wstring							_wsConnection;
	
	std::wstring							_wstrAuthentication;

	std::map<std::string, std::string>		_wmTokens;
	const std::string						_strHostName;
	const std::string						_strDeviceId;
	const std::string						_strSharedAccessKey;

	std::wstring							_wsContainerName;
	std::wstring							_wssasToken;
	std::wstring							_wshostName;
	std::wstring							_wsblobName;

	std::wstring							_wsError;
	std::wstring							_wsMessage;

};
