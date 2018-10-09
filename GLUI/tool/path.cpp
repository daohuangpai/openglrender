#include "path.h"


namespace PATH
{

	std::string modulefilename()
	{
		std::string result;
		int pahlength = MAX_PATH;
		result.resize(pahlength + 1);

		DWORD realpathlength = GetModuleFileName(NULL, &result[0], pahlength);
		if (realpathlength == 0)
			return "";

		if (realpathlength > pahlength) {
			pahlength = realpathlength;
			result.resize(pahlength + 1);
			realpathlength = GetModuleFileName(NULL, &result[0], pahlength);
			if (realpathlength == 0)
				return "";
		}
		return result;
	}

	std::string moduledirectory()
	{
		std::string modulepath = modulefilename();
		if (modulepath.size() == 0)
			return "";

		int findpathendsinal1 = (int)modulepath.find_last_of('\\');
		int findpathendsinal2 = (int)modulepath.find_last_of('/');
		int pathrootpos = max(findpathendsinal1, findpathendsinal2);
		std::string rootpath = (pathrootpos != std::string::npos) ? modulepath.substr(0, pathrootpos + 1) : "";
		return rootpath;
	}

	std::string shaderpath()
	{
		std::string ret = moduledirectory();
		if (ret.size() == 0)
			return "";

		ret.append("shader/");
		return ret;
	}

	std::string imagepath()
	{
		std::string ret = moduledirectory();
		if (ret.size() == 0)
			return "";

		ret.append("image/");
		return ret;
	}

}