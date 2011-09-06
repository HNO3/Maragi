// $Id$

#pragma once

#include "Singleton.h"
#include "MainWindow.h"

namespace Maragi
{
	class MainController : public Singleton<MainController>
	{
	private:
		unsigned exceptionCode;
		EXCEPTION_POINTERS exceptionPointers;
		HINSTANCE appInst;
		MainWindow *mainWin;

	public:
		MainController();
		~MainController();

	public:
		bool run(HINSTANCE, const std::wstring &, int);
		bool runImpl(HINSTANCE, const std::wstring &, int);

	private:
		void registerEvents();
		int filterOSException(unsigned, EXCEPTION_POINTERS *);
		bool showOSException();
		bool checkPrerequisites();
		void processCommandLine(const std::wstring &);
	};
}
