﻿#pragma once

#include "Objects.h"
#include "Utility.h"

namespace Gurigi
{
    namespace Resources
    {
        class ResourceID
        {
        private:
            union
            {
                uint16_t id;
                const wchar_t *name;
            };
            bool allocated;

        public:
            ResourceID(uint16_t iid)
                : id(iid), allocated(false)
            {}

            ResourceID(const wchar_t *iname)
                : name(iname), allocated(false)
            {}

            ResourceID(const std::wstring &);
            ResourceID(const ResourceID &);
            ResourceID(ResourceID &&);

            ~ResourceID()
            {
                if(allocated)
                    delete [] name;
            }

        public:
            ResourceID &operator =(const ResourceID &);
            ResourceID &operator =(ResourceID &&);

            operator uint16_t() const
            {
                return id;
            }

            operator const wchar_t *() const
            {
                return name;
            }

            friend bool operator ==(const ResourceID &, const ResourceID &);
            friend bool operator !=(const ResourceID &, const ResourceID &);
        };

        class ResourceHelper
        {
        private:
            ~ResourceHelper(); // = delete;

        public:
            static HINSTANCE findInstanceByResourceID(const ResourceID &, const ResourceID &);
        };

        class Resource;

        struct ResourcePtrDeleter
        {
            void operator ()(Resource *) const;
        };

        template<typename = Resource>
        class ResourcePtr;

        /*template<typename T>
        class ResourcePtr : public std::shared_ptr<T>
        {
            static_assert(std::is_convertible<T *, Resource *>::value, "T must be a derived class from Maragi::UI::Resources::Resource.");

        public:
            ResourcePtr()
                : std::shared_ptr<T>(nullptr, ResourcePtrDeleter())
            {}

            explicit ResourcePtr(T *ptr)
                : std::shared_ptr<T>(ptr, ResourcePtrDeleter())
            {}

            ResourcePtr(nullptr_t)
                : std::shared_ptr<T>(nullptr, ResourcePtrDeleter())
            {}

            template<typename Other>
            ResourcePtr(const ResourcePtr<Other> &that)
                : std::shared_ptr<T>(that)
            {}

            template<typename Other>
            ResourcePtr(ResourcePtr<Other> &&that)
                : std::shared_ptr<T>(std::forward<ResourcePtr<Other>>(that))
            {}
        };*/

        template<typename T>
        class ResourcePtr : public SharedPtr<Resource, ResourcePtrDeleter, T>
        {
        public:
            ResourcePtr()
                : SharedPtr<Resource, ResourcePtrDeleter, T>()
            {}

            ResourcePtr(Resource *iptr)
                : SharedPtr<Resource, ResourcePtrDeleter, T>(iptr)
            {}

            template<typename Other>
            ResourcePtr(const ResourcePtr<Other> &that)
                : SharedPtr<Resource, ResourcePtrDeleter, T>(that)
            {}

            template<typename Other>
            ResourcePtr(ResourcePtr<Other> &&that)
                : SharedPtr<Resource, ResourcePtrDeleter, T>(std::move(that))
            {}

            ResourcePtr(const std::shared_ptr<Resource> &iptr)
                : SharedPtr<Resource, ResourcePtrDeleter, T>(iptr)
            {}

            ResourcePtr(std::shared_ptr<Resource> &&iptr)
                : SharedPtr<Resource, ResourcePtrDeleter, T>(std::move(iptr))
            {}

            friend class Resource;
            template<typename Other>
            friend class ResourcePtr;
        };

        class Resource
        {
        protected:
            virtual ~Resource() = 0
            {}

        protected:
            Resource &operator =(const Resource &); // = delete;

            friend struct ResourcePtrDeleter;
        };

        class Icon : public Resource
        {
        private:
            HICON icon;
            bool shared;

        protected:
            Icon();
            Icon(HICON, bool);
            virtual ~Icon();

        public:
            static ResourcePtr<Icon> fromResource(const ResourceID &);
            static ResourcePtr<Icon> fromResource(const ResourceID &, const Objects::SizeI &);
            static ResourcePtr<Icon> fromSharedResource(const ResourceID &);
            static ResourcePtr<Icon> fromSharedResource(const ResourceID &, const Objects::SizeI &);
            static ResourcePtr<Icon> fromFile(const std::wstring &);
            static ResourcePtr<Icon> fromFile(const std::wstring &, const Objects::SizeI &);

        public:
            HICON getIcon() const
            {
                return icon;
            }

            operator HICON() const
            {
                return icon;
            }
        };

        class Cursor : public Resource
        {
        private:
            HCURSOR cursor;

        public:
            struct Predefined
            {
                static const ResourcePtr<Cursor> &arrow();
                static const ResourcePtr<Cursor> &ibeam();
                static const ResourcePtr<Cursor> &wait();
                static const ResourcePtr<Cursor> &hand();
                static const ResourcePtr<Cursor> &help();
            };

        protected:
            Cursor();
            Cursor(HCURSOR);
            virtual ~Cursor();

        public:
            static ResourcePtr<Cursor> fromResource(const ResourceID &);
            static ResourcePtr<Cursor> fromFile(const std::wstring &);

        public:
            HCURSOR getCursor() const
            {
                return cursor;
            }

            operator HCURSOR() const
            {
                return cursor;
            }
        };
    }
}