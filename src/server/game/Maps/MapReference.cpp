/*
 * Copyright (C) 2008-2018 TrinityCore <https://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "MapReference.h"
#include "Map.h"

void MapReference::targetObjectBuildLink()
{
    // called from link()
    getTarget()->_mapRefManager.insertFirst(this);
    getTarget()->_mapRefManager.incSize();
}

void MapReference::targetObjectDestroyLink()
{
    // called from unlink()
    if (isValid())
        getTarget()->_mapRefManager.decSize();
}

void MapReference::sourceObjectDestroyLink()
{
    // called from invalidate()
    getTarget()->_mapRefManager.decSize();
}
