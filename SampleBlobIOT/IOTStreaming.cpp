#include "stdafx.h"
#include "IOTStreaming.h"
#include "utility.h"

static size_t TEST_EXPIRY_TIME = 60 * 2;

IOTStreaming::IOTStreaming() : _strHostName("HostName") , _strDeviceId("DeviceId") , _strSharedAccessKey("SharedAccessKey")
{

}


IOTStreaming::~IOTStreaming()
{
}

void IOTStreaming::Extract_String(std::string strSource, std::string& strDest)
{
	size_t pos = strSource.find_first_of("=");
	if (pos != std::string::npos)
		strDest = strSource.substr(pos + 1);
}

HRESULT IOTStreaming::Extract_Connection(std::string strConnection)
{

	boost::char_separator<char> sep(";");
	typedef boost::tokenizer< boost::char_separator<char> > t_tokenizer;
	t_tokenizer tok(strConnection, sep);
	for (t_tokenizer::iterator beg = tok.begin(); beg != tok.end(); ++beg)
	{
		std::string str = *beg;

		if (str.find(_strHostName) != std::string::npos)
		{
			Extract_String(str, _wmTokens[_strHostName]);
		}
		if (str.find(_strDeviceId) != std::string::npos)
		{
			Extract_String(str, _wmTokens[_strDeviceId]);
		}
		if (str.find(_strSharedAccessKey) != std::string::npos)
		{
			Extract_String(str, _wmTokens[_strSharedAccessKey]);
		}
	}
	if (_wmTokens.size() != 3)
	{
		BOOST_LOG_TRIVIAL(error) << __FILE__ << ", invalid parameter";
		return HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);
	}
	return S_OK;
}

HRESULT IOTStreaming::SetConnectionString(std::string strConnection)
{
	return( Extract_Connection(strConnection));
}

std::string IOTStreaming::GetConnectionString()
{
	return _strHostName + "=" + _wmTokens[_strHostName] + ";" + _strDeviceId + "=" + _wmTokens[_strDeviceId] + ";" + _strSharedAccessKey + "=" + _wmTokens[_strSharedAccessKey];
}

std::string IOTStreaming::GetHostName()
{
	return _wmTokens[_strHostName];
}

std::string IOTStreaming::GetDeviceId()
{
	return _wmTokens[_strDeviceId];
}
std::string IOTStreaming::GetSharedAccessKey()
{
	return _wmTokens[_strSharedAccessKey];
}


/// <summary>
/// I use IoTHubClient_Auth_Create and IoTHubClient_Auth_Get_SasToken to construct the sas token
/// </summary>
/// <returns></returns>
HRESULT IOTStreaming::Authenticate()
{
	USES_CONVERSION;

	IOTHUB_AUTHORIZATION_HANDLE handle = IoTHubClient_Auth_Create( GetSharedAccessKey().c_str() , GetDeviceId().c_str() , NULL);
	char* conn_string = IoTHubClient_Auth_Get_SasToken(handle, GetHostName().c_str(), TEST_EXPIRY_TIME);
	
	if (nullptr == conn_string)
	{
		return HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);
	}

	_wstrAuthentication = A2W(conn_string);

	if (_wstrAuthentication.empty())
	{
		return HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);
	}	
	return S_OK;
}

/// <summary>
/// create the File uploads REST  https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-file-upload
/// </summary>
/// <param name="strFileName"></param>
/// <returns></returns>
pplx::task<HRESULT> IOTStreaming::SendToBlobAsync(std::string strFileName)
{
	USES_CONVERSION;
	

	std::wstring wstrURI;
	web::json::value obj;

	std::string strURI		= boost::str(boost::format("https://%1%/devices/%2%/files?api-version=2016-11-14") % GetHostName() % GetDeviceId());

	/*
	
	{
	"correlationId": "{correlation ID received from the initial request}",
	"isSuccess": bool,
	"statusCode": XXX,
	"statusDescription": "Description of status"
	}
	
	*/


	wstrURI = A2W(strURI.c_str());

	web::http::client::http_client client(wstrURI);
	
	obj[L"blobName"] = web::json::value::string(utility::conversions::to_string_t(strFileName));

	web::http::http_request request(web::http::methods::POST);

	request.headers().add(L"Authorization", _wstrAuthentication.c_str());
	request.headers().add(L"Host", A2W(GetHostName().c_str()));
	request.headers().add(L"Content-Type", L"application/json");

	request.set_body(obj);

	return client.request(request).then([](web::http::http_response response) -> pplx::task<web::json::value>
	{

		if (response.status_code() == web::http::status_codes::OK)
		{
			return response.extract_json();
		}
		else
		{
			BOOST_LOG_TRIVIAL(error) << __FILE__ << "SendToBlobAsync error, status code:" << response.status_code();
			return response.extract_json();
		}
	}).then([this](pplx::task<web::json::value> previousTask)
	{
		HRESULT hr = S_OK;
		try
		{
			web::json::value const & value = previousTask.get();
			hr = DisplayJSONValue(value);
			return (hr);
			
		}
		catch (const pplx::task_canceled& e)
		{
			BOOST_LOG_TRIVIAL(error) << __FILE__ << "task canceled";
			return (E_FAIL);
		}
		catch (std::exception const & e)
		{
			std::wcout << e.what() << std::endl;
			return (E_FAIL);
		}
	});
}


HRESULT IOTStreaming::DisplayJSONValue(web::json::value v)
{
	if (!v.is_null())
	{
		// Loop over each element in the object
		for (auto iter = v.as_object().cbegin(); iter != v.as_object().cend(); ++iter)
		{
			// It is necessary to make sure that you get the value as const reference
			// in order to avoid copying the whole JSON value recursively (too expensive for nested objects)

			const utility::string_t &key = iter->first;
			const web::json::value &value = iter->second;

			std::wcout << L"Key: " << key << L", Value: " << value.as_string() << std::endl;
			
			if (key == _T("containerName"))
			{
				_wsContainerName = value.as_string();
			}

			if (key == _T("hostName"))
			{
				_wshostName = value.as_string();
			}

			if (key == _T("sasToken"))
			{
				_wssasToken = value.as_string();
			}

			if (key == _T("blobName"))
			{
				_wsblobName = value.as_string();
			}
			if (key == _T("erreur"))
			{
				_wsError = value.as_string();
 			}
			if (key == _T("Message"))
			{
				_wsMessage = value.as_string();
				return E_FAIL;
			}

			if (key == _T("correlationId"))
			{
				_wscorrelationId = value.as_string();

			}
			
		}
	}
	return S_OK;
}

HRESULT IOTStreaming::Save(std::string strFileName)
{
	USES_CONVERSION;

	try
	{
		std::string s1 = Utility::urlEncode(W2A(_wsblobName.c_str()));
		_wsblobName = A2W(s1.c_str());

		std::wstring strURI = boost::str(boost::wformat(_T("https://%1%/%2%/%3%%4%")) % _wshostName % _wsContainerName % _wsblobName % _wssasToken);
		web::http::uri httpURI(strURI);

		azure::storage::storage_uri sas_uri(httpURI);

		azure::storage::cloud_block_blob blob_file( sas_uri );


		Concurrency::streams::istream input_stream = concurrency::streams::file_stream<uint8_t>::open_istream(U("c:\\temp\\test.html")).get();

		blob_file.upload_from_file(A2W(strFileName.c_str()));
	}
	catch (const std::exception& e)
	{
		BOOST_LOG_TRIVIAL(error) << __FILE__ << e.what();
		return E_FAIL;
	}
	return S_OK;
}


// "https://iotggtest.azure-devices.net/devices/Device1/files/notifications?api-version=2016-11-14"
// sr=IOTGGTest.azure-devices.net%2Fdevices%2FDevice1&sig=ZrEhbPSwr6LJoiVeZJ62ftWHi2eQL7rgGbarUwqrphs%3D&se=1491244975


pplx::task<HRESULT> IOTStreaming::Commit()
{
	USES_CONVERSION;



	std::wstring wstrURI;
	web::json::value obj;

	std::string strURI = boost::str(boost::format("https://%1%/devices/%2%/files/notifications?api-version=2016-11-14") % GetHostName() % GetDeviceId());

	obj[L"correlationId"] = web::json::value::string(utility::conversions::to_string_t(_wscorrelationId));
	obj[L"isSuccess"] = web::json::value::boolean(true);
	obj[L"statusCode"] = web::json::value::number(0);
	obj[L"statusDescription"] = web::json::value::string(U(""));

	wstrURI = A2W(strURI.c_str());

	web::http::client::http_client client(wstrURI);



	web::http::http_request request(web::http::methods::POST);

	request.headers().add(L"Authorization", _wstrAuthentication.c_str());
	request.headers().add(L"Host", A2W(GetHostName().c_str()));
	request.headers().add(L"Content-Type", L"application/json");

	request.set_body(obj);

	return client.request(request).then([](web::http::http_response response) -> pplx::task<web::json::value>
	{

		if (response.status_code() == web::http::status_codes::OK)
		{
			return response.extract_json();
		}
		else
		{
			BOOST_LOG_TRIVIAL(error) << __FILE__ << " error, status code:" << response.status_code();
			return response.extract_json();
		}
	}).then([this](pplx::task<web::json::value> previousTask)
	{
		HRESULT hr = S_OK;
		try
		{
			web::json::value const & value = previousTask.get();
			hr = DisplayJSONValue(value);
			return (hr);

		}
		catch (const pplx::task_canceled& e)
		{
			BOOST_LOG_TRIVIAL(error) << __FILE__ << "task canceled";
			return (E_FAIL);
		}
		catch (std::exception const & e)
		{
			std::wcout << e.what() << std::endl;
			return (E_FAIL);
		}
	});
}