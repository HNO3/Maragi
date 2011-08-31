// $Id$

#pragma once

namespace Maragi
{
	template<typename T>
	class Singleton
	{
	protected:
		~Singleton() {}

	public:
		static T &instance()
		{
			static T theInstance;
			return theInstance;
		}
		static T &inst() { return instance(); }
	};
}
