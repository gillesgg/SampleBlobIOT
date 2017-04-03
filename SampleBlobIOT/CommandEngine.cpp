#include "stdafx.h"
#include "CommandEngine.h"


CCommandEngine::CCommandEngine()
{
}


CCommandEngine::~CCommandEngine()
{
}

HRESULT CCommandEngine::ParseCommandLine(int argc,
	_TCHAR* argv[],
	std::vector<std::string>& wvecfiles,
	std::string &wstrCred)
{
	po::variables_map vm;
	po::options_description desc("Options");

	try
	{
		desc.add_options()
			("help,h", "Print help messages")
			("files,f", po::value<std::vector<std::string> >()->multitoken(), "files to copy")
			("cred,c", po::value<std::string>(&wstrCred), "iot credencial");

		po::store(po::parse_command_line(argc, argv, desc), vm); // can throw
		po::notify(vm);


		if (argc < 5)
		{
			if (vm.count("help"))
			{
				std::cout << "Command Line Parameter App" << std::endl << desc << std::endl;
			}
			return E_FAIL;
		}
		else
		{
			if (vm.count("help"))
			{
				std::cout << "Command Line Parameter App" << std::endl << desc << std::endl;
				return S_OK;
			}
			if (vm.count("cred"))
			{
				wstrCred = vm["cred"].as<std::string>();
			}

			if (vm.count("files") > 0)
			{
				wvecfiles = vm["files"].as<std::vector<std::string> >();
			}

		}
	}
	catch (std::exception& e)
	{
		BOOST_LOG_TRIVIAL(error) << __FILE__ << ", unable to parse command line:" << e.what();
		return E_FAIL;
	}

	return E_START;
}
