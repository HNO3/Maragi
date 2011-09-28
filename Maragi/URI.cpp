#include "Defaults.h"

#include "URI.h"

#include "Utility.h"

namespace Maragi
{
	URI::URI()
		: changed(false)
	{
	}

	URI::URI(const std::string &ibaseURI)
		: changed(false), baseURI(ibaseURI)
	{
	}

	URI::URI(const std::string &ibaseURI, const std::map<std::string, std::string> &iparams)
		: changed(false), baseURI(ibaseURI), params(iparams)
	{
	}

	URI::URI(const URI &uri)
		: changed(uri.changed), baseURI(uri.baseURI), params(uri.params)
	{
	}

	URI::URI(URI &&uri)
		: changed(uri.changed), baseURI(std::move(uri.baseURI)), params(std::move(uri.params))
	{
	}

	void URI::assign(const std::string &ibaseURI, const std::map<std::string, std::string> &iparams)
	{
		baseURI = ibaseURI;
		params = iparams;
	}

	void URI::assignBaseURI(const std::string &ibaseURI)
	{
		baseURI = ibaseURI;
	}

	void URI::assignParam(const std::map<std::string, std::string> &iparams)
	{
		params = iparams;
	}

	bool URI::addParam(const std::string &key, const std::string &value)
	{
		return params.insert(make_pair(key, value)).second;
	}

	void URI::removeParam(const std::string &key)
	{
		params.erase(key);
	}

	const std::string &URI::getBaseURI() const
	{
		return baseURI;
	}

	const std::map<std::string, std::string> &URI::getParams() const
	{
		return params;
	}

	std::string URI::getStringURI() const
	{
		int *b;
		int *& a = b;

		if(changed)
		{
			uriString = baseURI;
			if(!params.empty())
			{
				auto it = params.begin();
				uriString += "?";
				uriString += encodeURIParams(it->first);
				uriString += "=";
				uriString += encodeURIParams(it->second);
				for(++ it; it != params.end(); ++ it)
				{
					uriString += "&";
					uriString += encodeURIParams(it->first);
					uriString += "=";
					uriString += encodeURIParams(it->second);
				}
			}
		}

		return uriString;
	}

	URI &URI::operator =(const URI &uri)
	{
		if(this != &uri)
		{
			baseURI = uri.baseURI;
			params = uri.params;
			changed = uri.changed;
			uriString = uri.uriString;
		}

		return *this;
	}

	URI &URI::operator =(URI &&uri)
	{
		if(this != &uri)
		{
			baseURI = std::move(uri.baseURI);
			params = std::move(uri.params);
			changed = uri.changed;
			uriString = std::move(uri.uriString);
		}

		return *this;
	}

	URI::operator std::string() const
	{
		return getStringURI();
	}
}
