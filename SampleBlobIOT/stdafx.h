// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define _SCL_SECURE_NO_WARNINGS  

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>


#pragma region CommonHeader
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <tchar.h>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#pragma region

#pragma region string_wstring_convertor
#include <AtlBase.h>
#include <AtlConv.h>
#pragma region

#pragma region restSDK_pplSDK_wasSDK
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <cpprest/filestream.h>
#include <pplx/pplxtasks.h>
#include <was/storage_account.h>
#include <was/blob.h>
#pragma region

#pragma region boost_sdk
#include <boost/log/trivial.hpp>
#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

namespace		po = boost::program_options;

#pragma region


#ifdef ARDUINO
#include "AzureIoT.h"
#else
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/platform.h"

#include <D:\DEV\DEV.Schneider\IOT\azure-iot-sdk-c-master\iothub_client\inc\iothub_client_authorization.h>


#include "iothub_client_ll.h"
#include "iothub_message.h"
#include "iothubtransporthttp.h"
#endif



#ifdef MBED_BUILD_TIMESTAMP
#include "certs.h"
#endif // MBED_BUILD_TIMESTAMP

#define E_START 0x8001FFFFL