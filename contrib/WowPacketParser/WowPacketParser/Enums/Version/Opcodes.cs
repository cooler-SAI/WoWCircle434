using System.Globalization;
using WowPacketParser.Enums.Version.V4_3_4_15595;
using WowPacketParser.Misc;

namespace WowPacketParser.Enums.Version
{
    public static class Opcodes
    {
        private static BiDictionary<Opcode, int> Dict = GetOpcodeDictionary(ClientVersion.Build);

        public static void InitializeOpcodeDictionary()
        {
            Dict = GetOpcodeDictionary(ClientVersion.Build);
        }

        private static BiDictionary<Opcode, int> GetOpcodeDictionary(ClientVersionBuild build)
        {
            switch (build)
            {
                case ClientVersionBuild.V2_4_3_8606:
                case ClientVersionBuild.V3_0_2_9056:
                case ClientVersionBuild.V3_0_3_9183:
                case ClientVersionBuild.V3_0_8_9464:
                case ClientVersionBuild.V3_0_8a_9506:
                case ClientVersionBuild.V3_0_9_9551:
                case ClientVersionBuild.V3_1_0_9767:
                case ClientVersionBuild.V3_1_1_9806:
                case ClientVersionBuild.V3_1_1a_9835:
                case ClientVersionBuild.V3_1_2_9901:
                case ClientVersionBuild.V3_1_3_9947:
                case ClientVersionBuild.V3_2_0_10192:
                case ClientVersionBuild.V3_2_0a_10314:
                case ClientVersionBuild.V3_2_2_10482:
                case ClientVersionBuild.V3_2_2a_10505:
                case ClientVersionBuild.V3_3_0_10958:
                case ClientVersionBuild.V3_3_0a_11159:
                case ClientVersionBuild.V3_3_3_11685:
                case ClientVersionBuild.V3_3_3a_11723:
                case ClientVersionBuild.V3_3_5a_12340:
                case ClientVersionBuild.V4_0_3_13329:
                case ClientVersionBuild.V4_0_6_13596:
                case ClientVersionBuild.V4_0_6a_13623:
                case ClientVersionBuild.V4_1_0_13914:
                case ClientVersionBuild.V4_1_0a_14007:
                case ClientVersionBuild.V4_2_0_14333:
                case ClientVersionBuild.V4_2_0a_14480:
                case ClientVersionBuild.V4_2_2_14545:
                case ClientVersionBuild.V4_3_0_15005:
                case ClientVersionBuild.V4_3_0a_15050:
                case ClientVersionBuild.V4_3_2_15211:
                case ClientVersionBuild.V4_3_3_15354:
                case ClientVersionBuild.V4_3_4_15595:
                {
                    return Opcodes_4_3_4.Opcodes();
                }
                case ClientVersionBuild.V5_0_4_16016:
                case ClientVersionBuild.V5_0_5_16048:
                case ClientVersionBuild.V5_0_5a_16057:
                case ClientVersionBuild.V5_0_5b_16135:
                case ClientVersionBuild.V5_1_0_16309:
                case ClientVersionBuild.V5_1_0a_16357:
                case ClientVersionBuild.V5_2_0_16650:
                case ClientVersionBuild.V5_2_0_16669:
                case ClientVersionBuild.V5_2_0_16683:
                case ClientVersionBuild.V5_2_0_16685:
                case ClientVersionBuild.V5_2_0_16701:
                case ClientVersionBuild.V5_2_0_16709:
                case ClientVersionBuild.V5_2_0_16716:
                case ClientVersionBuild.V5_2_0_16733:
                case ClientVersionBuild.V5_2_0_16769:
                case ClientVersionBuild.V5_2_0_16826:
                case ClientVersionBuild.V5_3_0_16981:
                case ClientVersionBuild.V5_3_0_16983:
                case ClientVersionBuild.V5_3_0_16992:
                case ClientVersionBuild.V5_3_0_17055:
                case ClientVersionBuild.V5_3_0_17116:
                case ClientVersionBuild.V5_3_0_17128:
                default:
                {
                    return Opcodes_4_3_4.Opcodes();
                }
            }
        }

        public static Opcode GetOpcode(int opcodeId)
        {
            return Dict.GetBySecond(opcodeId);
        }

        public static int GetOpcode(Opcode opcode)
        {
            return Dict.GetByFirst(opcode);
        }

        public static string GetOpcodeName(int opcodeId)
        {
            var opc = GetOpcode(opcodeId);
            return opc == 0 ? opcodeId.ToString(CultureInfo.InvariantCulture) : opc.ToString();
        }
    }
}
