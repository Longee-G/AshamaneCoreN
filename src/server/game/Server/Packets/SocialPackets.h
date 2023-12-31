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

#ifndef SocialPackets_h__
#define SocialPackets_h__

#include "Packet.h"
#include "ObjectGuid.h"
#include "SharedDefines.h"

struct FriendInfo;
enum FriendsResult : uint8;

namespace WorldPackets
{
    namespace Social
    {
        class SendContactList final : public ClientPacket
        {
        public:
            SendContactList(WorldPacket&& packet) : ClientPacket(CMSG_SEND_CONTACT_LIST, std::move(packet)) { }

            void Read() override;

            uint32 Flags = 0; ///< @see enum SocialFlag
        };

        struct ContactInfo
        {
            ContactInfo(ObjectGuid const& guid, FriendInfo const& friendInfo);

            ObjectGuid Guid;
            ObjectGuid WowAccountGuid;
            uint32 VirtualRealmAddr = 0;
            uint32 NativeRealmAddr  = 0;
            uint32 TypeFlags        = 0; ///< @see enum SocialFlag
            std::string Notes;
            uint8 Status            = 0; ///< @see enum FriendStatus
            uint32 AreaID           = 0;
            uint32 Level            = 0;
            uint32 ClassID          = CLASS_NONE;
        };

        class ContactList final : public ServerPacket
        {
        public:
            ContactList() : ServerPacket(SMSG_CONTACT_LIST, 8) { }

            WorldPacket const* Write() override;

            std::vector<ContactInfo> Contacts;
            uint32 Flags = 0; ///< @see enum SocialFlag
        };

        class FriendStatus final : public ServerPacket
        {
        public:
            FriendStatus() : ServerPacket(SMSG_FRIEND_STATUS, 38) { }

            void Initialize(ObjectGuid const& guid, FriendsResult result, FriendInfo const& friendInfo);

            WorldPacket const* Write() override;

            uint32 VirtualRealmAddress = 0;
            std::string Notes;
            uint32 ClassID             = CLASS_NONE;
            uint8 Status               = 0; ///< @see enum FriendStatus
            ObjectGuid Guid;
            ObjectGuid WowAccountGuid;
            uint32 Level               = 0;
            uint32 AreaID              = 0;
            uint8 FriendResult         = 0; ///< @see enum FriendsResult
        };

        struct QualifiedGUID
        {
            ObjectGuid Guid;
            uint32 VirtualRealmAddress = 0;
        };

        class AddFriend final : public ClientPacket
        {
        public:
            AddFriend(WorldPacket&& packet) : ClientPacket(CMSG_ADD_FRIEND, std::move(packet)) { }

            void Read() override;

            std::string Notes;
            std::string Name;
        };

        class DelFriend final : public ClientPacket
        {
        public:
            DelFriend(WorldPacket&& packet) : ClientPacket(CMSG_DEL_FRIEND, std::move(packet)) { }

            void Read() override;

            QualifiedGUID Player;
        };

        class SetContactNotes final : public ClientPacket
        {
        public:
            SetContactNotes(WorldPacket&& packet) : ClientPacket(CMSG_SET_CONTACT_NOTES, std::move(packet)) { }

            void Read() override;

            QualifiedGUID Player;
            std::string Notes;
        };

        class AddIgnore final : public ClientPacket
        {
        public:
            AddIgnore(WorldPacket&& packet) : ClientPacket(CMSG_ADD_IGNORE, std::move(packet)) { }

            void Read() override;

            std::string Name;
        };

        class DelIgnore final : public ClientPacket
        {
        public:
            DelIgnore(WorldPacket&& packet) : ClientPacket(CMSG_DEL_IGNORE, std::move(packet)) { }

            void Read() override;

            QualifiedGUID Player;
        };

        // <WIP>
        class QuickJoinAutoAcceptRequests final : public ClientPacket
        {
        public:
            QuickJoinAutoAcceptRequests(WorldPacket&& packet) : ClientPacket(CMSG_QUICK_JOIN_AUTO_ACCEPT_REQUESTS, std::move(packet)) { }

            void Read() override;

            bool EnableAutoAccept = false;
        };

        class QuickJoinRequestInvite final : public ClientPacket
        {
        public:
            QuickJoinRequestInvite(WorldPacket&& packet) : ClientPacket(CMSG_QUICK_JOIN_REQUEST_INVITE, std::move(packet)) { }

            void Read() override;

            ObjectGuid GroupGUID;
            uint32 UnkInt1 = 0;
            uint32 UnkInt2 = 0;
            std::string UnkString1;
            std::string UnkString2;
            bool ApplyAsTank = false;
            bool ApplyAsHealer = false;
            bool ApplyAsDamage = false;
        };

        class QuickJoinRespondToInvite final : public ClientPacket
        {
        public:
            QuickJoinRespondToInvite(WorldPacket&& packet) : ClientPacket(CMSG_QUICK_JOIN_RESPOND_TO_INVITE, std::move(packet)) { }

            void Read() override;

            ObjectGuid GroupGUID;
            ObjectGuid GUID;
            bool Accept = false;
        };

        class QuickJoinSignalToastDisplayed final : public ClientPacket
        {
        public:
            QuickJoinSignalToastDisplayed(WorldPacket&& packet) : ClientPacket(CMSG_QUICK_JOIN_SIGNAL_TOAST_DISPLAYED, std::move(packet)) { }

            void Read() override;

            std::vector<ObjectGuid> UnkGuids;
            ObjectGuid GroupGUID;
            uint32 Priority = 0;
            bool UnkBit1 = false;
            bool UnkBit2 = false;
        };
    }
}

#endif // SocialPackets_h__
