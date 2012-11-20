// $Id$

#include "Common.h"

#include "Layouts.h"

namespace Maragi
{
	namespace UI
	{
		Layout::Layout(const ControlID &id)
			: Control(id)
		{
		}

		ShellLayout::ShellLayout(const ShellWeakPtr<> &ishell, const ControlID &id)
			: Layout(id)
		{
			shell(ishell);
		}

		ShellLayout::~ShellLayout()
		{
		}

		ControlPtr<ShellLayout> ShellLayout::create(
			const ShellWeakPtr<> &shell
			)
		{
			ControlPtr<ShellLayout> layout(new ShellLayout(shell, ControlManager::instance().getNextID()));
			layout->slot_.parent(layout);
			return layout;
		}

		void ShellLayout::createDrawingResources(Drawing::Context &ctx)
		{
			ControlPtr<> lchild = slot_.child().lock();
			if(lchild)
				lchild->createDrawingResources(ctx);
		}

		void ShellLayout::discardDrawingResources(Drawing::Context &ctx)
		{
			ControlPtr<> lchild = slot_.child().lock();
			if(lchild)
				lchild->discardDrawingResources(ctx);
		}

		void ShellLayout::draw(Drawing::Context &ctx)
		{
			ControlPtr<> lchild = slot_.child().lock();
			if(lchild)
				lchild->draw(ctx);
		}

		Objects::SizeF ShellLayout::computeSize()
		{
			ControlPtr<> lchild = slot_.child().lock();
			if(lchild)
				return lchild->computeSize();
			return Objects::SizeF();
		}

		ControlWeakPtr<> ShellLayout::findByPoint(const Objects::PointF &pt)
		{
			ControlPtr<> lchild = slot_.child().lock();
			if(lchild)
				return lchild->findByPoint(pt);
			return nullptr;
		}

		std::vector<ControlWeakPtr<>> ShellLayout::findTreeByPoint(const Objects::PointF &pt)
		{
			if(!rect().isIn(pt))
				return std::vector<ControlWeakPtr<>>();

			ControlPtr<> lchild = slot_.child().lock();
			if(lchild)
			{
				std::vector<ControlWeakPtr<>> ve = lchild->findTreeByPoint(pt);
				ve.insert(ve.begin(), sharedFromThis());
				return ve;
			}
			return std::vector<ControlWeakPtr<>>(1, sharedFromThis());
		}

		std::vector<ControlWeakPtr<>> ShellLayout::findReverseTreeByPoint(const Objects::PointF &pt)
		{
			if(!rect().isIn(pt))
				return std::vector<ControlWeakPtr<>>();

			ControlPtr<> lchild = slot_.child().lock();
			if(lchild)
			{
				std::vector<ControlWeakPtr<>> ve = lchild->findReverseTreeByPoint(pt);
				ve.push_back(sharedFromThis());
				return ve;
			}
			return std::vector<ControlWeakPtr<>>(1, sharedFromThis());
		}

		void ShellLayout::walk(const std::function<void (const ControlWeakPtr<> &)> &fn)
		{
			fn(sharedFromThis());
			ControlPtr<> lchild = slot_.child().lock();
			if(lchild)
				lchild->walk(fn);
		}

		void ShellLayout::walkReverse(const std::function<void (const ControlWeakPtr<> &)> &fn)
		{
			ControlPtr<> lchild = slot_.child().lock();
			if(lchild)
				lchild->walkReverse(fn);
			fn(sharedFromThis());
		}

		void ShellLayout::onResizeInternal(const Objects::RectangleF &rect)
		{
			ControlPtr<> lchild = slot_.child().lock();
			lchild->rect(rect);
		}

		Slot *ShellLayout::slot()
		{
			return &slot_;
		}
	}
}
