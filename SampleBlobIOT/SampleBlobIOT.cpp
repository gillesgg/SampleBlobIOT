// SampleBlobIOT.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CommandEngine.h"
#include "IOTStreaming.h"
#include "utility.h"

/// <summary>
/// starting point of the sample
/// </summary>
/// <param name="strCred">IOT credential syntax : HostName=<host name>;DeviceId=<DeviceID> ;SharedAccessKey=<the SharedAccessKey for the device>  </param>
/// <param name="wvecfiles">File to upload</param>
/// <returns></returns>
HRESULT iothub_client_sample_http_run(std::string strCred, std::vector<std::string> wvecfiles)
{
	HRESULT						hr = S_OK;
	IOTHUB_CLIENT_LL_HANDLE		iotHubClientHandle;
	IOTStreaming				ioHubRestCall;


	for (auto strFile : wvecfiles)
	{
		if (platform_init() != 0)
		{
			BOOST_LOG_TRIVIAL(error) << __FILE__ << ", Failed to initialize the platform.";
		}

		hr = ioHubRestCall.SetConnectionString(strCred);
		if (FAILED(hr))
		{
			BOOST_LOG_TRIVIAL(error) << __FILE__ << " ERROR: ioHubRestCall.SetConnectionString failed hr:" << hr;
			return hr;
		}
		if ((iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(strCred.c_str(), HTTP_Protocol)) == nullptr)
		{
			BOOST_LOG_TRIVIAL(error) << __FILE__ << " ERROR: iotHubClientHandle is NULL";
			return E_FAIL;
		}

		hr = ioHubRestCall.Authenticate();
		if (FAILED(hr))
		{
			BOOST_LOG_TRIVIAL(error) << __FILE__ << " ERROR: ioHubRestCall.Authenticate failed hr:" << hr;
			return hr;
		}

		hr = ioHubRestCall.SendToBlobAsync(strFile).wait();
		if (FAILED(hr))
		{
			BOOST_LOG_TRIVIAL(error) << __FILE__ << " failed hr:" << hr;
			return hr;
		}
		hr = ioHubRestCall.Save(strFile);
		if (FAILED(hr))
		{
			BOOST_LOG_TRIVIAL(error) << __FILE__ << " failed hr:" << hr;
		}

		hr = ioHubRestCall.Commit().wait();
		if (FAILED(hr))
		{
			BOOST_LOG_TRIVIAL(error) << __FILE__ << " failed hr:" << hr;
		}

		IoTHubClient_LL_Destroy(iotHubClientHandle);
		platform_deinit();

	}
	return (hr);
}
/// <summary>
///  starting point of execution  
/// </summary>
/// <param name="argc"></param>
/// <param name="argv"></param>
/// <returns></returns>
int _tmain(int argc, _TCHAR* argv[])
{
	HRESULT								hr = S_OK;
	CCommandEngine						commandLine;
	std::vector<std::string>			wvecfiles;
	std::string							wstrCred;
	if (argc == NULL)
	{
		BOOST_LOG_TRIVIAL(error) << ", unable to parse command line";
		return 2;
	}
	hr = commandLine.ParseCommandLine(argc, argv, wvecfiles, wstrCred);
	if (hr == E_START)
	{
		return iothub_client_sample_http_run(wstrCred, wvecfiles);

	}
    return 0;
}

