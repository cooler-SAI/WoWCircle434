/*
 * Copyright (C) 2008-2015 TrinityCore <http://www.trinitycore.org/>
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

#include "ScriptPCH.h"
#include "ObjectMgr.h"
#include "DatabaseEnv.h"

class CustomRewardScript : public PlayerScript
{
public:
    CustomRewardScript() : PlayerScript("CustomRewardScript") { }

    void OnLogin(Player* player)
    {
        if(!player)
            return;
        uint32 owner_guid = player->GetGUIDLow();
        ChatHandler chH = ChatHandler(player);

        //type:
        // 1 achievement
        // 2 title
        // 3 item
        // 4 quest
        // 5 give 1 level
        // 6 level up to
        // 7 spell
        // 8 Remove pvp items from sliver char
        // 9 Remove items from char from loot
        bool rewarded = false;
        QueryResult result = CharacterDatabase.PQuery("SELECT guid, type, id, count FROM `character_reward` WHERE owner_guid = '%u'", owner_guid);
        if (result)
        {
            do
            {
                Field* fields = result->Fetch();
                uint32 guid = fields[0].GetUInt32();
                uint32 type = fields[1].GetUInt32();
                uint32 id = fields[2].GetUInt32();
                uint32 count = fields[3].GetUInt32();
                rewarded = false;
                switch (type)
                {
                    case 1: // Add achievement to char
                    {
                        if (AchievementEntry const* achievementEntry = sAchievementStore.LookupEntry(id))
                            player->CompletedAchievement(achievementEntry);
                        rewarded = true;
                    }
                    break;
                    case 2: // Add title to char
                    {
                        if(CharTitlesEntry const* titleInfo = sCharTitlesStore.LookupEntry(id))
                        {
                            char const* targetName = player->GetName();
                            char titleNameStr[80];
                            snprintf(titleNameStr, 80, titleInfo->name, targetName);
                            player->SetTitle(titleInfo);
                            chH.PSendSysMessage(LANG_TITLE_ADD_RES, id, titleNameStr, targetName);
                            rewarded = true;
                        }
                    }
                    break;
                    case 3: // Add items to char
                    {
                        if(ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(id))
                        {
                            //Adding items
                            uint32 noSpaceForCount = 0;

                            // check space and find places
                            ItemPosCountVec dest;
                            InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, id, count, &noSpaceForCount);
                            if (msg != EQUIP_ERR_OK)                               // convert to possible store amount
                                count -= noSpaceForCount;

                            if (count == 0 || dest.empty())                         // can't add any
                            {
                                chH.PSendSysMessage(LANG_ITEM_CANNOT_CREATE, id, noSpaceForCount);
                                break;
                            }

                            Item* item = player->StoreNewItem(dest, id, true, Item::GenerateItemRandomPropertyId(id));

                            if (count > 0 && item)
                                player->SendNewItem(item, count, false, true);

                            if (noSpaceForCount > 0)
                            {
                                chH.PSendSysMessage(LANG_ITEM_CANNOT_CREATE, id, noSpaceForCount);
                                CharacterDatabase.PExecute("UPDATE character_reward SET count = count - %u WHERE guid = %u", count, guid);
                            }
                            else
                                rewarded = true;
                        }
                    }
                    break;
                    case 4: // Add quest to char
                    {
                        //not implemented
                    }
                    break;
                    case 5: // Add 1 level to char
                    {
                        //not implemented
                    }
                    break;
                    case 6: // Level up to
                    {
                        //not implemented
                    }
                    break;
                    case 7: // Learn spell
                    {
                        //not implemented
                    }
                    break;
                    case 8:
                    {
                        CleanItems(player);
                        //player->SaveToDB();
                        rewarded = true;
                    }
                    break;
                    case 9:
                    {
                        FindItem(player, id, count);
                        //player->SaveToDB();
                        rewarded = true;
                    }
                    break;
                    default:
                        break;
                }
                if(rewarded)
                {
                    CharacterDatabase.PExecute("DELETE FROM character_reward WHERE guid = %u", guid);
                    player->SaveToDB();
                }
            }while (result->NextRow());
        }
        else
        {
            if(QueryResult share_result = CharacterDatabase.PQuery("SELECT * FROM `character_share` WHERE guid = '%u'", owner_guid))
            {
                uint32 totaltime = player->GetTotalPlayedTime();
                bool update = false;
                Field* fields = share_result->Fetch();
                uint32 bonus1 = fields[1].GetBool();
                uint32 bonus2 = fields[2].GetBool();
                uint32 bonus3 = fields[3].GetBool();
                uint32 bonus4 = fields[4].GetBool();
                uint32 bonus5 = fields[5].GetBool();
                uint32 bonus6 = fields[6].GetBool();
                uint32 bonus7 = fields[7].GetBool();
                uint32 bonus8 = fields[8].GetBool();
                uint32 bonus9 = fields[9].GetBool();
                uint32 bonus10 = fields[10].GetBool();

                if(!bonus1 && totaltime >= (100 * HOUR))
                {
                    CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','3','%u','1')", owner_guid, 34955);
                    update = true;
                    bonus1 = true;
                }
                if(!bonus2 && totaltime >= (250 * HOUR))
                {
                    if(player->GetTeam() == HORDE)
                        CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','3','%u','1')", owner_guid, 49046);
                    else
                        CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','3','%u','1')", owner_guid, 49044);
                    update = true;
                    bonus2 = true;
                }
                if(!bonus3 && totaltime >= (500 * HOUR))
                {
                    CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','3','%u','1')", owner_guid, 49646);
                    update = true;
                    bonus3 = true;
                }
                if(!bonus4 && totaltime >= (1000 * HOUR))
                {
                    CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','3','%u','1')", owner_guid, 71665);
                    update = true;
                    bonus4 = true;
                }
                if(!bonus5 && totaltime >= (2500 * HOUR))
                {
                    CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','3','%u','1')", owner_guid, 67418);
                    update = true;
                    bonus5 = true;
                }
                if(!bonus6 && totaltime >= (5000 * HOUR))
                {
                    CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','2','%u','1')", owner_guid, 89);
                    CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','3','%u','1')", owner_guid, 54811);
                    update = true;
                    bonus6 = true;
                }
                if(!bonus7 && totaltime >= (9000 * HOUR))
                {
                    CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','2','%u','1')", owner_guid, 85);
                    CharacterDatabase.PQuery("INSERT INTO `character_reward` (`owner_guid`, `type`, `id`, `count`) VALUES('%u','3','%u','1')", owner_guid, 21176);
                    update = true;
                    bonus7 = true;
                }
                if(update)
                    CharacterDatabase.PQuery("UPDATE `character_share` SET `bonus1` = '%u', `bonus2` = '%u', `bonus3` = '%u', `bonus4` = '%u', `bonus5` = '%u', `bonus6` = '%u', `bonus7` = '%u', `bonus8` = '%u', `bonus9` = '%u', `bonus10` = '%u' WHERE guid = '%u'", bonus1, bonus2, bonus3, bonus4, bonus5, bonus6, bonus7, bonus8, bonus9, bonus10, owner_guid);
            }
            else
                CharacterDatabase.PQuery("INSERT INTO `character_share` (`guid`, `bonus1`, `bonus2`, `bonus3`, `bonus4`, `bonus5`, `bonus6`, `bonus7`, `bonus8`, `bonus9`, `bonus10`) values('%u','0','0','0','0','0','0','0','0','0','0')", owner_guid);
        }
    }

    void FindItem(Player* player, uint32 entry, uint32 count)
    {
        // in inventory
        for (int i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; ++i)
            if (Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                if(pItem->GetEntry() == entry)
                {
                    count = ItemDel(pItem, player, count);
                    if(!count)
                        return;
                }

        for (int i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; ++i)
            if (Bag* pBag = player->GetBagByPos(i))
                for (uint32 j = 0; j < pBag->GetBagSize(); ++j)
                    if (Item* pItem = pBag->GetItemByPos(j))
                        if(pItem->GetEntry() == entry)
                        {
                            count = ItemDel(pItem, player, count);
                            if(!count)
                                return;
                        }

        for (int i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_BAG_END; ++i)
            if (Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            {
                count = ItemDel(pItem, player, count);
                if(!count)
                    return;
            }

        for (uint8 i = BANK_SLOT_ITEM_START; i < BANK_SLOT_ITEM_END; ++i)
            if (Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                if(pItem->GetEntry() == entry)
                {
                    count = ItemDel(pItem, player, count);
                    if(!count)
                        return;
                }

        for (uint8 i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; ++i)
            if (Bag* pBag = player->GetBagByPos(i))
                for (uint32 j = 0; j < pBag->GetBagSize(); ++j)
                    if (Item* pItem = pBag->GetItemByPos(j))
                        if(pItem->GetEntry() == entry)
                        {
                            count = ItemDel(pItem, player, count);
                            if(!count)
                                return;
                        }
    }

    uint32 ItemDel(Item* _item, Player* player, uint32 count)
    {
        uint32 tempcount = count;
        QueryResult result = CharacterDatabase.PQuery("SELECT itemEntry FROM character_donate WHERE itemguid = '%u'", _item->GetGUIDLow());
        if(!result)
        {
            ChatHandler chH = ChatHandler(player);
            if (_item->GetCount() >= count)
                count = 0;
            else
                count -=_item->GetCount();

            player->DestroyItemCount(_item, tempcount, true);
            //sLog->outError("ItemDel item delete %u, count %u, tempcount %u", _item->GetEntry(), count, tempcount);
            chH.PSendSysMessage(20021, sObjectMgr->GetItemTemplate(_item->GetEntry())->Name1.c_str());
        }
        return count;
    }

    void CleanItems(Player* player)
    {
        // in inventory
        for (int i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; ++i)
            if (Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                ItemIfExistDel(pItem, player);

        for (int i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; ++i)
            if (Bag* pBag = player->GetBagByPos(i))
                for (uint32 j = 0; j < pBag->GetBagSize(); ++j)
                    if (Item* pItem = pBag->GetItemByPos(j))
                        ItemIfExistDel(pItem, player);

        for (int i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_BAG_END; ++i)
            if (Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                ItemIfExistDel(pItem, player);

        for (uint8 i = BANK_SLOT_ITEM_START; i < BANK_SLOT_ITEM_END; ++i)
            if (Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                ItemIfExistDel(pItem, player);

        for (uint8 i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; ++i)
            if (Bag* pBag = player->GetBagByPos(i))
                for (uint32 j = 0; j < pBag->GetBagSize(); ++j)
                    if (Item* pItem = pBag->GetItemByPos(j))
                        ItemIfExistDel(pItem, player);
    }

    void ItemIfExistDel(Item* _item, Player* player)
    {
        switch (_item->GetEntry())
        {
            case 51388:
            case 51389:
            case 51390:
            case 51391:
            case 51392:
            case 51393:
            case 51394:
            case 51395:
            case 51396:
            case 51397:
            case 51398:
            case 51399:
            case 51400:
            case 51401:
            case 51404:
            case 51405:
            case 51406:
            case 51407:
            case 51408:
            case 51409:
            case 51410:
            case 51411:
            case 51412:
            case 51413:
            case 51414:
            case 51415:
            case 51416:
            case 51418:
            case 51419:
            case 51420:
            case 51421:
            case 51422:
            case 51424:
            case 51425:
            case 51426:
            case 51427:
            case 51428:
            case 51430:
            case 51433:
            case 51434:
            case 51435:
            case 51436:
            case 51438:
            case 51439:
            case 51440:
            case 51441:
            case 51442:
            case 51443:
            case 51444:
            case 51445:
            case 51446:
            case 51447:
            case 51448:
            case 51449:
            case 51450:
            case 51451:
            case 51452:
            case 51453:
            case 51454:
            case 51455:
            case 51456:
            case 51457:
            case 51458:
            case 51459:
            case 51460:
            case 51461:
            case 51462:
            case 51463:
            case 51464:
            case 51465:
            case 51466:
            case 51467:
            case 51468:
            case 51469:
            case 51470:
            case 51471:
            case 51473:
            case 51474:
            case 51475:
            case 51476:
            case 51477:
            case 51479:
            case 51480:
            case 51481:
            case 51482:
            case 51483:
            case 51484:
            case 51485:
            case 51486:
            case 51487:
            case 51488:
            case 51489:
            case 51490:
            case 51491:
            case 51492:
            case 51493:
            case 51494:
            case 51495:
            case 51496:
            case 51497:
            case 51498:
            case 51499:
            case 51500:
            case 51502:
            case 51503:
            case 51504:
            case 51505:
            case 51506:
            case 51508:
            case 51509:
            case 51510:
            case 51511:
            case 51512:
            case 51514:
            case 51515:
            case 51516:
            case 51517:
            case 51518:
            case 51519:
            case 51520:
            case 51521:
            case 51522:
            case 51523:
            case 51524:
            case 51525:
            case 51526:
            case 51527:
            case 51528:
            case 51529:
            case 51530:
            case 51531:
            case 51532:
            case 51533:
            case 51534:
            case 51535:
            case 51536:
            case 51537:
            case 51538:
            case 51539:
            case 51540:
            case 51541:
            case 51542:
            case 51543:
            case 51544:
            case 51545:
            case 60408:
            case 60409:
            case 60410:
            case 60411:
            case 60412:
            case 60418:
            case 60419:
            case 60420:
            case 60421:
            case 60422:
            case 60423:
            case 60424:
            case 60425:
            case 60426:
            case 60427:
            case 60428:
            case 60429:
            case 60430:
            case 60431:
            case 60432:
            case 60433:
            case 60434:
            case 60435:
            case 60436:
            case 60437:
            case 60438:
            case 60439:
            case 60440:
            case 60441:
            case 60442:
            case 60443:
            case 60444:
            case 60445:
            case 60446:
            case 60447:
            case 60448:
            case 60449:
            case 60450:
            case 60451:
            case 60452:
            case 60453:
            case 60454:
            case 60455:
            case 60456:
            case 60457:
            case 60458:
            case 60459:
            case 60460:
            case 60461:
            case 60462:
            case 60463:
            case 60464:
            case 60465:
            case 60466:
            case 60467:
            case 60468:
            case 60469:
            case 60470:
            case 60471:
            case 60472:
            case 60473:
            case 60474:
            case 60475:
            case 60476:
            case 60477:
            case 60478:
            case 60479:
            case 60480:
            case 60481:
            case 60482:
            case 60505:
            case 60507:
            case 60508:
            case 60512:
            case 60515:
            case 60516:
            case 60517:
            case 60518:
            case 60520:
            case 60521:
            case 60523:
            case 60537:
            case 60538:
            case 60539:
            case 60540:
            case 60541:
            case 60554:
            case 60555:
            case 60557:
            case 60559:
            case 60564:
            case 60565:
            case 60566:
            case 60568:
            case 60570:
            case 60571:
            case 60612:
            case 60613:
            case 60614:
            case 60626:
            case 60627:
            case 60628:
            case 60629:
            case 60630:
            case 60631:
            case 60632:
            case 60633:
            case 60634:
            case 60635:
            case 60636:
            case 60637:
            case 60645:
            case 60647:
            case 60650:
            case 60651:
            case 60658:
            case 60659:
            case 60661:
            case 60662:
            case 60664:
            case 60668:
            case 60669:
            case 60670:
            case 60673:
            case 60776:
            case 60778:
            case 60779:
            case 60786:
            case 60787:
            case 60794:
            case 60795:
            case 60796:
            case 60797:
            case 60798:
            case 60799:
            case 60800:
            case 60801:
            case 60806:
            case 60807:
            case 61026:
            case 61030:
            case 61031:
            case 61032:
            case 61033:
            case 61034:
            case 61035:
            case 61045:
            case 61046:
            case 61326:
            case 61339:
            case 61341:
            case 61342:
            case 61343:
            case 61346:
            case 61350:
            case 61353:
            case 61354:
            case 61355:
            case 61357:
            case 61358:
            case 61359:
            case 61360:
            case 61361:
            case 64695:
            case 64701:
            case 64726:
            case 64744:
            case 64755:
            case 64783:
            case 64810:
            case 64860:
            case 65523:
            case 65524:
            case 65525:
            case 65526:
            case 65527:
            case 65528:
            case 65529:
            case 65530:
            case 65531:
            case 65532:
            case 65533:
            case 65534:
            case 65535:
            case 65536:
            case 65537:
            case 65538:
            case 65539:
            case 65540:
            case 65541:
            case 65542:
            case 65543:
            case 65544:
            case 65545:
            case 65546:
            case 65547:
            case 65548:
            case 65549:
            case 65550:
            case 65551:
            case 65552:
            case 65553:
            case 65554:
            case 65555:
            case 65556:
            case 65557:
            case 65558:
            case 65559:
            case 65560:
            case 65561:
            case 65562:
            case 65563:
            case 65564:
            case 65565:
            case 65566:
            case 65567:
            case 65568:
            case 65569:
            case 65570:
            case 65571:
            case 65572:
            case 65573:
            case 65574:
            case 65575:
            case 65576:
            case 65577:
            case 65578:
            case 65579:
            case 65580:
            case 65581:
            case 65582:
            case 65583:
            case 65584:
            case 65587:
            case 65588:
            case 65589:
            case 65593:
            case 65594:
            case 65595:
            case 65596:
            case 65597:
            case 65598:
            case 65599:
            case 65600:
            case 65603:
            case 65604:
            case 65605:
            case 65606:
            case 67447:
            case 67448:
            case 67449:
            case 67450:
            case 67451:
            case 67452:
            case 67453:
            case 67461:
            case 67462:
            case 67463:
            case 67464:
            case 67465:
            case 67475:
            case 67476:
            case 67477:
            case 67478:
            case 67479:
            case 69781:
            case 69782:
            case 69783:
            case 69784:
            case 69785:
            case 69786:
            case 70178:
            case 70179:
            case 70180:
            case 70181:
            case 70182:
            case 70183:
            case 70184:
            case 70192:
            case 70193:
            case 70194:
            case 70195:
            case 70196:
            case 70206:
            case 70207:
            case 70208:
            case 70209:
            case 70210:
            case 70213:
            case 70224:
            case 70225:
            case 70226:
            case 70227:
            case 70228:
            case 70231:
            case 70234:
            case 70235:
            case 70236:
            case 70237:
            case 70238:
            case 70239:
            case 70240:
            case 70241:
            case 70242:
            case 70243:
            case 70244:
            case 70245:
            case 70246:
            case 70247:
            case 70248:
            case 70254:
            case 70255:
            case 70256:
            case 70257:
            case 70258:
            case 70259:
            case 70260:
            case 70261:
            case 70262:
            case 70263:
            case 70264:
            case 70265:
            case 70266:
            case 70267:
            case 70268:
            case 70269:
            case 70270:
            case 70271:
            case 70272:
            case 70273:
            case 70274:
            case 70275:
            case 70276:
            case 70277:
            case 70278:
            case 70279:
            case 70280:
            case 70281:
            case 70282:
            case 70283:
            case 70284:
            case 70285:
            case 70286:
            case 70287:
            case 70288:
            case 70289:
            case 70290:
            case 70291:
            case 70292:
            case 70293:
            case 70294:
            case 70295:
            case 70296:
            case 70297:
            case 70298:
            case 70299:
            case 70300:
            case 70301:
            case 70302:
            case 70303:
            case 70304:
            case 70305:
            case 70306:
            case 70307:
            case 70308:
            case 70309:
            case 70310:
            case 70311:
            case 70312:
            case 70313:
            case 70314:
            case 70315:
            case 70316:
            case 70317:
            case 70318:
            case 70319:
            case 70320:
            case 70322:
            case 70324:
            case 70325:
            case 70326:
            case 70327:
            case 70332:
            case 70333:
            case 70334:
            case 70335:
            case 70336:
            case 70337:
            case 70338:
            case 70339:
            case 70340:
            case 70360:
            case 70361:
            case 70362:
            case 70363:
            case 70364:
            case 70365:
            case 70367:
            case 70368:
            case 70369:
            case 70370:
            case 70371:
            case 70372:
            case 70373:
            case 70374:
            case 70375:
            case 70376:
            case 70377:
            case 70378:
            case 70379:
            case 70380:
            case 70381:
            case 70382:
            case 70383:
            case 70384:
            case 70385:
            case 70387:
            case 70388:
            case 70389:
            case 70390:
            case 70391:
            case 70392:
            case 70393:
            case 70394:
            case 70395:
            case 70396:
            case 70397:
            case 70398:
            case 70399:
            case 70400:
            case 70401:
            case 70402:
            case 70403:
            case 70404:
            case 70409:
            case 70410:
            case 70411:
            case 70412:
            case 70413:
            case 70414:
            case 70420:
            case 70421:
            case 70422:
            case 70423:
            case 70424:
            case 70425:
            case 70426:
            case 70427:
            case 70428:
            case 70429:
            case 70430:
            case 70431:
            case 70432:
            case 70433:
            case 70434:
            case 70435:
            case 70436:
            case 70437:
            case 70438:
            case 70439:
            case 70440:
            case 70441:
            case 70442:
            case 70443:
            case 70444:
            case 70445:
            case 70446:
            case 70447:
            case 70448:
            case 70449:
            case 70450:
            case 70451:
            case 70452:
            case 70453:
            case 70454:
            case 70455:
            case 70456:
            case 70457:
            case 70458:
            case 70459:
            case 70460:
            case 70461:
            case 70462:
            case 70463:
            case 70464:
            case 70465:
            case 70466:
            case 70467:
            case 70468:
            case 70469:
            case 70470:
            case 70471:
            case 70472:
            case 70473:
            case 70474:
            case 70475:
            case 70476:
            case 70477:
            case 70478:
            case 70479:
            case 70480:
            case 70481:
            case 70484:
            case 70485:
            case 70486:
            case 70490:
            case 70491:
            case 70492:
            case 70493:
            case 70494:
            case 70495:
            case 70496:
            case 70497:
            case 70500:
            case 70501:
            case 70502:
            case 70503:
            case 70513:
            case 70514:
            case 70517:
            case 70518:
            case 70519:
            case 70520:
            case 70521:
            case 70522:
            case 70529:
            case 70530:
            case 70531:
            case 70532:
            case 70533:
            case 70534:
            case 70535:
            case 70536:
            case 70537:
            case 70538:
            case 70539:
            case 70540:
            case 70541:
            case 70542:
            case 70543:
            case 70544:
            case 70545:
            case 70546:
            case 70547:
            case 70548:
            case 70549:
            case 70550:
            case 70551:
            case 70552:
            case 70553:
            case 70554:
            case 70555:
            case 70556:
            case 70557:
            case 70558:
            case 70559:
            case 70560:
            case 70561:
            case 70562:
            case 70563:
            case 70564:
            case 70565:
            case 70566:
            case 70567:
            case 70568:
            case 70569:
            case 70570:
            case 70573:
            case 70574:
            case 70575:
            case 70576:
            case 70577:
            case 70578:
            case 70579:
            case 70580:
            case 70581:
            case 70582:
            case 70583:
            case 70584:
            case 70585:
            case 70586:
            case 70587:
            case 70588:
            case 70589:
            case 70590:
            case 70591:
            case 70592:
            case 70593:
            case 70594:
            case 70595:
            case 70596:
            case 70602:
            case 70603:
            case 70604:
            case 70605:
            case 70606:
            case 70607:
            case 70613:
            case 70614:
            case 70620:
            case 70621:
            case 70622:
            case 70623:
            case 70624:
            case 70625:
            case 70626:
            case 70627:
            case 70632:
            case 70633:
            case 70634:
            case 70635:
            case 70636:
            case 70637:
            case 70638:
            case 70639:
            case 70641:
            case 70653:
            case 70654:
            case 70655:
            case 70656:
            case 70657:
            case 70658:
            case 70659:
            case 70660:
            case 70661:
            case 70662:
            case 70669:
            case 70670:
            case 70671:
            case 70672:
            case 70673:
            case 70674:
            case 70675:
            case 72304:
            case 72305:
            case 72306:
            case 72307:
            case 72308:
            case 72309:
            case 72311:
            case 72312:
            case 72313:
            case 72314:
            case 72315:
            case 72316:
            case 72317:
            case 72318:
            case 72319:
            case 72321:
            case 72322:
            case 72323:
            case 72324:
            case 72325:
            case 72326:
            case 72327:
            case 72329:
            case 72330:
            case 72331:
            case 72332:
            case 72333:
            case 72334:
            case 72335:
            case 72336:
            case 72337:
            case 72338:
            case 72339:
            case 72340:
            case 72341:
            case 72345:
            case 72346:
            case 72347:
            case 72348:
            case 72349:
            case 72353:
            case 72354:
            case 72355:
            case 72356:
            case 72357:
            case 72359:
            case 72360:
            case 72361:
            case 72362:
            case 72363:
            case 72364:
            case 72365:
            case 72366:
            case 72367:
            case 72368:
            case 72369:
            case 72370:
            case 72371:
            case 72372:
            case 72373:
            case 72374:
            case 72375:
            case 72376:
            case 72377:
            case 72383:
            case 72384:
            case 72385:
            case 72386:
            case 72387:
            case 72388:
            case 72394:
            case 72395:
            case 72398:
            case 72399:
            case 72410:
            case 72411:
            case 72412:
            case 72413:
            case 72414:
            case 72415:
            case 72422:
            case 72423:
            case 72424:
            case 72425:
            case 72426:
            case 72448:
            case 72449:
            case 72450:
            case 72451:
            case 72452:
            case 72453:
            case 72454:
            case 72455:
            case 72457:
            case 72458:
            case 72459:
            case 72460:
            case 72461:
            case 72462:
            case 72463:
            case 72464:
            case 72465:
            case 72466:
            case 72467:
            case 72468:
            case 73412:
            case 73413:
            case 73414:
            case 73415:
            case 73416:
            case 73417:
            case 73418:
            case 73419:
            case 73420:
            case 73421:
            case 73422:
            case 73423:
            case 73424:
            case 73425:
            case 73426:
            case 73427:
            case 73428:
            case 73429:
            case 73430:
            case 73431:
            case 73432:
            case 73433:
            case 73434:
            case 73435:
            case 73436:
            case 73437:
            case 73438:
            case 73439:
            case 73440:
            case 73441:
            case 73442:
            case 73443:
            case 73444:
            case 73445:
            case 73446:
            case 73447:
            case 73448:
            case 73449:
            case 73450:
            case 73451:
            case 73452:
            case 73453:
            case 73454:
            case 73455:
            case 73456:
            case 73457:
            case 73458:
            case 73459:
            case 73460:
            case 73461:
            case 73462:
            case 73463:
            case 73464:
            case 73465:
            case 73466:
            case 73467:
            case 73468:
            case 73469:
            case 73470:
            case 73471:
            case 73472:
            case 73473:
            case 73474:
            case 73475:
            case 73476:
            case 73477:
            case 73478:
            case 73479:
            case 73480:
            case 73481:
            case 73482:
            case 73483:
            case 73484:
            case 73485:
            case 73486:
            case 73487:
            case 73488:
            case 73489:
            case 73490:
            case 73491:
            case 73492:
            case 73493:
            case 73494:
            case 73495:
            case 73496:
            case 73497:
            case 73498:
            case 73501:
            case 73502:
            case 73503:
            case 73504:
            case 73505:
            case 73506:
            case 73507:
            case 73508:
            case 73509:
            case 73510:
            case 73511:
            case 73512:
            case 73513:
            case 73514:
            case 73515:
            case 73516:
            case 73517:
            case 73518:
            case 73519:
            case 73520:
            case 73522:
            case 73523:
            case 73524:
            case 73525:
            case 73526:
            case 73527:
            case 73528:
            case 73529:
            case 73530:
            case 73531:
            case 73532:
            case 73533:
            case 73534:
            case 73535:
            case 73536:
            case 73537:
            case 73538:
            case 73539:
            case 73540:
            case 73541:
            case 73542:
            case 73543:
            case 73544:
            case 73545:
            case 73546:
            case 73547:
            case 73548:
            case 73549:
            case 73550:
            case 73551:
            case 73552:
            case 73553:
            case 73554:
            case 73555:
            case 73556:
            case 73557:
            case 73558:
            case 73559:
            case 73560:
            case 73561:
            case 73562:
            case 73563:
            case 73564:
            case 73565:
            case 73566:
            case 73567:
            case 73568:
            case 73569:
            case 73570:
            case 73571:
            case 73572:
            case 73573:
            case 73574:
            case 73575:
            case 73576:
            case 73578:
            case 73579:
            case 73580:
            case 73581:
            case 73582:
            case 73583:
            case 73584:
            case 73585:
            case 73586:
            case 73587:
            case 73588:
            case 73589:
            case 73590:
            case 73591:
            case 73592:
            case 73593:
            case 73594:
            case 73595:
            case 73596:
            case 73597:
            case 73598:
            case 73599:
            case 73600:
            case 73601:
            case 73602:
            case 73603:
            case 73604:
            case 73605:
            case 73606:
            case 73607:
            case 73608:
            case 73609:
            case 73610:
            case 73611:
            case 73612:
            case 73613:
            case 73614:
            case 73615:
            case 73616:
            case 73617:
            case 73618:
            case 73619:
            case 73620:
            case 73621:
            case 73622:
            case 73623:
            case 73624:
            case 73625:
            case 73626:
            case 73627:
            case 73628:
            case 73629:
            case 73630:
            case 73631:
            case 73632:
            case 73633:
            case 73634:
            case 73635:
            case 73636:
            case 73637:
            case 73638:
            case 73639:
            case 73640:
            case 73641:
            case 73642:
            case 73643:
            case 73644:
            case 73645:
            case 73646:
            case 73647:
            case 73648:
            case 73651:
            case 73652:
            case 73653:
            case 73654:
            case 73655:
            case 73656:
            case 73657:
            case 73658:
            case 73659:
            case 73660:
            case 73661:
            case 73662:
            case 73663:
            case 73664:
            case 73665:
            case 73666:
            case 73667:
            case 73668:
            case 73669:
            case 73670:
            case 73671:
            case 73672:
            case 73673:
            case 73674:
            case 73675:
            case 73676:
            case 73677:
            case 73678:
            case 73679:
            case 73680:
            case 73681:
            case 73682:
            case 73683:
            case 73684:
            case 73685:
            case 73686:
            case 73687:
            case 73688:
            case 73689:
            case 73690:
            case 73691:
            case 73692:
            case 73693:
            case 73694:
            case 73695:
            case 73696:
            case 73697:
            case 73698:
            case 73699:
            case 73700:
            case 73701:
            case 73702:
            case 73703:
            case 73704:
            case 73705:
            case 73706:
            case 73707:
            case 73708:
            case 73709:
            case 73710:
            case 73711:
            case 73712:
            case 73713:
            case 73714:
            case 73715:
            case 73716:
            case 73717:
            case 73718:
            case 73719:
            case 73720:
            case 73721:
            case 73722:
            case 73723:
            case 73724:
            case 73725:
            case 73726:
            case 73727:
            case 73728:
            case 73729:
            case 73730:
            case 73731:
            case 73732:
            case 73733:
            case 73734:
            case 73735:
            case 73736:
            case 73737:
            case 73738:
            case 73739:
            case 73740:
            case 73741:
            case 73742:
            case 73743:
            case 73744:
            case 73745:
            case 77245:
            case 77938:
            case 87641:
            {
                QueryResult result = CharacterDatabase.PQuery("SELECT itemEntry FROM character_donate WHERE itemguid = '%u'", _item->GetGUIDLow());
                if(!result)
                {
                    ChatHandler chH = ChatHandler(player);
                    player->DestroyItem(_item->GetBagSlot(), _item->GetSlot(), true);
                    //sLog->outError("ItemIfExistDel item delete %u", _item->GetEntry());
                    chH.PSendSysMessage(20020, sObjectMgr->GetItemTemplate(_item->GetEntry())->Name1.c_str());
                }
            }
        }
    }
};

void AddSC_custom_reward()
{
    new CustomRewardScript();
}
