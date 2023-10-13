/*
 * Copyright (C) 2008-2018 TrinityCore <https://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
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

#include "UpdateData.h"
#include "Errors.h"
#include "WorldPacket.h"
#include "Opcodes.h"

UpdateData::UpdateData(uint32 map) : _map(map), _blockCount(0) { }

// the guids out of player's sight
void UpdateData::AddOutOfRangeGUID(GuidSet& guids)
{
    _outOfRangeGUIDs.insert(guids.begin(), guids.end());
}

void UpdateData::AddOutOfRangeGUID(ObjectGuid guid)
{
    _outOfRangeGUIDs.insert(guid);
}

// 添加一份数据块到UpdateData中...
void UpdateData::AddUpdateBlock(const ByteBuffer &block)
{
    _data.append(block);
    ++_blockCount;
}

// 将updateData的数据复制到消息包中
bool UpdateData::BuildPacket(WorldPacket* packet)
{
    ASSERT(packet->empty());                                // shouldn't happen
    packet->Initialize(SMSG_UPDATE_OBJECT, 2 + 4 + (_outOfRangeGUIDs.empty() ? 0 : 1 + 4 + 9 * _outOfRangeGUIDs.size()) + _data.wpos());

    // 这个消息包定义的真是奇葩，_blockCount在最前面，实际的数据在最后面，中间插入了要被删除的ojbect guid列表...

    *packet << uint32(_blockCount);
    *packet << uint16(_map);

    if (packet->WriteBit(!_outOfRangeGUIDs.empty()))
    {
        *packet << uint16(0);   // object limit to instantly destroy - objects before this index on _outOfRangeGUIDs list get "smoothly phased out"
        *packet << uint32(_outOfRangeGUIDs.size());

        for (GuidSet::const_iterator i = _outOfRangeGUIDs.begin(); i != _outOfRangeGUIDs.end(); ++i)
            *packet << *i;
    }

    *packet << uint32(_data.size());
    packet->append(_data);
    return true;
}

void UpdateData::Clear()
{
    _data.clear();
    _outOfRangeGUIDs.clear();
    _blockCount = 0;
    _map = 0;
}
