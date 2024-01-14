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

#include "TokenPackets.h"
#include "WorldSession.h"

void WorldSession::HandleUpdateListedAuctionableTokens(WorldPackets::Token::UpdateListedAuctionableTokens& updateListedAuctionableTokens)
{
    WorldPackets::Token::UpdateListedAuctionableTokensResponse response;

    /// @todo: 6.x fix implementation
    response.UnkInt = updateListedAuctionableTokens.UnkInt;
    response.Result = TOKEN_RESULT_SUCCESS;

    SendPacket(response.Write());
}

void WorldSession::HandleRequestWowTokenMarketPrice(WorldPackets::Token::RequestWowTokenMarketPrice& requestWowTokenMarketPrice)
{
    WorldPackets::Token::WowTokenMarketPriceResponse response;

    /// @todo: 6.x fix implementation
    response.CurrentMarketPrice = 300000000;
    
    response.Result = /*TOKEN_RESULT_SUCCESS*/TOKEN_RESULT_ERROR_DISABLED;

    response.UnkInt = requestWowTokenMarketPrice.UnkInt;
    response.AuctionDuration = 14400;   //packet.ReadUInt32("UnkInt32");


    SendPacket(response.Write());
}

void WorldSession::HandleBuyWowTokenStart(WorldPackets::Token::WowTokenBuyStart& /*wowTokenBuyStart*/)
{
    // TODO:
}

void WorldSession::HandleCheckVeteranTokenEligibility(WorldPackets::Token::CheckVeteranTokenEligibility & packet)
{
    WorldPackets::Token::WowTokenCanVeteranBuyResult result;
    result.UnkLong = 0;
    result.UnkInt = packet.UnkInt;
    result.UnkInt2 = 1;
    SendPacket(result.Write());
}
