-- -------------------------------------------------------- 
-- 2011_06_24_07_world_achievement_criteria_data.sql 
-- -------------------------------------------------------- 
DELETE FROM `spell_dbc` WHERE `id`=59046;
INSERT INTO `spell_dbc` (`Id`,`Effect1`,`EffectRadiusIndex1`,`EffectImplicitTargetA1`,`EffectImplicitTargetB1`,`comment`)
VALUES
(59046,3,28,22,30,'Tribunal of Ages - Criteria Marker');

DELETE FROM `achievement_criteria_data` WHERE `criteria_id`=7590 AND `type`=11;
INSERT INTO `achievement_criteria_data` (`criteria_id`,`type`,`value1`,`value2`,`ScriptName`)
VALUES
(7590,11,0,0,'achievement_brann_spankin_new');
 
 
-- -------------------------------------------------------- 
-- 2011_06_25_00_world_spell_dbc.sql 
-- -------------------------------------------------------- 
DELETE FROM `spell_dbc` WHERE `Id`=24308;
INSERT INTO `spell_dbc`(`Id`,`Dispel`,`Mechanic`,`Attributes`,`AttributesEx`,`AttributesEx2`,`AttributesEx3`,`AttributesEx4`,`AttributesEx5`,`Stances`,`StancesNot`,`Targets`,`CastingTimeIndex`,`AuraInterruptFlags`,`ProcFlags`,`ProcChance`,`ProcCharges`,`MaxLevel`,`BaseLevel`,`SpellLevel`,`DurationIndex`,`RangeIndex`,`StackAmount`,`EquippedItemClass`,`EquippedItemSubClassMask`,`EquippedItemInventoryTypeMask`,`Effect1`,`Effect2`,`Effect3`,`EffectDieSides1`,`EffectDieSides2`,`EffectDieSides3`,`EffectRealPointsPerLevel1`,`EffectRealPointsPerLevel2`,`EffectRealPointsPerLevel3`,`EffectBasePoints1`,`EffectBasePoints2`,`EffectBasePoints3`,`EffectMechanic1`,`EffectMechanic2`,`EffectMechanic3`,`EffectImplicitTargetA1`,`EffectImplicitTargetA2`,`EffectImplicitTargetA3`,`EffectImplicitTargetB1`,`EffectImplicitTargetB2`,`EffectImplicitTargetB3`,`EffectRadiusIndex1`,`EffectRadiusIndex2`,`EffectRadiusIndex3`,`EffectApplyAuraName1`,`EffectApplyAuraName2`,`EffectApplyAuraName3`,`EffectAmplitude1`,`EffectAmplitude2`,`EffectAmplitude3`,`EffectMultipleValue1`,`EffectMultipleValue2`,`EffectMultipleValue3`,`EffectMiscValue1`,`EffectMiscValue2`,`EffectMiscValue3`,`EffectMiscValueB1`,`EffectMiscValueB2`,`EffectMiscValueB3`,`EffectTriggerSpell1`,`EffectTriggerSpell2`,`EffectTriggerSpell3`,`EffectSpellClassMaskA1`,`EffectSpellClassMaskA2`,`EffectSpellClassMaskA3`,`EffectSpellClassMaskB1`,`EffectSpellClassMaskB2`,`EffectSpellClassMaskB3`,`EffectSpellClassMaskC1`,`EffectSpellClassMaskC2`,`EffectSpellClassMaskC3`,`MaxTargetLevel`,`SpellFamilyName`,`SpellFamilyFlags1`,`SpellFamilyFlags2`,`SpellFamilyFlags3`,`MaxAffectedTargets`,`DmgClass`,`PreventionType`,`DmgMultiplier1`,`DmgMultiplier2`,`DmgMultiplier3`,`AreaGroupId`,`SchoolMask`,`Comment`) VALUES
('24308','0','0','256','0','0','0','0','0','0','0','0','1','0','0','101','0','0','0','0','26','1','0','-1','0','0','41','0','0','1','0','0','0','0','0','0','0','0','0','0','0','18','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','14986','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','1','0','0','0','0','Summon Shade of Jin''Do');
 
 
-- -------------------------------------------------------- 
-- 2011_06_25_01_world_script_texts.sql 
-- -------------------------------------------------------- 
-- Fix typo for Ragnaros' Magmaburst
UPDATE `script_texts` SET `content_default`='MY PATIENCE IS DWINDLING! COME, GNATS, TO YOUR DEATH!' WHERE `npc_entry`=11502 AND `entry`='-1409018';
 
 
-- -------------------------------------------------------- 
-- 2011_06_25_02_world_misc.sql 
-- -------------------------------------------------------- 
-- Fix Quest 11465 "The Ransacked Caravan"
-- Fjord Turkey SAI
SET @ENTRY   := 24746; -- NPC entry
SET @SPELL1  := 44323; -- Hawk Hunting
SET @SPELL2  := 44327; -- Hawk Hunting
UPDATE `creature_template` SET `AIName`='SmartAI', `ScriptName`='' WHERE `entry`=@ENTRY;
DELETE FROM `creature_ai_scripts` WHERE `creature_id`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid`=@ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,1,8,0,100,0,@SPELL1,0,0,0,11,@SPELL2,0,0,0,0,0,7,0,0,0,0,0,0,0, 'Fjord Turkey - On Spellhit - Cast spell on invoker'),
(@ENTRY,0,1,0,61,0,100,0,0,0,0,0,41,0,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Fjord Turkey - On Spellhit - Despawn');
-- Fix condition for Item 34111 to only target Fjord Turkey
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=18 AND `SourceEntry`=34111;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`ElseGroup`,`ConditionTypeOrReference`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(18,0,34111,0,24,1,@ENTRY,0,63,'','Item Trained Rock Falcon targets only Fjord Turkey');
-- Dragonflayer Ambusher SAI
SET @ENTRY   := 24779; -- NPC entry
SET @SPELL1  := 38557; -- Throw
UPDATE `creature_template` SET `AIName`='SmartAI', `ScriptName`='' WHERE `entry`=@ENTRY;
DELETE FROM `creature_ai_scripts` WHERE `creature_id`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid`=@ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,0,11,0,100,0,0,0,0,0,89,10,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Dragonflayer Ambusher - On spawn - Set random movement'),
(@ENTRY,0,1,0,4,0,100,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Dragonflayer Ambusher - On aggro - Say text'),
(@ENTRY,0,2,0,0,0,100,1,0,0,0,0,11,@SPELL1,0,0,0,0,0,2,0,0,0,0,0,0,0, 'Dragonflayer Ambusher - Combat - Cast spell on victim');
-- NPC talk text insert from sniff
DELETE FROM `creature_text` WHERE `entry`=24779;
INSERT INTO `creature_text` (`entry`,`groupid`,`id`,`text`,`type`,`language`,`probability`,`emote`,`duration`,`sound`,`comment`) VALUES
(24779,0,0, 'Tonight we feast on you!',12,0,100,0,0,0, 'Dragonflayer Ambusher'),
(24779,0,1, 'Such easy prey...',12,0,100,0,0,0, 'Dragonflayer Ambusher'),
(24779,0,2, 'Who''s hunting who?',12,0,100,0,0,0, 'Dragonflayer Ambusher');
 
 
-- -------------------------------------------------------- 
-- 2011_06_25_03_world_sai.sql 
-- -------------------------------------------------------- 
-- Ice Heart Jormungar Feeder SAI
SET @ENTRY   := 26358; -- NPC entry
SET @SPELL1  := 47447; -- Corrosive Spit
UPDATE `creature_template` SET `AIName`='SmartAI', `ScriptName`='' WHERE `entry`=@ENTRY;
DELETE FROM `creature_ai_scripts` WHERE `creature_id`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid`=@ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,0,0,0,100,0,5000,11000,20000,26000,11,@SPELL1,0,0,0,0,0,2,0,0,0,0,0,0,0, 'Ice Heart Jormungar Feeder - Combat - Cast Corrosive Spit');

-- SAI for Fulgorge
SET @ENTRY := 18678;
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry`=@ENTRY;
UPDATE `creature` SET `spawndist`=20,`MovementType`=1 WHERE `id`=@ENTRY;
DELETE FROM `creature_ai_scripts` WHERE `creature_id`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid`=@ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,1,1,0,100,1,0,0,0,0,11,34038,0,0,0,0,0,1,0,0,0,0,0,0,0,'Fulgorge - OOC - Cast Submerge Visual'),
(@ENTRY,0,1,0,61,0,100,1,0,0,0,0,18,33554434,0,0,0,0,0,1,0,0,0,0,0,0,0,'Fulgorge - OOC - Set Unselectable and Unattackable Flags'),
(@ENTRY,0,2,3,0,0,100,1,0,0,0,0,28,34038,0,0,0,0,0,1,0,0,0,0,0,0,0,'Fulgorge - On Aggro - Remove Submerge Visual'),
(@ENTRY,0,3,4,61,0,100,0,0,0,0,0,19,33554434,0,0,0,0,0,1,0,0,0,0,0,0,0,'Fulgorge - On Aggro - Remove Unselectable and Unattackable Flags'),
(@ENTRY,0,4,5,61,0,100,0,0,0,0,0,21,0,0,0,0,0,0,1,0,0,0,0,0,0,0,'Fulgorge - On Aggro - Prevent Combat Movement'),
(@ENTRY,0,5,0,61,0,100,0,0,0,0,0,22,1,0,0,0,0,0,1,0,0,0,0,0,0,0,'Fulgorge - On Aggro - Set Phase 1'),
(@ENTRY,0,6,7,0,1,100,0,1000,1000,2100,4500,11,31747,1,0,0,0,0,2,0,0,0,0,0,0,0,'Fulgorge - Combat - Cast Poison (Phase 1)'),
(@ENTRY,0,7,0,61,1,100,0,0,0,0,0,21,0,0,0,0,0,0,1,0,0,0,0,0,0,0,'Fulgorge - Combat - Prevent Combat Movement (Phase 1)'),
(@ENTRY,0,8,0,0,1,100,0,20400,20400,45000,50000,11,32738,1,0,0,0,0,2,0,0,0,0,0,0,0,'Fulgorge - Combat - Cast Bore (Phase 1)'),
(@ENTRY,0,9,10,9,1,100,0,20,60,0,0,11,34038,0,0,0,0,0,1,0,0,0,0,0,0,0,'Fulgorge - At 20 - 60 Yards Range - Cast Submerge Visual (Phase 1)'),
(@ENTRY,0,10,0,61,1,100,0,0,0,0,0,22,2,0,0,0,0,0,1,0,0,0,0,0,0,0,'Fulgorge - At 20 - 60 Yards Range - Set Phase 2 (Phase 1)'),
(@ENTRY,0,11,12,9,2,100,0,0,8,0,0,28,34038,0,0,0,0,0,1,0,0,0,0,0,0,0,'Fulgorge - At 0 - 8 Yards - Remove Submerge Visual (Phase 2)'),
(@ENTRY,0,12,13,61,2,100,0,0,0,0,0,21,0,0,0,0,0,0,1,0,0,0,0,0,0,0,'Fulgorge - Combat - Prevent Combat Movement (Phase 2)'),
(@ENTRY,0,13,14,61,2,100,0,0,8,0,0,19,33554434,0,0,0,0,0,1,0,0,0,0,0,0,0,'Fulgorge - At 0 - 8 Yards - Remove Unselectable and Unattackable Flags (Phase 2)'),
(@ENTRY,0,14,0,61,2,100,0,0,0,0,0,22,1,0,0,0,0,0,1,0,0,0,0,0,0,0,'Fulgorge - At 20 - 60 Yards Range - Set Phase 1 (Phase 2)');

-- Deranged Helboar SAI
SET @ENTRY  := 16863; -- NPC entry
SET @SPELL1 := 33908; -- Burning Spikes
SET @SPELL2 := 8599;  -- Enrage
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry`=@ENTRY;
DELETE FROM `creature_ai_scripts` WHERE `creature_id`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid`=@ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,0,11,0,100,0,0,0,0,0,11,@SPELL1,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Deranged Helboar - On spawn - Cast Burning Spikes on self'),
(@ENTRY,0,1,0,2,0,100,1,0,30,0,0,11,@SPELL2,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Deranged Helboar - On Health level - Cast Enrage When Below 30% HP');

-- Stonescythe Alpha SAI
SET @ENTRY  := 16929; -- NPC entry
SET @SPELL1 := 33911; -- Tear Armor
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry`=@ENTRY;
DELETE FROM `creature_ai_scripts` WHERE `creature_id`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid`=@ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,0,0,0,50,0,3000,3000,8000,8000,11,@SPELL1,0,0,0,0,0,2,0,0,0,0,0,0,0, 'Stonescythe Alpha - Combat - Cast Tear Armor');

 
 
-- -------------------------------------------------------- 
-- 2011_06_25_04_world_achievement_criteria_data.sql 
-- -------------------------------------------------------- 
DELETE FROM `disables` WHERE `entry`=7593 AND `sourceType`=4;

DELETE FROM `achievement_criteria_data` WHERE `criteria_id`=7593 AND `type`=11;
INSERT INTO `achievement_criteria_data` (`criteria_id`,`type`,`value1`,`value2`,`ScriptName`)
VALUES
(7593,11,0,0,'achievement_abuse_the_ooze');
 
 
-- -------------------------------------------------------- 
-- 2011_06_25_05_world_achievement_criteria_data.sql 
-- -------------------------------------------------------- 
DELETE FROM `disables` WHERE `entry`=7579 AND `sourceType`=4;

DELETE FROM `achievement_criteria_data` WHERE `criteria_id`=7579 AND `type`=11;
INSERT INTO `achievement_criteria_data` (`criteria_id`,`type`,`value1`,`value2`,`ScriptName`)
VALUES
(7579,11,0,0,'achievement_consumption_junction');
 
 
-- -------------------------------------------------------- 
-- 2011_06_25_06_world_achievement_criteria_data.sql 
-- -------------------------------------------------------- 
DELETE FROM `achievement_criteria_data` WHERE `criteria_id`=12976 AND `type`=11;
INSERT INTO `achievement_criteria_data` (`criteria_id`,`type`,`value1`,`value2`,`ScriptName`)
VALUES
(12976,11,0,0,'achievement_three_faced');
 
 
-- -------------------------------------------------------- 
-- 2011_06_25_07_world_achievement_criteria_data.sql 
-- -------------------------------------------------------- 
DELETE FROM `disables` WHERE `entry`=7359 AND `sourceType`=4;

DELETE FROM `achievement_criteria_data` WHERE `criteria_id`=7359 AND `type`=11;
INSERT INTO `achievement_criteria_data` (`criteria_id`,`type`,`value1`,`value2`,`ScriptName`)
VALUES
(7359,11,0,0,'achievement_volunteer_work');
 
 
-- -------------------------------------------------------- 
-- 2011_06_25_08_world_achievement_criteria_data.sql 
-- -------------------------------------------------------- 
UPDATE `achievement_criteria_data` SET `ScriptName`='achievement_brann_spankin_new' WHERE `ScriptName`='achievement_brann_sparklin_news';
 
 
-- -------------------------------------------------------- 
-- 2011_06_25_09_world_achievement_criteria_data.sql 
-- -------------------------------------------------------- 
DELETE FROM `disables` WHERE `entry` IN (7328,7329,7330,7331,7332,7333) AND `sourceType`=4;

DELETE FROM `achievement_criteria_data` WHERE `criteria_id` IN (7328,7329,7330,7331,7332,7333) AND `type`=11;
INSERT INTO `achievement_criteria_data` (`criteria_id`,`type`,`value1`,`value2`,`ScriptName`)
VALUES
(7328,11,0,0,'achievement_twilight_assist'),
(7329,11,0,0,'achievement_twilight_duo'),
(7330,11,0,0,'achievement_twilight_zone'),
(7331,11,0,0,'achievement_twilight_assist'),
(7332,11,0,0,'achievement_twilight_duo'),
(7333,11,0,0,'achievement_twilight_zone');
 
 
-- -------------------------------------------------------- 
-- 2011_06_25_10_world_achievement_criteria_data.sql 
-- -------------------------------------------------------- 
DELETE FROM `disables` WHERE `entry`=7315 AND `sourceType`=4;

DELETE FROM `achievement_criteria_data` WHERE `criteria_id`=7315 AND `type`=11;
INSERT INTO `achievement_criteria_data` (`criteria_id`,`type`,`value1`,`value2`,`ScriptName`)
VALUES
(7315,11,0,0,'achievement_intense_cold');

DELETE FROM `spell_script_names` WHERE `spell_id`=48095;
INSERT INTO `spell_script_names` (`spell_id`,`ScriptName`)
VALUES
(48095,'spell_intense_cold');
 
 
-- -------------------------------------------------------- 
-- 2011_06_25_11_world_creature_template.sql 
-- -------------------------------------------------------- 
UPDATE `creature_template` SET `ScriptName`='npc_pool_of_tar' WHERE `entry`=33090;
 
 
-- -------------------------------------------------------- 
-- 2011_06_25_12_world_trinity_string.sql 
-- -------------------------------------------------------- 
UPDATE `trinity_string` SET `content_default`='You try to view cinematic %u but it doesn\'t exist.' WHERE entry='1200';
 
 
-- -------------------------------------------------------- 
-- 2011_06_25_13_world_achievement_criteria_data.sql 
-- -------------------------------------------------------- 
DELETE FROM `conditions` WHERE `SourceEntry`=24311 AND `ConditionValue2` IN (14825,14986,14826,14883); 
 
-- -------------------------------------------------------- 
-- 2011_06_25_14_world_spell_proc_event.sql 
-- -------------------------------------------------------- 
DELETE FROM `spell_proc_event` WHERE `entry` = 70817;
INSERT INTO `spell_proc_event` VALUES
(70817,0,11,0,0x1000,0,0x10000,0,0,0,0);
 
 
-- -------------------------------------------------------- 
-- 2011_06_25_15_world_achievement_criteria_data.sql 
-- -------------------------------------------------------- 
DELETE FROM `disables` WHERE `entry` IN (10056,10057,10058,10059,10060,10061,10218,10219) AND `sourceType`=4;

DELETE FROM `achievement_criteria_data` WHERE `criteria_id` IN (10056,10057,10058,10059,10060,10061,10218,10219) AND `type`=11;
INSERT INTO `achievement_criteria_data` (`criteria_id`,`type`,`value1`,`value2`,`ScriptName`)
VALUES
(10056,11,0,0,'achievement_orbital_bombardment'),
(10057,11,0,0,'achievement_orbital_devastation'),
(10058,11,0,0,'achievement_nuked_from_orbit'),
(10059,11,0,0,'achievement_orbital_bombardment'),
(10060,11,0,0,'achievement_orbital_devastation'),
(10061,11,0,0,'achievement_nuked_from_orbit'),
(10218,11,0,0,'achievement_orbit_uary'),
(10219,11,0,0,'achievement_orbit_uary');
 
 
-- -------------------------------------------------------- 
-- 2011_06_25_16_world_spell_proc_event.sql 
-- -------------------------------------------------------- 
DELETE FROM `spell_proc_event` WHERE `entry` = 63086;
INSERT INTO `spell_proc_event` VALUES 
(63086, 0x00, 9, 0x00000000, 0x00000000, 0x00010000, 0x00000000, 0x00000000, 0, 0, 0);
 
 
-- -------------------------------------------------------- 
-- 2011_06_26_00_world_spell_script_names.sql 
-- -------------------------------------------------------- 
DELETE FROM `spell_script_names` WHERE `ScriptName`='spell_deathbringer_mark_of_the_fallen_champion';
 
 
-- -------------------------------------------------------- 
-- 2011_06_26_01_world_command.sql 
-- -------------------------------------------------------- 
DELETE FROM `command` WHERE `name`='pet tp';
 
 
-- -------------------------------------------------------- 
-- 2011_06_26_02_world_spell_script_names.sql 
-- -------------------------------------------------------- 
DELETE FROM `spell_script_names` WHERE `ScriptName`='spell_deathbringer_boiling_blood';
INSERT INTO `spell_script_names` (`spell_id`,`ScriptName`) VALUES
(72385,'spell_deathbringer_boiling_blood'),
(72441,'spell_deathbringer_boiling_blood'),
(72442,'spell_deathbringer_boiling_blood'),
(72443,'spell_deathbringer_boiling_blood');
 
 
-- -------------------------------------------------------- 
-- 2011_06_26_03_world_spell_dbc.sql 
-- -------------------------------------------------------- 
UPDATE `spell_dbc` SET `Effect1`=28, `EffectMiscValueB1`=64 WHERE `Id`=24308;
 
 
-- -------------------------------------------------------- 
-- 2011_06_26_04_world_spell_proc.sql 
-- -------------------------------------------------------- 
DROP TABLE IF EXISTS `spell_proc`;
CREATE TABLE `spell_proc` (
  `spellId` mediumint(8) NOT NULL DEFAULT '0',
  `schoolMask` tinyint(4) NOT NULL DEFAULT '0',
  `spellFamilyName` smallint(5) unsigned NOT NULL DEFAULT '0',
  `spellFamilyMask0` int(10) unsigned NOT NULL DEFAULT '0',
  `spellFamilyMask1` int(10) unsigned NOT NULL DEFAULT '0',
  `spellFamilyMask2` int(10) unsigned NOT NULL DEFAULT '0',
  `typeMask` int(10) unsigned NOT NULL DEFAULT '0',
  `spellTypeMask` int(10) unsigned NOT NULL DEFAULT '0',
  `spellPhaseMask` int(10) NOT NULL DEFAULT '0',
  `hitMask` int(10) NOT NULL DEFAULT '0',
  `attributesMask` int(10) unsigned NOT NULL DEFAULT '0',
  `ratePerMinute` float NOT NULL DEFAULT '0',
  `chance` float NOT NULL DEFAULT '0',
  `cooldown` float unsigned NOT NULL DEFAULT '0',
  `charges` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`spellId`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
 
 
-- -------------------------------------------------------- 
-- 2011_06_26_05_world_command.sql 
-- -------------------------------------------------------- 
DELETE FROM `command` WHERE `name` = 'reload spell_proc';
INSERT INTO `command` VALUES
('reload spell_proc',3,'Syntax: .reload spell_proc\nReload spell_proc table.');
 
 
-- -------------------------------------------------------- 
-- 2011_06_26_06_world_achievement_criteria_data.sql 
-- -------------------------------------------------------- 
DELETE FROM `achievement_criteria_data` WHERE `criteria_id` IN (10447,10448,10449,10459,10460,10461) AND `type` IN (11,18);
INSERT INTO `achievement_criteria_data` (`criteria_id`,`type`,`value1`,`value2`,`ScriptName`)
VALUES
(10447,18,0,0,''),
(10448,18,0,0,''),
(10449,18,0,0,''),
(10459,18,0,0,''),
(10460,18,0,0,''),
(10461,18,0,0,'');
 
 
-- -------------------------------------------------------- 
-- 2011_06_26_07_world_spell_script_names.sql 
-- -------------------------------------------------------- 
DELETE FROM `spell_script_names` WHERE `spell_id`=66218;
INSERT INTO `spell_script_names` (`spell_id`,`ScriptName`) VALUES (66218,'spell_gen_launch'); 
 
-- -------------------------------------------------------- 
-- 2011_06_27_00_world_sai.sql 
-- -------------------------------------------------------- 
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry` IN (33354,33355,33430,33431,33525,33526,33527,33528);

DELETE FROM `smart_scripts` WHERE `entryorguid` IN (33354,33355,33430,33431,33525,33526,33527,33528) AND `event_type`=6;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`)
VALUES
(33354,0,0,0,6,0,100,0,0,0,0,0,34,2,1,0,0,0,0,19,32906,0,0,0,0,0,0,'Corrupted Servitor - on death set instance data for achievement Con-Speed-Atory'),
(33355,0,0,0,6,0,100,0,0,0,0,0,34,2,1,0,0,0,0,19,32906,0,0,0,0,0,0,'Misguided Nymph - on death set instance data for achievement Con-Speed-Atory'),
(33430,0,0,0,6,0,100,0,0,0,0,0,34,2,1,0,0,0,0,19,32906,0,0,0,0,0,0,'Guardian Lasher - on death set instance data for achievement Con-Speed-Atory'),
(33431,0,0,0,6,0,100,0,0,0,0,0,34,2,1,0,0,0,0,19,32906,0,0,0,0,0,0,'Forest Swarmer - on death set instance data for achievement Con-Speed-Atory'),
(33525,0,0,0,6,0,100,0,0,0,0,0,34,2,1,0,0,0,0,19,32906,0,0,0,0,0,0,'Mangrove Ent - on death set instance data for achievement Con-Speed-Atory'),
(33526,0,0,0,6,0,100,0,0,0,0,0,34,2,1,0,0,0,0,19,32906,0,0,0,0,0,0,'Ironroot Lasher - on death set instance data for achievement Con-Speed-Atory'),
(33527,0,0,0,6,0,100,0,0,0,0,0,34,2,1,0,0,0,0,19,32906,0,0,0,0,0,0,'Natures\'s Blade - on death set instance data for achievement Con-Speed-Atory'),
(33528,0,0,0,6,0,100,0,0,0,0,0,34,2,1,0,0,0,0,19,32906,0,0,0,0,0,0,'Guardian of Life - on death set instance data for achievement Con-Speed-Atory');
 
 
-- -------------------------------------------------------- 
-- 2011_06_27_01_world_spell_script_names.sql 
-- -------------------------------------------------------- 
DELETE FROM `spell_script_names` WHERE `spell_id` IN(65266,65635,65636,66666,66667,66668);
INSERT INTO `spell_script_names` (`spell_id`,`ScriptName`) VALUES
(65266,'spell_gen_vehicle_scaling'),
(65635,'spell_gen_vehicle_scaling'),
(65636,'spell_gen_vehicle_scaling'),
(66666,'spell_gen_vehicle_scaling'),
(66667,'spell_gen_vehicle_scaling'),
(66668,'spell_gen_vehicle_scaling');
 
 
-- -------------------------------------------------------- 
-- 2011_06_27_02_world_vehicle_scaling_info.sql 
-- -------------------------------------------------------- 
DROP TABLE `vehicle_scaling_info`;
 
 
-- -------------------------------------------------------- 
-- 2011_06_27_03_world_achievement_criteria_data.sql 
-- -------------------------------------------------------- 
DELETE FROM `achievement_criteria_data` WHERE `criteria_id` IN (10447,10448,10449,10459,10460,10461) AND `type` IN (11,12,18);
INSERT INTO `achievement_criteria_data` (`criteria_id`,`type`,`value1`,`value2`,`ScriptName`)
VALUES
(10447,11,0,0,'achievement_knock_on_wood'),
(10447,12,0,0,''),
(10448,11,0,0,'achievement_knock_knock_on_wood'),
(10448,12,0,0,''),
(10449,11,0,0,'achievement_knock_knock_knock_on_wood'),
(10449,12,0,0,''),
(10459,11,0,0,'achievement_knock_on_wood'),
(10459,12,1,0,''),
(10460,11,0,0,'achievement_knock_knock_on_wood'),
(10460,12,1,0,''),
(10461,11,0,0,'achievement_knock_knock_knock_on_wood'),
(10461,12,1,0,'');

UPDATE `creature_template` SET `AIName`='' WHERE `entry` IN (33354,33355,33430,33431,33525,33526,33527,33528);



DELETE FROM `smart_scripts` WHERE `entryorguid` IN (33354,33355,33430,33431,33525,33526,33527,33528);
 
 
-- -------------------------------------------------------- 
-- 2011_06_27_04_world_achievement_criteria_data.sql 
-- -------------------------------------------------------- 
DELETE FROM `disables` WHERE `entry` IN (10088,10089,10418,10419,10420,10421);

DELETE FROM `achievement_criteria_data` WHERE `criteria_id` IN (10088,10089,10418,10419,10420,10421) AND `type`=11;
INSERT INTO `achievement_criteria_data` (`criteria_id`,`type`,`value1`,`value2`,`ScriptName`)
VALUES
(10088,5,58501,0,''),
(10089,5,58501,0,''),
(10418,5,58501,0,''),
(10419,5,58501,0,''),
(10420,5,58501,0,''),
(10421,5,58501,0,'');
 
 
-- -------------------------------------------------------- 
-- _MERGED.sql 
-- -------------------------------------------------------- 
 
 
