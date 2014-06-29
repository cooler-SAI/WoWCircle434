using System;
using System.Collections.Generic;
using WowPacketParser.Enums;
using WowPacketParser.Misc;
using WowPacketParser.Store;
using WowPacketParser.Store.Objects;

namespace WowPacketParser.Parsing.Parsers
{
    public static class ActionBarHandler
    {

        [Parser(Opcode.CMSG_SET_ACTIONBAR_TOGGLES)]
        public static void HandleSetActionBarToggles(Packet packet)
        {
            packet.ReadByte("Action Bar");
        }
    }
}
