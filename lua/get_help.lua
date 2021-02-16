--[[
- entropoy.mac
- help.lua
- 
- help output
-
--]]

require "common"
local arg = {...}
command = string.lower(arg[1])
option = string.lower(arg[2])


echo('Help for${sep}\aw/'..command..'')


--[ /debug --]
if command == 'debug' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w group \aw] [\a-w subject \aw]')
    echo(discord..'\aw/debug buff now')
  elseif option == 'h' then
    echo(discord..'spams the crap out of you with debug information. were you after something else?')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maDebug')
  end


--[ /cb --]
elseif command == 'cb' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w a|b|c|s|g|t|u \aw]')
  elseif option == 'h' then
    echo(discord..'a${lsep}force a cycle check for auras')
    echo(discord..'b${lsep}force a cycle check of beg buffs')
    echo(discord..'c${lsep}force a cycle check of crew buffs')
    echo(discord..'s${lsep}force a cycle check of self buffs')
    echo(discord..'g${lsep}force a cycle check of group (non dannet) buffs')
    echo(discord..'t${lsep}force a cycle check of temp buffs')
    echo(discord..'u${lsep}force a cycle check for the class unity aa buff')
 end


--[ /take --]
elseif command == 'take' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w broadcast|all|group|raid|zone \aw]')
  elseif option == 'h' then
    echo(discord..'Accepts, according to broadcast${sep}')
    echo(discord..'${dot}translocate, guild hall port, adventures, challenges, expeditions')
    echo(discord..'${dot}resurrections, highlighted task, trade window, shared task')
  end


--[ /port --]
elseif command == 'port' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then

  end

--[ /area --]
elseif command == 'area' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w option \aw] \a-wtarget\aw')
  elseif option == 'h' then
    echo(discord..'too many to put here. go see the wiki')
    echo(discord..'all but the safe command require a target, dead or alive is fine,')
    echo(discord..'to mark the mob in the zone database')
  end


--[ /home --]
elseif command == 'home' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo(discord..'set${lsep}set a home marker for your toon')
    echo(discord..'variance${lsep}a +/- range of acceptable variation in the homemarker')
    echo(discord..'arc${lsep}1-45 degree of acceptable facing range of the target')
    echo(discord..'campfire${lsep}places a campfire, the nice one if you have components')
    echo(discord..'deathclear${lsep}release the home set marker on a death')
    echo(discord..'facefast${lsep}faces a target fast w/o turning')
    echo(discord..'incombat${lsep}toon will try to keep their campe even in combat')
    echo(discord..'onauto${lsep}enables home camp setting whenever auto is turned on')
    echo(discord..'position${lsep}movement to [free|left|right|face|rear] of the target')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maHome')
  end


--[ /cc --]
elseif command == 'cc' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo(discord..'alliance${lsep}use class alliances')
    echo(discord..'ass${lsep}assist settings')
    echo(discord..'engage${lsep}% of mobs health to engage in combat')
    echo(discord..'force${lsep}manual mode force engage a tgarget')
    echo(discord..'glyph${lsep}glyph use settings')
    echo(discord..'kill${lsep}shortcut to /override engage')
    echo(discord..'mode${lsep}types of combat to engage in [melee|nuke|dot|range|heal|cc')
    echo(discord..'stoponds${lsep}no melee on a mob with a damage shield')
    echo(discord..'zradius${lsep}Z radius for assist')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maCC')
  elseif option == 'ccasserror' then
    echo(discord..'${notice} Smart Assits are ${eol}')
    echo(discord..'${dot} \awg\ax = Group Roll${sep}\a-wGroup Main Assist\ax')
    echo(discord..'${dot} \aw1\ax = Raid Assist${sep}\a-w1\ax')
    echo(discord..'${dot} \aw2\ax = Raid Assist${sep}\a-w2\ax')
    echo(discord..'${dot} \aw3\ax = Raid Assist${sep}\a-w3\ax')
    echo(discord..'${dot} \awoff\ax = Turn off all Smart Assist')
  end


--[ /pull --]
elseif command == 'pull' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo(discord..'active${lsep}puling on or off?')
    echo(discord..'chain${lsep}when only this many mobs in camp, seconday puller goes and gets more')
    echo(discord..'mode${lsep}how you want to pull')
    echo(discord..'navstop${lsep}stop this far from a mob then attempt to pull')
    echo(discord..'outrun${lsep}get this far from a mob, turn walk on')
    echo(discord..'pathlogic${lsep}use nav path distance to find targets')
    echo(discord..'rad${lsep}y/x radius to pull in')
    echo(discord..'sethome${lsep}auto set a home marker before pulling')
    echo(discord..'stop${lsep}reasons to stop pulling?')
    echo(discord..'with${lsep}list of things you can pull with')
    echo(discord..'zrad${lsep}z radius to pull in')

  elseif option == 's' then
    mq.cmd.call('Bind_command_see maPull')
  end


--[ /rez --]
elseif command == 'rez' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo(discord..'dannet${lsep}rez anyone in your dannet')
    echo(discord..'divine${lsep}rez clerics with Divine Rez AA')
    echo(discord..'everyone${lsep}rez everyone.. fuck it')
    echo(discord..'ic${lsep}rez others while in combat')
    echo(discord..'ooc${lsep}rez others while out of combat')
    echo(discord..'pct${lsep}min % rez to take')
    echo(discord..'rad${lsep}radius to watch for corpses to rez')
    echo(discord..'take${lsep}take rezes')
    echo(discord..'takecall${lsep}take calls')
    echo(discord..'token${lsep}rez with tokens')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maRez')
  end


--[ /minion --]
elseif command == 'minion' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo(discord..'buff${lsep}enable buffing YOUR pet (read not other pets)')
    echo(discord..'build${lsep}forces a rebuild/check of pet and buffs')
    echo(discord..'ds${lsep}attack a mob with a ds.. or dont')
    echo(discord..'engage${lsep}mob health % to send pet in')
    echo(discord..'gear${lsep}what weapons to give pet')
    echo(discord..'illusion${lsep}pet illusion?')
    echo(discord..'onforce${lsep}sent pet in when /cc force is used')
    echo(discord..'pet${lsep}use pets')
    echo(discord..'sengage${lsep}$ of mobs health to use swarm pets')
    echo(discord..'shrink${lsep}yes, you should shrink your damn pet')
    echo(discord..'swarm${lsep}use swarm pets')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maMinion')
  end


--[ /heal --]
elseif command == 'heal' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo(discord..'adj${lsep}adjust max health for detremental zones')
    echo(discord..'break${lsep}break heal if over healpoint')
    echo(discord..'ch${lsep}single ch healing')
    echo(discord..'chain${lsep}ch chain rotation (in progress)')
    echo(discord..'dannet${lsep}use dannet healing')
    echo(discord..'group${lsep}use group healing')
    echo(discord..'groupcount${lsep}# in group hurt to use grouphot')
    echo(discord..'grouphot${lsep}use group heal over time')
    echo(discord..'hot${lsep}use single heal over time')
    echo(discord..'pet${lsep}heal pets in group.. why?')
    echo(discord..'point${lsep}set heal points by class')
    echo(discord..'promise${lsep}use promise healing')
    echo(discord..'rampage${lsep}name of rampage tank')
    echo(discord..'self${lsep}heal yourself (not needed with group healing)')
    echo(discord..'splash${lsep}# of mobs in /env rad to use splash')
    echo(discord..'weight${lsep}calculate the worst hurt by healpoint settings')
    echo(discord..'xt${lsep}heal people on xtarget')
    echo(discord..'xtclass${lsep}set xtarget classes to heal')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maHeal')
  end


--[ /cure --]
elseif command == 'cure' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo(discord..'crew${lsep}cure all your dannet peoples (not you)')
    echo(discord..'group${lsep}cure your group (not you)')
    echo(discord..'self${lsep}cure yourself only')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maCure')
  end


--[ /nuke --]
elseif command == 'nuke' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo(discord..'delay${lsep}delay (1/10 second) between nukes')
    echo(discord..'loop${lsep}forces a complete nuke loop cycle regardless of cooldowns')
    echo(discord..'pctstop${lsep}targets health % to stop nuking it')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maNuke')
  end


--[ /agro --]
elseif command == 'agro' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo(discord..'active${lsep}use class agro skills (\a-wnot taunt\ax)')
    echo(discord..'hold${lsep}minimum agro % you want to try to hold')
    echo(discord..'taunt${lsep}minimum # agro to use taunt')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maAgro')
  end


--[ /dot --]
elseif command == 'dot' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo(discord..'refresh${lsep}time (in seconds) left on a dot to force a refresh')
    echo(discord..'pctstop${lsep}targets health % to stop dots')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maDoT')
  end
  
  
--[ /defense --]
elseif command == 'defense' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo(discord..'count${lsep}# of mobs in your immediate are to activate')
    echo(discord..'pct${lsep}percent of your health to activate')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maDefense')
  end


--[ /clickitem --]
elseif command == 'clickitem' then
  if option == 'u' then
    echo(discord..'\aw/'..command..'')
  elseif option == 'h' then
    echo(discord..'there are no options to /clickitem')
    echo(discord..'/clickitem is only used diring active combat with a valid target')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maItem')
  end
  
  
--[ /miscdps --]
elseif command == 'miscdps' then
  if option == 'u' then
    echo(discord..'\aw/'..command..'')
  elseif option == 'h' then
    echo(discord..'Engine 2/3 settings only. see wiki')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maMiscDPS')
  end


--[ /song --]
elseif command == 'song' then
  if option == 'u' then
    echo(discord..'\aw/'..command..'')
  elseif option == 'h' then
    echo(discord..'no additional options')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maSong')
  end
  
  
--[ /env --]
elseif command == 'env' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo(discord..'lots of options. see the wiki')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maEnv')
  end


--[ /beg --]
elseif command == 'beg' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w alias \aw]')
  elseif option == 'h' then
    echo(discord..'broadcasts a request to a general queue that any')
    echo(discord..'character you box can respond too')
    echo(discord..'e.g. \a-w/beg haste\ax would beg for haste')
    echo(discord..'the first class with haste set as an alias as a')
    echo(discord..'buff would answer')
  end


--[ /on --]
elseif command == 'on' then
  if option == 'u' then
    echo(discord..'\aw/'..command..'')
  elseif option == 'h' then
    echo(discord..'turns all toons to \a-w/env auto on\ax except you')
  end
  

--[ /off --]
elseif command == 'off' then
  if option == 'u' then
    echo(discord..'\aw/'..command..'')
  elseif option == 'h' then
    echo(discord..'turns all toons to \a-w/env auto off\ax except you')
  end
  

--[ /navto --]
elseif command == 'navto' then
  if option == 'u'then
    echo(discord..'\aw/'..command..' [\a-w name|target|yxz loc|wp \aw] [\a-w nickname \aw]')
  elseif option == 'h' then
    echo(discord..'navs to something in that zone; a name, partial name, a set waypoint, a YXZ loc')
  end


--[ /grab --]
elseif command == 'grab' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w name|partialname \aw]')
  elseif option == 'h' then
    echo(discord..'will pick up a ground spawn that is within 20 paces of you')
  end


--[ /here --]
elseif command == 'here' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w broadcast|all|group|raid|zone \aw]')
  elseif option == 'h' then
    echo(discord..'oh shit command, sets all toons (except you) to:')
    echo(discord..'${dot} instant combat ready')
    echo(discord..'${dot} removes all invis')
    echo(discord..'${dot} delays all timers')
    echo(discord..'${dot} all toons to /env auto on')
  end


--[ /enter --]
elseif command == 'enter' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w broadcast|all|group|raid|zone \aw]')
  elseif option == 'h' then
    echo(discord..'attemts to activate any lever or click to enter portal within 40 paces')
  end


--[ /alto --]
elseif command == 'alto' then
  if option == 'u' then
    echo(discord..'\aw/'..command..'')
  elseif option == 'h' then
    echo(discord..'basically, a stop everything command. keep in mind, if you are in auto, you will go right')
    echo(discord..'back to whatever you were doing')
  end


--[ /invis --]
elseif command == 'invis' then
  if option == 'u' then
    echo(discord..'\aw/'..command..'')
  elseif option == 'h' then
    echo(discord..'used for stacking invis')
    echo(discord..'WIZ,MAG,ENC,SHM,RNG,DRU${lsep}use \a-w/chr inv group\ax setting')
    echo(discord..'CLR,SHD,NEC,PAL${lsep}use \a-w/chr inv groupivu\ax setting')
  end
  
  
--[ /repeat --]
elseif command == 'repeat' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' "text to repeat"')
  elseif option == 'h' then
    echo(discord..'tells everyone within 60 pases of you to:')
    echo(discord..'delay 1-3 seconds')
    echo(discord..'target the intended mob')
    echo(discord..'repeat what you say')
  end


--[ /grain --]
elseif command == 'grain' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo(discord..'all outputs in entropy are associated with a #')
    echo(discord..'you can turn off that output type by turning that # off')
    echo(discord..'some outputs are forced, deal with it')
    echo(discord..'\a-w/grain list\ax to see them all')
  end


--[ /crew --]
elseif command == 'crew' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo(discord..'control group settings and builds')
    echo(discord..'see wiki for details')
  end

--[ /sash --]
elseif command == 'burn' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo(discord..'active${lsep} use bandolier swapping')
    echo(discord..'flash${lsep} [tanks] shield flash settings')
    echo(discord..'proc${lsep}weapons to swap to until they proc on the target')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maSash')
  end
    

--[ /burn --]
elseif command == 'burn' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo(discord..'los${lsep}line of sight requirements')
    echo(discord..'auto${lsep}automatic burn on named mobs')
    echo(discord..'count${lsep}number of mobs in your /env rad to start burns')
    echo(discord..'engage${lsep}% of mobs health to start burning')
    echo(discord..'force${lsep}force burning 100% of the time')
    echo(discord..'raid${lsep}burn on raids or not')
    echo(discord..'spinup${lsep}one shot burn startup. as much as possible w/o a target')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maBurn')
  end
  

--[ /override --]
elseif command == 'override' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo(discord..'los${lsep}line of sight requirements')
    echo(discord..'engage${lsep}mob HP engage pct nad auto engage at 100%')
    echo(discord..'loot${lsep}try to loot regardless of advanced loot settings')
    echo(discord..'safenames${lsep}safename requirements. accept anyone')
    echo(discord..'trainspell${lsep}mode trainspell skill limits and cast forever')
    echo(discord..'dscheck${lsep}damage shield chech for melee combat')
    echo(discord..'envauto${lsep}start the macro with the character in auto')
    echo(discord..'groupass${lsep}override yourself as groupassist and force manual assist')
    echo(discord..'splashhurt${lsep}allows clerics to ignore bypass hurtchecks for splashing to keep it on forced cooldown')
    echo(discord..'targetclear${lsep}removes target clearing requirement')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maOver')
  end


--[ /cursor --]
elseif command == 'cursor' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w option \aw] [ITEMONCURSOR]')
  elseif option == 'h' then
    echo(discord..'destroy${lsep}set item as destroy')
    echo(discord..'ignore${lsep}set item as ignore')
    echo(discord..'keep${lsep}set item as keep [system default]')
    echo(discord..'leave${lsep}set item as leave on corpse')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maBurn')
  end


--[ /tc --]
elseif command == 'tc' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo(discord..'break${lsep}"attempt" to stop everything to start tie')
    echo(discord..'mode${lsep}method to use for tie [\a-w nav|stick \ax]')
    echo(discord..'toon${lsep}name of the character you want to tie too')
    echo(discord..'variance${lsep}min distance to try and keep to tie toon')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maTie')
  end
  

--[ /tie --]
elseif command == 'tie' then
  if option == 'u' then
    echo(discord..'\aw/'..command..'')
  elseif option == 'h' then
    echo(discord..'enables or disables tie')
  end

  
--[ /build --]
elseif command == 'build' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w save|load|set \ax] "\a-wnick name\ax" [\a-w all \ax]')
  elseif option == 'h' then
    echo(discord..'too much to explain here. see the wiki...')
  end


--[ /mode --]
elseif command == 'mode' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w option \ax]')
  elseif option == 'h' then
    echo(discord..'drag${lsep}drags all the dead people')
    echo(discord..'farm${lsep}see wiki. used for repeating missions')
    echo(discord..'fish${lsep}golfing with beer')
    echo(discord..'forage${lsep}looks through the trash for used tp..')
    echo(discord..'harvest${lsep}picks things up off ground')
    echo(discord..'hunt${lsep}kills all the mobs')
    echo(discord..'lush${lsep}Moes Tavern!')
    echo(discord..'petfarm${lsep}stand there while your pet does all the work')
    echo(discord..'summon${lsep}summons your corpse in lobby')
    echo(discord..'temppet${lsep}[wix|cler] temp pet farming')
    echo(discord..'trainskill${lsep}trains skills. use on pet in a hall')
    echo(discord..'trainspell${lsep}casts spells.. a lot')
    echo(discord..'data${lsep}info needed for other modes such as harvest and hunt')
    echo(discord..'stop${lsep}stops all modes')
  end


--[ /ttl --]
elseif command == 'ttl' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo(discord..'logic${lsep}use ttl logic for dps')
    echo(discord..'clear${lsep}resets current collected data')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maTTL')
  end


--[ /pred --]
elseif command == 'pred' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo(discord..'logic${lsep}enable or disable using preditcion logic')
    echo(discord..'prepburn${lsep}start burn routines')
    echo(discord..'preclass${lsep}start class specific prediction logic')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maPred')
  end


--[ /lesson --]
elseif command == 'lesson' then
  if option == 'u' then
    echo(discord..'\aw/'..command..'')
  elseif option == 'h' then
    echo(discord..'casts AA Lesson of the Devoted')
  end


--[ /cls --]
elseif command == 'cls' then
  if option == 'u' then
    echo(discord..'\aw/'..command..'')
  elseif option == 'h' then
    echo(discord..'clears your screen of all corpses:')
    echo(discord..'sets /hidecorpse looted on')
    echo(discord..'sets /hidecorpse npc on')
  end

  
--[ /see --]
elseif command == 'see' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' mapname or listname')
  elseif option == 'h' then
    echo(discord..'outputs the content of a map or list')
  end
  
  
--[ /coh --]
elseif command == 'coh' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w group \aw]')
  elseif option == 'h' then
    echo(discord..'/coh will request a call from a mage in your group')
    echo(discord..'/coh group will request a mage in your group to call the entire group')
  end


--[ /buff --]
elseif command == 'buff' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo(discord..'master${lsep}the master switch')
    echo(discord..'ammo${lsep}things like arrows, axes, small rocks, whatever')
    echo(discord..'aura${lsep}feed the cat.. i dunno')
    echo(discord..'beg${lsep}answer beg buffing requests')
    echo(discord..'crew${lsep}buff your dannet people')
    echo(discord..'drop${lsep}auto drop buffs from lsDropBuffs')
    echo(discord..'group${lsep}buff mercs and people you dont box')
    echo(discord..'now${lsep}self 24/7 always up buffs')
    echo(discord..'raid${lsep}buff raid members')
    echo(discord..'self${lsep}buf yourself')
    echo(discord..'shrink${lsep}yes, you should shrink')
    echo(discord..'temp${lsep}temp buffs in vcombat for other players')
    echo(discord..'unity${lsep}your unity aa buff')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maBuff')
  end


--[ /chr --]
elseif command == 'chr' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo(discord..'/chr options will varry based off class')
    echo(discord..'shared commands are:')
    echo(discord..'ae${lsep}# of mobs in /env rad to start using AE stuffs')
    echo(discord..'fade${lsep}% of health to try and fade an attacker off')
    echo(discord..'intensity${lsep}settings for AA Intensity of the Resolute')
    echo(discord..'invis${lsep}settings for the types if invisibilities you want to use')
    echo(discord..'spire${lsep}how to use your class spire')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maChr')
  end


--[ /debuff --]
elseif command == 'debuff' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo(discord..'${lsep}')
  elseif option == 's' then
    echo(discord..'cripple${lsep}use defined cripples')
    echo(discord..'eradicate${lsep}use defined eradicate')
    echo(discord..'malo${lsep}use defined malos')
    echo(discord..'mez${lsep}use defined mezes')
    echo(discord..'slow${lsep}use defined slows')
    echo(discord..'snare${lsep}use defined snares')
    echo(discord..'tash${lsep}use defined snares')
  end
  
  
--[ /gtfo --]
elseif command == 'gtfo' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo(discord..'active${lsep}enable get the fuck out logic')
    echo(discord..'dead${lsep}# in your group dead to bail')
    echo(discord..'force${lsep}manual force run and hide')
    echo(discord..'spawns${lsep}# of mobs in your /env rad to RUNNOFT')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maGTFO')
  end
  

--[ /melee --]
elseif command == 'melee' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo(discord..'/melee has no options. just set the skills you want to use')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maMelee')
  end
  

--[ /nomore --]
elseif command == 'nomore' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo(discord..'invis${lsep}remove all invis buffs')
    echo(discord..'levi${lsep}remove all levi buffs')
    echo(discord..'removeall${lsep}remove all buffs')
    echo(discord..'spa##${lsep}remove al buffs with spa NAME or ##')
  end 


--[ /edit --]
elseif command == 'edit' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w many options... \aw]')
  elseif option == 'h' then
    echo(discord..'allows for raw editing of any variable, list or map')
    echo(discord..'see wiki for full details')
  end


end






