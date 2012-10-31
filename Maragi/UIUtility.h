// $Id$

#pragma once

#include "Singleton.h"
#include "Window.h"

namespace Maragi
{
	namespace UI
	{
		typename<template = Shell>
		class ShellPtr;

		template<typename T>
		class ShellPtr
		{
		};

		template<typename = Control>
		class ControlPtr;

		template<typename T>
		class ControlPtr
		{
		private:
			std::shared_ptr<Control> ptr;
			T *castPtr;

		public:
			ControlPtr()
				: castPtr(nullptr)
			{
			}

			ControlPtr(Control *iptr)
			{
				if(iptr == nullptr)
					castPtr = nullptr;
				else
				{
					castPtr = dynamic_cast<T *>(iptr);
					ptr = std::shared_ptr<Control>(iptr);
				}
			}

			ControlPtr(std::shared_ptr<Control> iptr)
				: ptr(iptr)
			{
				castPtr = dynamic_cast<T *>(ptr.get());
			}

			template<typename Other>
			ControlPtr(const ControlPtr<Other> &that)
				: ptr(that.ptr)
			{
				castPtr = dynamic_cast<T *>(ptr.get());
			}

		public:
			bool operator ==(const ControlPtr &rhs)
			{
				return castPtr == rhs.castPtr;
			}

			bool operator !=(const ControlPtr &rhs)
			{
				return castPtr != rhs.castPtr;
			}

			bool operator <(const ControlPtr &rhs)
			{
				return castPtr < rhs.castPtr;
			}

			bool operator >(const ControlPtr &rhs)
			{
				return castPtr > rhs.castPtr;
			}

			bool operator <=(const ControlPtr &rhs)
			{
				return castPtr <= rhs.castPtr;
			}

			bool operator >=(const ControlPtr &rhs)
			{
				return castPtr >= rhs.castPtr;
			}

			T *operator ->()
			{
				return castPtr;
			}

			const T *operator ->() const
			{
				return castPtr;
			}

			operator bool() const
			{
				return castPtr != nullptr;
			}

			bool operator !() const
			{
				return castPtr == nullptr;
			}
		};

		class ShellManager : public Singleton<ShellManager>
		{
		private:
			std::map<HWND, Shell *> shells;

		public:
			ShellManager();

		private:
			~ShellManager();

		private:

			friend class Shell;
			friend class Singleton<ShellManager>;
		};

		class ControlManager : public Singleton<ControlManager>
		{
		private:
			std::map<ControlID, Control *> controls;

		private:
			ControlID nextID;

		public:
			ControlManager();

		private:
			~ControlManager();

		private:
			ControlID getNextID();
			void add(Control *);
			Control *find(ControlID);
			void remove(ControlID);

			friend class Control;
			friend class Singleton<ControlManager>;
		};
	}
}