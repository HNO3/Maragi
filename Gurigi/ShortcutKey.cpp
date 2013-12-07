﻿#include "Common.h"

#include "ShortcutKey.h"

using namespace std;

namespace Gurigi
{
    ShortcutKey::ShortcutKey()
    {
    }

    ShortcutKey::~ShortcutKey()
    {
    }

    ShortcutKey::Modifier ShortcutKey::getModifier(bool ctrl, bool alt, bool shift)
    {
        return static_cast<Modifier>(
            (ctrl ? Ctrl : 0)
            | (alt ? Alt : 0)
            | (shift ? Shift : 0));
    }

    namespace
    {
        bool isNormalKey(uint32_t key)
        {
            // TODO: check key is normal (can make character)
            return false;
        }
    }

    bool ShortcutKey::verifyKey(Modifier modifier, uint32_t key)
    {
        if(modifier == None && isNormalKey(key))
            return false;
        else if(modifier == Shift && isNormalKey(key))
            return false;
        return true;
    }

    ShortcutKey::Key ShortcutKey::makeKey(Modifier modifier, uint32_t key)
    {
        if(!verifyKey(modifier, key))
            ; // TODO: reject
        Key skey = {modifier, key};
        return skey;
    }

    void ShortcutKey::load(const std::vector<uint8_t> &dataVec)
    {
        if(dataVec.empty())
            return;

        const ConfigureData *data = reinterpret_cast<const ConfigureData *>(&*dataVec.begin());
    }

    // Extended key: other side's key (e.g. numpad keys, right-hand control keys, ...)
    bool ShortcutKey::addShortcut(const Shell *window, uint32_t id, Key key, bool canExtended, bool canRepeated)
    {
        Command cmd;
        cmd.canExtended = canExtended;
        cmd.canRepeated = canRepeated;
        cmd.id = id;

        return keyMap.insert(make_pair(make_pair(key, window), cmd)).second;
    }

    bool ShortcutKey::modifyShortcut(const Shell *window, uint32_t id, Key key, bool canExtended, bool canRepeated)
    {
        if(!removeShortcut(window, id))
            return false;
        return addShortcut(window, id, key, canExtended, canRepeated);
    }

    bool ShortcutKey::removeShortcut(const Shell *window, uint32_t id)
    {
        for(auto it = keyMap.begin(); it != keyMap.end(); ++ it)
        {
            if(it->first.second == window && it->second.id == id)
            {
                keyMap.erase(it);
                return true;
            }
        }

        return false;
    }

    uint32_t ShortcutKey::processShortcut(const Shell *window, Key key, bool extended, bool repeated) const
    {
        auto it = keyMap.find(make_pair(key, window));
        if(it == keyMap.end())
            return 0;

        if((extended && !it->second.canExtended) || (repeated && !it->second.canRepeated))
            return 0;

        return it->second.id;
    }

    bool ShortcutKey::processKey(const Shell *window, bool ctrl, bool alt, bool shift, uint32_t keycode, bool extended, uint32_t repeat) const
    {
        Key key;
        key.modifier = getModifier(ctrl, alt, shift);
        key.key = keycode;

        uint32_t id = processShortcut(window, key, extended, repeat > 1);
        if(id == 0)
            return false;

        return PostMessageW(window->hwnd(), WM_COMMAND, id, 0) != FALSE;
    }
}