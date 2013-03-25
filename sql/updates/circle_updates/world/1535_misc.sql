﻿UPDATE `creature_text` SET `sound` = 17352 WHERE `entry` = 36597 AND `groupid` = 3;

DELETE FROM `creature_text` WHERE `entry` = 38995 AND `groupid` = 4;
INSERT INTO `creature_text` VALUES
(38995, 4, 0, 'The Lich King must fall!', 14, 0, 0, 0, 0, 17389, 'Highlord Tirion Fordring - SAY_TIRION_OUTRO_3');

DELETE FROM `locales_creature_text` WHERE `entry` = 38995 AND `textGroup` = 4;
DELETE FROM `locales_creature_text` WHERE `entry` IN (38579,36823,39217);
INSERT INTO `locales_creature_text` (`entry`,`textGroup`,`id`,`text_loc8`) VALUES
(38995, 4, 0, 'Король-Лич падет!'),
(38579, 0, 0, 'Наконец я свободен! Все кончено, сын мой. Настал час расплаты.'),
(38579, 1, 0, 'Поднимитесь, воины Света!'),
(36823, 0, 0, 'Вы пришли, чтобы вершить суд над Артасом? Чтобы уничтожить Короля-лича?'),
(36823, 1, 0, 'Вы не должны оказаться во власти Ледяной Скорби. Иначе, как и я, будете навек порабощены этим проклятым клинком.'),
(36823, 2, 0, 'Помогите мне уничтожить эти истерзанные души! Вместе мы вытянем силу из Ледяной Скорби и ослабим Короля-лича!'),
(39217, 0, 0, 'Вы пришли, чтобы вершить суд над Артасом? Чтобы уничтожить Короля-лича?'),
(39217, 1, 0, 'Вы не должны оказаться во власти Ледяной Скорби. Иначе, как и я, будете навек порабощены этим проклятым клинком.');
