using System;
using System.Collections.Generic;
using WowPacketParser.Enums;
using WowPacketParser.Enums.Version;
using WowPacketParser.Misc;
using WowPacketParser.Store;
using WowPacketParser.Store.Objects;

namespace WowPacketParser.Parsing.Parsers
{
    public static class QuestHandler
    {
        private static void ReadExtraQuestInfo510(ref Packet packet, bool readFlags = true)
        {
            packet.ReadUInt32("Choice Item Count");
            for (var i = 0; i < 6; i++)
            {
                packet.ReadEntryWithName<UInt32>(StoreNameType.Item, "Choice Item Id", i);
                packet.ReadUInt32("Choice Item Count", i);
                packet.ReadUInt32("Choice Item Display Id", i);
            }

            packet.ReadUInt32("Reward Item Count");

            for (var i = 0; i < 4; i++)
                packet.ReadEntryWithName<UInt32>(StoreNameType.Item, "Reward Item Id", i);
            for (var i = 0; i < 4; i++)
                packet.ReadUInt32("Reward Item Count", i);
            for (var i = 0; i < 4; i++)
                packet.ReadUInt32("Reward Item Display Id", i);

            packet.ReadUInt32("Money");
            packet.ReadUInt32("XP");
            packet.ReadUInt32("Title Id");
            packet.ReadUInt32("Bonus Talents");
            packet.ReadUInt32("Reward Reputation Mask");

            for (var i = 0; i < 5; i++)
                packet.ReadUInt32("Reputation Faction", i);
            for (var i = 0; i < 5; i++)
                packet.ReadUInt32("Reputation Value Id", i);
            for (var i = 0; i < 5; i++)
                packet.ReadInt32("Reputation Value", i);

            packet.ReadEntryWithName<Int32>(StoreNameType.Spell, "Spell Id");
            packet.ReadEntryWithName<Int32>(StoreNameType.Spell, "Spell Cast Id");

            for (var i = 0; i < 4; i++)
                packet.ReadUInt32("Currency Id", i);
            for (var i = 0; i < 4; i++)
                packet.ReadUInt32("Currency Count", i);

            packet.ReadUInt32("Reward SkillId");
            packet.ReadUInt32("Reward Skill Points");
        }

        private static void ReadExtraQuestInfo(ref Packet packet, bool readFlags = true)
        {
            var choiceCount = packet.ReadUInt32("Choice Item Count");
            var effectiveChoiceCount = ClientVersion.AddedInVersion(ClientVersionBuild.V4_0_1_13164) ? 6 : choiceCount;

            if (ClientVersion.AddedInVersion(ClientVersionBuild.V4_0_1_13164))
            {
                for (var i = 0; i < effectiveChoiceCount; i++)
                    packet.ReadEntryWithName<UInt32>(StoreNameType.Item, "Choice Item Id", i);
                for (var i = 0; i < effectiveChoiceCount; i++)
                    packet.ReadUInt32("Choice Item Count", i);
                for (var i = 0; i < effectiveChoiceCount; i++)
                    packet.ReadUInt32("Choice Item Display Id", i);

                packet.ReadUInt32("Reward Item Count");
                const int effectiveRewardCount = 4;

                for (var i = 0; i < effectiveRewardCount; i++)
                    packet.ReadEntryWithName<UInt32>(StoreNameType.Item, "Reward Item Id", i);
                for (var i = 0; i < effectiveRewardCount; i++)
                    packet.ReadUInt32("Reward Item Count", i);
                for (var i = 0; i < effectiveRewardCount; i++)
                    packet.ReadUInt32("Reward Item Display Id", i);
            }
            else
            {
                for (var i = 0; i < choiceCount; i++)
                {
                    packet.ReadEntryWithName<UInt32>(StoreNameType.Item, "Choice Item Id", i);
                    packet.ReadUInt32("Choice Item Count", i);
                    packet.ReadUInt32("Choice Item Display Id", i);
                }

                var rewardCount = packet.ReadUInt32("Reward Item Count");
                for (var i = 0; i < rewardCount; i++)
                {
                    packet.ReadEntryWithName<UInt32>(StoreNameType.Item, "Reward Item Id", i);
                    packet.ReadUInt32("Reward Item Count", i);
                    packet.ReadUInt32("Reward Item Display Id", i);
                }
            }

            packet.ReadUInt32("Money");

            if (ClientVersion.AddedInVersion(ClientVersionBuild.V3_2_2_10482))
                packet.ReadUInt32("XP");

            if (ClientVersion.AddedInVersion(ClientVersionBuild.V4_0_1_13164))
            {
                packet.ReadUInt32("Title Id");
                packet.ReadUInt32("Unknown UInt32 1");
                packet.ReadSingle("Unknown float");
                packet.ReadUInt32("Bonus Talents");
                packet.ReadUInt32("Unknown UInt32 2");
                packet.ReadUInt32("Reward Reputation Mask");
            }
            else
            {
                packet.ReadUInt32("Honor Points");

                if (ClientVersion.AddedInVersion(ClientVersionBuild.V3_3_0_10958))
                    packet.ReadSingle("Honor Multiplier");

                if (readFlags)
                        packet.ReadEnum<QuestFlags>("Quest Flags", TypeCode.UInt32);

                packet.ReadEntryWithName<Int32>(StoreNameType.Spell, "Spell Id");
                packet.ReadEntryWithName<Int32>(StoreNameType.Spell, "Spell Cast Id");
                packet.ReadUInt32("Title Id");

                if (ClientVersion.AddedInVersion(ClientVersionBuild.V3_0_2_9056))
                    packet.ReadUInt32("Bonus Talents");

                if (ClientVersion.AddedInVersion(ClientVersionBuild.V3_3_0_10958))
                {
                    packet.ReadUInt32("Arena Points");
                    packet.ReadUInt32("Unk UInt32");
                }
            }

            if (ClientVersion.AddedInVersion(ClientVersionBuild.V3_3_0_10958))
            {
                for (var i = 0; i < 5; i++)
                    packet.ReadUInt32("Reputation Faction", i);

                for (var i = 0; i < 5; i++)
                    packet.ReadUInt32("Reputation Value Id", i);

                for (var i = 0; i < 5; i++)
                    packet.ReadInt32("Reputation Value", i);
            }

            if (ClientVersion.AddedInVersion(ClientVersionBuild.V4_0_1_13164))
            {
                packet.ReadEntryWithName<Int32>(StoreNameType.Spell, "Spell Id");
                packet.ReadEntryWithName<Int32>(StoreNameType.Spell, "Spell Cast Id");

                for (var i = 0; i < 4; i++)
                    packet.ReadUInt32("Currency Id", i);
                for (var i = 0; i < 4; i++)
                    packet.ReadUInt32("Currency Count", i);

                packet.ReadUInt32("Reward SkillId");
                packet.ReadUInt32("Reward Skill Points");
            }
        }

        [Parser(Opcode.CMSG_QUEST_QUERY)]
        [Parser(Opcode.CMSG_PUSHQUESTTOPARTY)]
        public static void HandleQuestQuery(Packet packet)
        {
            packet.ReadInt32("Entry");
        }

        [Parser(Opcode.CMSG_QUEST_POI_QUERY)]
        [Parser(Opcode.CMSG_QUEST_NPC_QUERY, ClientVersionBuild.Zero, ClientVersionBuild.V4_3_0_15005)]
        public static void HandleQuestPoiQuery(Packet packet)
        {
            var count = packet.ReadUInt32("Count");

            for (var i = 0; i < count; i++)
                packet.ReadEntryWithName<Int32>(StoreNameType.Quest, "Quest ID", i);
        }

        [Parser(Opcode.CMSG_QUEST_NPC_QUERY, ClientVersionBuild.V4_3_0_15005)]
        public static void HandleQuestNPCQuery430(Packet packet)
        {
            var count = packet.ReadBits("Count", 24);
            for (int i = 0; i < count; ++i)
                packet.ReadEntryWithName<UInt32>(StoreNameType.Quest, "Quest", i);
        }

        [Parser(Opcode.SMSG_QUEST_NPC_QUERY_RESPONSE, ClientVersionBuild.Zero, ClientVersionBuild.V4_3_4_15595)]
        public static void HandleQuestNpcQueryResponse(Packet packet)
        {
            var count = packet.ReadUInt32("Count");

            for (var i = 0; i < count; ++i)
            {
                var count2 = packet.ReadUInt32("Number of NPC", i);
                for (var j = 0; j < count2; ++j)
                {
                    var entry = packet.ReadEntry();
                    packet.WriteLine("[{0}] [{1}] {2}: {3}", i, j, entry.Value ? "Creature" : "GameObject",
                        StoreGetters.GetName(entry.Value ? StoreNameType.GameObject : StoreNameType.Unit, entry.Key));
                }
            }

            for (var i = 0; i < count; ++i)
                packet.ReadEntryWithName<Int32>(StoreNameType.Quest, "Quest ID", i);
        }

        [Parser(Opcode.SMSG_QUEST_NPC_QUERY_RESPONSE, ClientVersionBuild.V4_3_4_15595)]
        public static void HandleQuestNpcQueryResponse434(Packet packet)
        {
            var count = packet.ReadBits("Count", 23);
            var counts = new uint[count];

            for (int i = 0; i < count; ++i)
                counts[i] = packet.ReadBits("Count", 24, i);

            for (int i = 0; i < count; ++i)
            {
                packet.ReadEntryWithName<Int32>(StoreNameType.Quest, "Quest ID", i);
                for (int j = 0; j < counts[i]; ++j)
                {
                    var entry = packet.ReadEntry();
                    packet.WriteLine("[{0}] [{1}] {2}: {3}", i, j, entry.Value ? "GameObject" : "Creature",
                        StoreGetters.GetName(entry.Value ? StoreNameType.GameObject : StoreNameType.Unit, entry.Key));
                }
            }
        }

        [Parser(Opcode.SMSG_QUEST_POI_QUERY_RESPONSE)]
        public static void HandleQuestPoiQueryResponse(Packet packet)
        {
            var count = packet.ReadInt32("Count");

            for (var i = 0; i < count; ++i)
            {
                var questId = packet.ReadEntryWithName<Int32>(StoreNameType.Quest, "Quest ID", i);

                var counter = packet.ReadInt32("POI Counter", i);
                for (var j = 0; j < counter; ++j)
                {
                    var questPoi = new QuestPOI();

                    var idx = packet.ReadInt32("POI Index", i, j);
                    questPoi.ObjectiveIndex = packet.ReadInt32("Objective Index", i, j);

                    questPoi.Map = (uint) packet.ReadEntryWithName<UInt32>(StoreNameType.Map, "Map Id", i);
                    questPoi.WorldMapAreaId = packet.ReadUInt32("World Map Area", i, j);
                    questPoi.FloorId = packet.ReadUInt32("Floor Id", i, j);
                    questPoi.UnkInt1 = packet.ReadUInt32("Unk Int32 2", i, j);
                    questPoi.UnkInt2 = packet.ReadUInt32("Unk Int32 3", i, j);

                    var pointsSize = packet.ReadInt32("Points Counter", i, j);
                    questPoi.Points = new List<QuestPOIPoint>(pointsSize);
                    for (var k = 0u; k < pointsSize; ++k)
                    {
                        var questPoiPoint = new QuestPOIPoint
                                            {
                                                Index = k,
                                                X = packet.ReadInt32("Point X", i, j, (int) k),
                                                Y = packet.ReadInt32("Point Y", i, j, (int) k)
                                            };
                        questPoi.Points.Add(questPoiPoint);
                    }

                    Storage.QuestPOIs.Add(new Tuple<uint, uint>((uint) questId, (uint) idx), questPoi, packet.TimeSpan);
                }
            }
        }

        [Parser(Opcode.SMSG_QUEST_FORCE_REMOVE)]
        [Parser(Opcode.CMSG_QUEST_CONFIRM_ACCEPT)]
        [Parser(Opcode.SMSG_QUESTUPDATE_FAILED)]
        [Parser(Opcode.SMSG_QUESTUPDATE_FAILEDTIMER)]
        [Parser(Opcode.SMSG_QUESTUPDATE_COMPLETE, ClientVersionBuild.Zero, ClientVersionBuild.V4_2_2_14545)]
        [Parser(Opcode.SMSG_QUESTUPDATE_COMPLETE, ClientVersionBuild.V4_3_4_15595)]
        public static void HandleQuestForceRemoved(Packet packet)
        {
            packet.ReadEntryWithName<Int32>(StoreNameType.Quest, "Quest ID");
        }

        [Parser(Opcode.SMSG_QUESTUPDATE_COMPLETE, ClientVersionBuild.V4_2_2_14545, ClientVersionBuild.V4_3_4_15595)]
        public static void HandleQuestUpdateComplete422(Packet packet)
        {
            packet.ReadGuid("Guid");
            packet.ReadEntryWithName<UInt32>(StoreNameType.Quest, "Quest ID");
            packet.ReadCString("Title");
            packet.ReadCString("Complete Text");
            packet.ReadCString("QuestGiver Text Window");
            packet.ReadCString("QuestGiver Target Name");
            packet.ReadCString("QuestTurn Text Window");
            packet.ReadCString("QuestTurn Target Name");
            packet.ReadInt32("QuestGiver Portrait");
            packet.ReadInt32("QuestTurn Portrait");
            packet.ReadByte("Unk Byte");
            packet.ReadEnum<QuestFlags>("Quest Flags", TypeCode.UInt32);
            packet.ReadInt32("Unk Int32");
            var emoteCount = packet.ReadUInt32("Quest Emote Count");
            for (var i = 0; i < emoteCount; i++)
            {
                packet.ReadUInt32("Emote Id", i);
                packet.ReadUInt32("Emote Delay (ms)", i);
            }

            ReadExtraQuestInfo(ref packet);
        }

        [Parser(Opcode.SMSG_QUERY_QUESTS_COMPLETED_RESPONSE)]
        public static void HandleQuestCompletedResponse(Packet packet)
        {
            packet.ReadInt32("Count");
            // Prints ~4k lines of quest IDs, should be DEBUG only or something...
            /*
            for (var i = 0; i < count; i++)
                packet.ReadEntryWithName<Int32>(StoreNameType.Quest, "Rewarded Quest");
            */
            packet.WriteLine("Packet is currently not printed");
            packet.ReadBytes((int)packet.Length);
        }

        [Parser(Opcode.CMSG_QUESTGIVER_STATUS_QUERY)]
        [Parser(Opcode.CMSG_QUESTGIVER_HELLO)]
        [Parser(Opcode.CMSG_QUESTGIVER_QUEST_AUTOLAUNCH)]
        public static void HandleQuestgiverStatusQuery(Packet packet)
        {
            packet.ReadGuid("GUID");
        }

        [Parser(Opcode.SMSG_QUESTGIVER_QUEST_LIST)]
        public static void HandleQuestgiverQuestList(Packet packet)
        {
            packet.ReadGuid("GUID");
            packet.ReadCString("Title");
            packet.ReadUInt32("Delay");
            packet.ReadUInt32("Emote");

            var count = packet.ReadByte("Count");
            for (var i = 0; i < count; i++)
            {
                packet.ReadEntryWithName<UInt32>(StoreNameType.Quest, "Quest ID", i);
                packet.ReadUInt32("Quest Icon", i);
                packet.ReadInt32("Quest Level", i);
                packet.ReadEnum<QuestFlags>("Quest Flags", TypeCode.UInt32, i);

                packet.ReadBoolean("Change icon", i);
                packet.ReadCString("Title", i);
            }

        }

        [Parser(Opcode.CMSG_QUESTGIVER_QUERY_QUEST)]
        public static void HandleQuestgiverQueryQuest(Packet packet)
        {
            packet.ReadGuid("GUID");
            packet.ReadEntryWithName<UInt32>(StoreNameType.Quest, "Quest ID");
            packet.ReadByte("Start/End (1/2)");
        }

        [Parser(Opcode.CMSG_QUESTGIVER_ACCEPT_QUEST)]
        public static void HandleQuestgiverAcceptQuest(Packet packet)
        {
            packet.ReadGuid("GUID");
            packet.ReadEntryWithName<UInt32>(StoreNameType.Quest, "Quest ID");

            if (ClientVersion.AddedInVersion(ClientVersionBuild.V3_1_2_9901))
                packet.ReadUInt32("Unk UInt32");
        }

        [Parser(Opcode.SMSG_QUESTGIVER_QUEST_DETAILS, ClientVersionBuild.Zero)]
        public static void HandleQuestgiverDetails(Packet packet)
        {
            packet.ReadGuid("GUID1");

            packet.ReadEntryWithName<UInt32>(StoreNameType.Quest, "Quest ID");
            packet.ReadCString("Title");
            packet.ReadCString("Details");
            packet.ReadCString("Objectives");

                packet.ReadBoolean("Auto Accept", TypeCode.Int32);

            var flags = QuestFlags.None;
            flags = packet.ReadEnum<QuestFlags>("Quest Flags", TypeCode.UInt32);

            packet.ReadUInt32("Suggested Players");

            ReadExtraQuestInfo(ref packet, false);

            var emoteCount = packet.ReadUInt32("Quest Emote Count");
            for (var i = 0; i < emoteCount; i++)
            {
                packet.ReadUInt32("Emote Id", i);
                packet.ReadUInt32("Emote Delay (ms)", i);
            }
        }

        [Parser(Opcode.CMSG_QUESTGIVER_COMPLETE_QUEST)]
        public static void HandleQuestCompleteQuest(Packet packet)
        {
            packet.ReadGuid("GUID");
            packet.ReadEntryWithName<UInt32>(StoreNameType.Quest, "Quest ID");
        }

        [Parser(Opcode.CMSG_QUESTGIVER_REQUEST_REWARD)]
        public static void HandleQuestRequestReward(Packet packet)
        {
            packet.ReadGuid("GUID");
            packet.ReadEntryWithName<UInt32>(StoreNameType.Quest, "Quest ID");
            if (ClientVersion.AddedInVersion(ClientVersionBuild.V4_3_0_15005) && ClientVersion.RemovedInVersion(ClientVersionBuild.V4_3_4_15595)) // remove confirmed for 434
                packet.ReadUInt32("Unk UInt32");
        }

        [Parser(Opcode.SMSG_QUESTGIVER_REQUEST_ITEMS, ClientVersionBuild.Zero, ClientVersionBuild.V4_3_4_15595)]
        public static void HandleQuestRequestItems(Packet packet)
        {
            packet.ReadGuid("GUID");
            var entry = packet.ReadEntryWithName<UInt32>(StoreNameType.Quest, "Quest ID");
            packet.ReadCString("Title");
            var text = packet.ReadCString("Text");
            packet.ReadUInt32("Emote");
            packet.ReadUInt32("Unk UInt32 1");
            packet.ReadUInt32("Close Window on Cancel");

            Storage.QuestRewards.Add((uint) entry, new QuestReward {RequestItemsText = text}, null);

            if (ClientVersion.AddedInVersion(ClientVersionBuild.V3_3_3_11685))
                packet.ReadEnum<QuestFlags>("Quest Flags", TypeCode.UInt32);

            packet.ReadUInt32("Suggested Players");
            packet.ReadUInt32("Money");

            var count = packet.ReadUInt32("Number of Required Items");
            for (var i = 0; i < count; i++)
            {
                packet.ReadEntryWithName<UInt32>(StoreNameType.Item, "Required Item Id", i);
                packet.ReadUInt32("Required Item Count", i);
                packet.ReadUInt32("Required Item Display Id", i);
            }

            // flags
            packet.ReadUInt32("Unk flags 1");
            packet.ReadUInt32("Unk flags 2");
            packet.ReadUInt32("Unk flags 3");
            packet.ReadUInt32("Unk flags 4");

            if (ClientVersion.AddedInVersion(ClientVersionBuild.V4_0_1_13164))
            {
                packet.ReadUInt32("Unk flags 5");
                packet.ReadUInt32("Unk flags 6");
            }
        }

        [Parser(Opcode.SMSG_QUESTGIVER_REQUEST_ITEMS, ClientVersionBuild.V4_3_4_15595)]
        public static void HandleQuestRequestItems434(Packet packet)
        {
            packet.ReadGuid("GUID");
            var entry = packet.ReadEntryWithName<UInt32>(StoreNameType.Quest, "Quest ID");
            packet.ReadCString("Title");
            var text = packet.ReadCString("Text");
            packet.ReadUInt32("Delay");  // not confirmed
            packet.ReadUInt32("Emote");  // not confirmed
            packet.ReadUInt32("Close Window on Cancel");

            Storage.QuestRewards.Add((uint)entry, new QuestReward { RequestItemsText = text }, null);

            packet.ReadEnum<QuestFlags>("Quest Flags", TypeCode.UInt32);

            packet.ReadUInt32("Suggested Players");
            packet.ReadUInt32("Money");

            var countItems = packet.ReadUInt32("Number of Required Items");
            for (var i = 0; i < countItems; i++)
            {
                packet.ReadEntryWithName<UInt32>(StoreNameType.Item, "Required Item Id", i);
                packet.ReadUInt32("Required Item Count", i);
                packet.ReadUInt32("Required Item Display Id", i);
            }

            var countCurrencies = packet.ReadUInt32("Number of Required Currencies");
            for (var i = 0; i < countCurrencies; i++)
            {
                packet.ReadUInt32("Required Currency Id", i);
                packet.ReadUInt32("Required Currency Count", i);
            }

            // flags, if any of these flags is 0 quest is not completable
            packet.ReadUInt32("Unk flags 1"); // 2
            packet.ReadUInt32("Unk flags 2"); // 4
            packet.ReadUInt32("Unk flags 3"); // 8
            packet.ReadUInt32("Unk flags 4"); // 16
            packet.ReadUInt32("Unk flags 5"); // 64
        }

        [Parser(Opcode.SMSG_QUESTGIVER_OFFER_REWARD)]
        public static void HandleQuestOfferReward(Packet packet)
        {
            packet.ReadGuid("GUID");
            var entry = packet.ReadEntryWithName<UInt32>(StoreNameType.Quest, "Quest ID");
            packet.ReadCString("Title");
            var text = packet.ReadCString("Text");

            Storage.QuestOffers.Add((uint) entry, new QuestOffer {OfferRewardText = text}, null);

            if (ClientVersion.AddedInVersion(ClientVersionBuild.V4_0_1_13164))
            {
                packet.ReadCString("QuestGiver Text Window");
                packet.ReadCString("QuestGiver Target Name");
                packet.ReadCString("QuestTurn Text Window");
                packet.ReadCString("QuestTurn Target Name");
                packet.ReadUInt32("QuestGiverPortrait");
                packet.ReadUInt32("QuestTurnInPortrait");
            }

            if (ClientVersion.AddedInVersion(ClientVersionBuild.V3_3_0_10958))
                packet.ReadBoolean("Auto Finish");
            else
                packet.ReadBoolean("Auto Finish", TypeCode.Int32);

            if (ClientVersion.AddedInVersion(ClientVersionBuild.V3_3_3_11685))
                packet.ReadEnum<QuestFlags>("Quest Flags", TypeCode.UInt32);

            packet.ReadUInt32("Suggested Players");

            var count1 = packet.ReadUInt32("Emote Count");
            for (var i = 0; i < count1; i++)
            {
                packet.ReadUInt32("Emote Delay", i);
                packet.ReadEnum<EmoteType>("Emote Id", TypeCode.UInt32, i);
            }

            ReadExtraQuestInfo(ref packet);
        }

        [Parser(Opcode.CMSG_QUESTGIVER_CHOOSE_REWARD)]
        public static void HandleQuestChooseReward(Packet packet)
        {
            packet.ReadGuid("GUID");
            packet.ReadEntryWithName<UInt32>(StoreNameType.Quest, "Quest ID");
            packet.ReadUInt32("Reward");
        }

        [Parser(Opcode.SMSG_QUESTGIVER_QUEST_INVALID)]
        public static void HandleQuestInvalid(Packet packet)
        {
            packet.ReadEnum<QuestReasonType>("Reason", TypeCode.UInt32);
        }

        [Parser(Opcode.SMSG_QUESTGIVER_QUEST_FAILED)]
        public static void HandleQuestFailed(Packet packet)
        {
            packet.ReadEntryWithName<UInt32>(StoreNameType.Quest, "Quest ID");
            packet.ReadEnum<QuestReasonType>("Reason", TypeCode.UInt32);
        }

        [Parser(Opcode.SMSG_QUESTGIVER_QUEST_COMPLETE, ClientVersionBuild.Zero, ClientVersionBuild.V4_0_6a_13623)]
        public static void HandleQuestCompleted(Packet packet)
        {
            packet.ReadEntryWithName<Int32>(StoreNameType.Quest, "Quest ID");
            packet.ReadInt32("Reward");
            packet.ReadInt32("Money");
            packet.ReadInt32("Honor");
            packet.ReadInt32("Talents");
            packet.ReadInt32("Arena Points");
        }

        [Parser(Opcode.SMSG_QUESTGIVER_QUEST_COMPLETE, ClientVersionBuild.V4_0_6a_13623, ClientVersionBuild.V4_2_2_14545)]
        public static void HandleQuestCompleted406(Packet packet)
        {
            packet.ReadBit("Unk");
            packet.ReadUInt32("Reward Skill Id");
            packet.ReadEntryWithName<Int32>(StoreNameType.Quest, "Quest ID");
            packet.ReadInt32("Money");
            packet.ReadInt32("Talent Points");
            packet.ReadUInt32("Reward Skill Points");
            packet.ReadInt32("Reward XP");
        }

        [Parser(Opcode.SMSG_QUESTGIVER_QUEST_COMPLETE, ClientVersionBuild.V4_2_2_14545, ClientVersionBuild.V4_3_4_15595)]
        public static void HandleQuestCompleted422(Packet packet)
        {
            packet.ReadByte("Unk Byte");
            packet.ReadInt32("Reward XP");
            packet.ReadInt32("Money");
            packet.ReadInt32("Reward Skill Points");
            packet.ReadEntryWithName<Int32>(StoreNameType.Quest, "Quest ID");
            packet.ReadInt32("Reward Skill Id");
            packet.ReadInt32("Unk5"); // Talent points?
        }

        [Parser(Opcode.SMSG_QUESTGIVER_QUEST_COMPLETE, ClientVersionBuild.V4_3_4_15595)]
        public static void HandleQuestCompleted434(Packet packet)
        {
            packet.ReadInt32("TalentPoints");
            packet.ReadInt32("RewSkillPoints");
            packet.ReadInt32("Money");
            packet.ReadInt32("XP");
            packet.ReadEntryWithName<Int32>(StoreNameType.Quest, "Quest ID");
            packet.ReadInt32("RewSkillId");
            packet.ReadBit("Unk Bit 1"); // if true EVENT_QUEST_FINISHED is fired, target cleared and gossip window is open
            packet.ReadBit("Unk Bit 2");
        }

        [Parser(Opcode.CMSG_QUESTLOG_SWAP_QUEST)]
        public static void HandleQuestSwapQuest(Packet packet)
        {
            packet.ReadByte("Slot 1");
            packet.ReadByte("Slot 2");
        }

        [Parser(Opcode.CMSG_QUESTLOG_REMOVE_QUEST)]
        public static void HandleQuestRemoveQuest(Packet packet)
        {
            packet.ReadByte("Slot");
        }

        [Parser(Opcode.SMSG_QUESTUPDATE_ADD_KILL)]
        [Parser(Opcode.SMSG_QUESTUPDATE_ADD_ITEM)]
        public static void HandleQuestUpdateAdd(Packet packet)
        {
            packet.ReadEntryWithName<Int32>(StoreNameType.Quest, "Quest ID");
            var entry = packet.ReadEntry();
            packet.WriteLine("Entry: " +
                StoreGetters.GetName(entry.Value ? StoreNameType.GameObject : StoreNameType.Unit, entry.Key));

            packet.ReadInt32("Count");

            packet.ReadInt32("Required Count");
            packet.ReadGuid("GUID");
        }

        [Parser(Opcode.SMSG_QUESTGIVER_STATUS)]
        [Parser(Opcode.SMSG_QUESTGIVER_STATUS_MULTIPLE)]
        public static void HandleQuestgiverStatus(Packet packet)
        {
            uint count = 1;
            if (packet.Opcode == Opcodes.GetOpcode(Opcode.SMSG_QUESTGIVER_STATUS_MULTIPLE))
                count = packet.ReadUInt32("Count");

            if (ClientVersion.AddedInVersion(ClientVersionBuild.V4_0_6a_13623))
                for (var i = 0; i < count; i++)
                {
                    packet.ReadGuid("GUID", i);
                    packet.ReadEnum<QuestGiverStatus4x>("Status", TypeCode.Int32, i);
                }
            else
                for (var i = 0; i < count; i++)
                {
                    packet.ReadGuid("GUID", i);
                    packet.ReadEnum<QuestGiverStatus>("Status", TypeCode.Byte, i);
                }
        }

        [Parser(Opcode.SMSG_QUESTUPDATE_ADD_PVP_KILL)]
        public static void HandleQuestupdateAddPvpKill(Packet packet)
        {
            packet.ReadEntryWithName<Int32>(StoreNameType.Quest, "Quest ID");
            packet.ReadInt32("Count");
            packet.ReadInt32("Required Count");
        }

        [Parser(Opcode.SMSG_QUEST_CONFIRM_ACCEPT)]
        public static void HandleQuestConfirAccept(Packet packet)
        {
            packet.ReadEntryWithName<Int32>(StoreNameType.Quest, "Quest ID");
            packet.ReadCString("Title");
            packet.ReadGuid("GUID");
        }

        [Parser(Opcode.MSG_QUEST_PUSH_RESULT)]
        public static void HandleQuestPushResult(Packet packet)
        {
            packet.ReadGuid("GUID");
            if (packet.Direction == Direction.ClientToServer)
                packet.ReadUInt32("Quest Id");
            packet.ReadEnum<QuestPartyResult>("Result", TypeCode.Byte);
        }

        [Parser(Opcode.CMSG_QUERY_QUESTS_COMPLETED)]
        [Parser(Opcode.SMSG_QUESTLOG_FULL)]
        [Parser(Opcode.CMSG_QUESTGIVER_CANCEL)]
        [Parser(Opcode.CMSG_QUESTGIVER_STATUS_MULTIPLE_QUERY)]
        public static void HandleQuestZeroLengthPackets(Packet packet)
        {
        }

        //[Parser(Opcode.CMSG_FLAG_QUEST)]
        //[Parser(Opcode.CMSG_FLAG_QUEST_FINISH)]
        //[Parser(Opcode.CMSG_CLEAR_QUEST)]
    }
}
