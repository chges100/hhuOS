/*
 * Copyright (C) 2018  Filip Krakowski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __Set_include__
#define __Set_include__

#include "Collection.h"

namespace Util {

    /**
     * Base interface for all sets.
     *
     * @author Filip Krakowski
     */
    template <typename T>
    class Set : public Collection<T> {

    public:

        virtual bool add(const T &element) = 0;

        virtual bool addAll(const Collection<T> &other) = 0;

        virtual bool remove(const T &element) = 0;

        virtual bool removeAll(const Collection<T> &other) = 0;

        virtual bool contains(const T &element) const = 0;

        virtual bool containsAll(const Collection<T> &other) const = 0;

        virtual bool isEmpty() const = 0;

        virtual void clear() = 0;

        virtual Iterator<T> begin() const = 0;

        virtual Iterator<T> end() const = 0;

        virtual uint32_t size() const = 0;

        virtual Array<T> toArray() const = 0;

    };
}



#endif
