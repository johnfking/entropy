--[[
entropoy.mac
help.lua
 
help output
--]]



require "common"
local arg = {...}
command = string.lower(arg[1])
option = string.lower(arg[2])



--[ help titles --]
if option == 'u' then
  echo('Usage for${sep}\aw/'..command..'')
elseif option == 'h' then
  echo('Help for${sep}\aw/'..command..'')
elseif option == 's' then
  echo('Map Contents for${sep}\aw/'..command..'')
elseif option == 't' then
  echo('Tags for${sep}\aw/'..command..'')
end


--[ /debug --]
if command == 'debug' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w group \aw] [\a-w subject \aw]')
    echo('\aw/debug buff now')
  elseif option == 'h' then
    echo('spams the crap out of you with debug information. were you after something else?')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maDebug')
  elseif option == 't' then
    out('notag')
  end


--[ /cb --]
elseif command == 'cb' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w a|b|c|s|g|t|u \aw]')
  elseif option == 'h' then
    echo('a${lsep}force a cycle check for auras')
    echo('b${lsep}force a cycle check of beg buffs')
    echo('c${lsep}force a cycle check of crew buffs')
    echo('s${lsep}force a cycle check of self buffs')
    echo('g${lsep}force a cycle check of group (non dannet) buffs')
    echo('t${lsep}force a cycle check of temp buffs')
    echo('u${lsep}force a cycle check for the class unity aa buff')
    echo('xx${lsep}force auras, self and unity buff cycles')
    echo('robe${lsep}plate robe ornament bug, use:\a-ystOrnamentRobe\ax for the robe to swap out')
  elseif option == 's' then
    out('nomap')
  elseif option == 't' then
    out('notag')
 end


--[ /take --]
elseif command == 'take' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w broadcast|all|group|raid|zone \aw]')
  elseif option == 'h' then
    echo('Accepts, according to broadcast${sep}')
    echo('${dot}translocate, guild hall port, adventures, challenges, expeditions')
    echo('${dot}resurrections, highlighted task, trade window, shared task')
  elseif option == 's' then
    out('nomap')
  elseif option == 't' then
    out('notag')
  end


--[ /port --]
elseif command == 'port' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo('${lsep}port help goes here /sigh')
  elseif option == 's' then
    out('nomap')
  elseif option == 't' then
    out('notag')
  end


--[ /area --]
elseif command == 'area' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w option \aw] \a-wtarget\aw')
  elseif option == 'h' then
    echo('too many to put here. go see the wiki')
    echo('all but the safe command require a target, dead or alive is fine,')
    echo('to mark the mob in the zone database')
  elseif option == 's' then
    out('nomap')
  elseif option == 't' then
    out('notag')
  end


--[ /home --]
elseif command == 'home' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo('set${lsep}set a home marker for your toon')
    echo('variance${lsep}a +/- range of acceptable variation in the homemarker')
    echo('arc${lsep}1-45 degree of acceptable facing range of the target')
    echo('campfire${lsep}places a campfire, the nice one if you have components')
    echo('deathclear${lsep}release the home set marker on a death')
    echo('facefast${lsep}faces a target fast w/o turning')
    echo('incombat${lsep}toon will try to keep their campe even in combat')
    echo('onauto${lsep}enables home camp setting whenever auto is turned on')
    echo('position${lsep}movement to [free|left|right|face|rear] of the target')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maHome')
  elseif option == 't' then
    out('notag')
  end


--[ /cc --]
elseif command == 'cc' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo('alliance${lsep}use class alliances')
    echo('ass${lsep}assist settings')
    echo('engage${lsep}% of mobs health to engage in combat')
    echo('force${lsep}manual mode force engage a tgarget')
    echo('glyph${lsep}glyph use settings')
    echo('kill${lsep}shortcut to /override engage')
    echo('mode${lsep}types of combat to engage in [melee|nuke|dot|range|heal|cc')
    echo('stoponds${lsep}no melee on a mob with a damage shield')
    echo('zradius${lsep}Z radius for assist')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maCC')
  elseif option == 't' then
    out('notag')
  elseif option == 'ccasserror' then
    echo('${notice} Smart Assits are ${eol}')
    echo('${dot} \awg\ax = Group Roll${sep}\a-wGroup Main Assist\ax')
    echo('${dot} \aw1\ax = Raid Assist${sep}\a-w1\ax')
    echo('${dot} \aw2\ax = Raid Assist${sep}\a-w2\ax')
    echo('${dot} \aw3\ax = Raid Assist${sep}\a-w3\ax')
    echo('${dot} \awoff\ax = Turn off all Smart Assist')
  end


--[ /pull --]
elseif command == 'pull' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo('active${lsep}puling on or off?')
    echo('chain${lsep}when only this many mobs in camp, seconday puller goes and gets more')
    echo('healcheck${lsep}check yourself for heals while pulling')
    echo('mode${lsep}how you want to pull base|pet|int|nav|multi|watch')
    echo('navstop${lsep}stop this far from a mob then attempt to pull')
    echo('navlos${lsep}require los on nav pull checks')
    echo('outrun${lsep}get this far from a mob, turn walk on')
    echo('pathlogic${lsep}use nav path distance to find targets')
    echo('petwatch${lsep}## radius to watch for pets to kill')
    echo('rad${lsep}y/x radius to pull in')
    echo('sethome${lsep}auto set a home marker before pulling')
    echo('step${lsep}## (0-10) steps to chop up env rad for pulling mobs?')
    echo('stop${lsep}reasons to stop pulling?')
    echo('with${lsep}list of things you can pull with')
    echo('zrad${lsep}z radius to pull in')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maPull')
  elseif option == 't' then
    mq.cmd.call('Bind_noparse_tag see pull')
  end


--[ /rez --]
elseif command == 'rez' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo('dannet${lsep}rez anyone in your dannet')
    echo('divine${lsep}rez clerics with Divine Rez AA')
    echo('everyone${lsep}rez everyone.. fuck it')
    echo('ic${lsep}rez others while in combat')
    echo('ooc${lsep}rez others while out of combat')
    echo('pct${lsep}min % rez to take')
    echo('rad${lsep}radius to watch for corpses to rez')
    echo('take${lsep}take rezes')
    echo('takecall${lsep}take calls')
    echo('token${lsep}rez with tokens')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maRez')
  elseif option == 't' then
    out('notag')
  end


--[ /minion --]
elseif command == 'minion' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo('buff${lsep}enable buffing YOUR pet (read not other pets)')
    echo('build${lsep}forces a rebuild/check of pet and buffs')
    echo('ds${lsep}attack a mob with a ds.. or dont')
    echo('engage${lsep}mob health % to send pet in')
    echo('gear${lsep}what weapons to give pet')
    echo('illusion${lsep}pet illusion?')
    echo('onforce${lsep}sent pet in when /cc force is used')
    echo('pet${lsep}use pets')
    echo('sengage${lsep}$ of mobs health to use swarm pets')
    echo('shrink${lsep}yes, you should shrink your damn pet')
    echo('swarm${lsep}use swarm pets')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maMinion')
  elseif option == 't' then
    out('notag')
  end


--[ /heal --]
elseif command == 'heal' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo('adj${lsep}adjust max health for detremental zones')
    echo('break${lsep}break heal if over healpoint')
    echo('ch${lsep}single ch healing')
    echo('chain${lsep}ch chain rotation (in progress)')
    echo('dannet${lsep}use dannet healing')
    echo('group${lsep}use group healing')
    echo('groupcount${lsep}# in group hurt to use grouphot')
    echo('grouphot${lsep}use group heal over time')
    echo('hot${lsep}use single heal over time')
    echo('pet${lsep}heal pets in group.. why?')
    echo('point${lsep}set heal points by class')
    echo('promise${lsep}use promise healing')
    echo('rampage${lsep}name of rampage tank')
    echo('self${lsep}heal yourself (not needed with group healing)')
    echo('splash${lsep}# of mobs in /env rad to use splash')
    echo('twin${lsep}(clr,dru,shm) attempt to force twincast heals.')
    echo('weight${lsep}calculate the worst hurt by healpoint settings')
    echo('xt${lsep}heal people on xtarget')
    echo('xtclass${lsep}set xtarget classes to heal')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maHeal')
  elseif option == 't' then
    mq.cmd.call('Bind_noparse_tag see heal')
  end


--[ /cure --]
elseif command == 'cure' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo('crew${lsep}cure all your dannet peoples (not you)')
    echo('group${lsep}cure your group (not you)')
    echo('self${lsep}cure yourself only')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maCure')
  elseif option == 't' then
    mq.cmd.call('Bind_noparse_tag see cure')
  end


--[ /nuke --]
elseif command == 'nuke' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo('delay${lsep}delay (1/10 second) between nukes')
    echo('loop${lsep}forces a complete nuke loop cycle regardless of cooldowns')
    echo('pctstop${lsep}targets health % to stop nuking it')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maNuke')
  elseif option == 't' then
    mq.cmd.call('Bind_noparse_tag see nuke')
  end


--[ /agro --]
elseif command == 'agro' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo('active${lsep}use class agro skills (\a-wnot taunt\ax)')
    echo('hold${lsep}minimum agro % you want to try to hold')
    echo('taunt${lsep}minimum # agro to use taunt')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maAgro')
  elseif option == 't' then
    mq.cmd.call('Bind_noparse_tag see agro')
  end


--[ /dot --]
elseif command == 'dot' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo('refresh${lsep}time (in seconds) left on a dot to force a refresh')
    echo('pctstop${lsep}targets health % to stop dots')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maDoT')
  elseif option == 't' then
    mq.cmd.call('Bind_noparse_tag see dot')
  end
  
  
--[ /defense --]
elseif command == 'defense' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo('count${lsep}# of mobs in your immediate are to activate')
    echo('pct${lsep}percent of your health to activate')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maDefense')
  elseif option == 't' then
    mq.cmd.call('Bind_noparse_tag see defense')
  end


--[ /clickitem --]
elseif command == 'clickitem' then
  if option == 'u' then
    echo('\aw/'..command..'')
  elseif option == 'h' then
    echo('there are no options to /clickitem')
    echo('/clickitem is only used diring active combat with a valid target')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maItem')
  elseif option == 't' then
    mq.cmd.call('Bind_noparse_tag see item')
  end
  
  
--[ /miscdps --]
elseif command == 'miscdps' then
  if option == 'u' then
    echo('\aw/'..command..'')
  elseif option == 'h' then
    echo('Engine 2/3 settings only. see wiki')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maMiscDPS')
  elseif option == 't' then
    mq.cmd.call('Bind_noparse_tag see miscdps')
   end


--[ /song --]
elseif command == 'song' then
  if option == 'u' then
    echo('\aw/'..command..'')
  elseif option == 'h' then
    echo('no additional options')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maSong')
  elseif option == 't' then
    mq.cmd.call('Bind_noparse_tag see song')
  end
  
  
--[ /env --]
elseif command == 'env' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo('lots of options. see the wiki')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maEnv')
  elseif option == 't' then
    out('notag')
  end


--[ /beg --]
elseif command == 'beg' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w alias \aw]')
  elseif option == 'h' then
    echo('broadcasts a request to a general queue that any')
    echo('character you box can respond too')
    echo('e.g. \a-w/beg haste\ax would beg for haste')
    echo('the first class with haste set as an alias as a')
    echo('buff would answer')
  elseif option == 's' then
    out('nomap')
  elseif option == 't' then
    out('notag')
  end


--[ /on --]
elseif command == 'on' then
  if option == 'u' then
    echo('\aw/'..command..'')
  elseif option == 'h' then
    echo('turns all toons to \a-w/env auto on\ax except you')
  elseif option == 's' then
    out('nomap')
  elseif option == 't' then
    out('notag')
  end
  

--[ /off --]
elseif command == 'off' then
  if option == 'u' then
    echo('\aw/'..command..'')
  elseif option == 'h' then
    echo('turns all toons to \a-w/env auto off\ax except you')
  elseif option == 's' then
    out('nomap')
  elseif option == 't' then
    out('notag')
  end
  

--[ /navto --]
elseif command == 'navto' then
  if option == 'u'then
    echo('\aw/'..command..' [\a-w name|target|yxz loc|wp \aw] [\a-w nickname \aw]')
  elseif option == 'h' then
    echo('navs to something in that zone; a name, partial name, a set waypoint, a YXZ loc')
  elseif option == 's' then
    out('nomap')
  elseif option == 't' then
    out('notag')
  end


--[ /grab --]
elseif command == 'grab' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w name|partialname \aw]')
  elseif option == 'h' then
    echo('will pick up a ground spawn that is within 20 paces of you')
  elseif option == 's' then
    out('nomap')
  elseif option == 't' then
    out('notag')
  end


--[ /here --]
elseif command == 'here' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w broadcast|all|group|raid|zone \aw]')
  elseif option == 'h' then
    echo('oh shit command, sets all toons (except you) to:')
    echo('${dot} instant combat ready')
    echo('${dot} removes all invis')
    echo('${dot} delays all timers')
    echo('${dot} all toons to /env auto on')
  elseif option == 's' then
    out('nomap')
  elseif option == 't' then
    out('notag')
  end


--[ /enter --]
elseif command == 'enter' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w broadcast|all|group|raid|zone \aw]')
  elseif option == 'h' then
    echo('attemts to activate any lever or click to enter portal within 40 paces')
  elseif option == 's' then
    out('nomap')
  elseif option == 't' then
    out('notag')
  end


--[ /alto --]
elseif command == 'alto' then
  if option == 'u' then
    echo('\aw/'..command..'')
  elseif option == 'h' then
    echo('basically, a stop everything command. keep in mind, if you are in auto, you will go right')
    echo('back to whatever you were doing')
  elseif option == 's' then
    out('nomap')
  elseif option == 't' then
    out('notag')
  end


--[ /invis --]
elseif command == 'invis' then
  if option == 'u' then
    echo('\aw/'..command..'')
  elseif option == 'h' then
    echo('used for stacking invis')
    echo('WIZ,MAG,ENC,SHM,RNG,DRU${lsep}use \a-w/chr inv group\ax setting')
    echo('CLR,SHD,NEC,PAL${lsep}use \a-w/chr inv groupivu\ax setting')
  elseif option == 's' then
    out('nomap')
  elseif option == 't' then
    out('notag')
  end
  
  
--[ /repeat --]
elseif command == 'repeat' then
  if option == 'u' then
    echo('\aw/'..command..' "text to repeat"')
  elseif option == 'h' then
    echo('tells everyone within 60 eq feet of you to:')
    echo('delay 1-3 seconds')
    echo('target the intended mob')
    echo('repeat what you say')
  elseif option == 's' then
    out('nomap')
  elseif option == 't' then
    out('notag')
  end


--[ /grain --]
elseif command == 'grain' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo('all outputs in entropy are associated with a #')
    echo('you can turn off that output type by turning that # off')
    echo('some outputs are forced, deal with it')
    echo('\a-w/grain list\ax to see them all')
  elseif option == 's' then
    out('nomap')
  elseif option == 't' then
    out('notag')
  end


--[ /crew --]
elseif command == 'crew' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo('control group settings and builds')
    echo('see wiki for details')
  elseif option == 's' then
    out('nomap')
  elseif option == 't' then
    out('notag')
  end


--[ /sash --]
elseif command == 'sash' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo('active${lsep} use bandolier swapping')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maSash')
  elseif option == 't' then
    mq.cmd.call('Bind_noparse_tag see sash')
  end
    

--[ /burn --]
elseif command == 'burn' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo('los${lsep}line of sight requirements')
    echo('auto${lsep}automatic burn on named mobs')
    echo('count${lsep}number of mobs in your /env rad to start burns')
    echo('engage${lsep}% of mobs health to start burning')
    echo('force${lsep}force burning 100% of the time')
    echo('raid${lsep}burn on raids or not')
    echo('spinup${lsep}one shot burn startup. as much as possible w/o a target')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maBurn')
  elseif option == 't' then
    mq.cmd.call('Bind_noparse_tag see burn')
  end
  

--[ /override --]
elseif command == 'override' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo('los${lsep}line of sight requirements')
    echo('engage${lsep}mob HP engage pct nad auto engage at 100%')
    echo('loot${lsep}try to loot regardless of advanced loot settings')
    echo('safenames${lsep}safename requirements. accept anyone')
    echo('trainspell${lsep}mode trainspell skill limits and cast forever')
    echo('dscheck${lsep}damage shield chech for melee combat')
    echo('envauto${lsep}start the macro with the character in auto')
    echo('groupass${lsep}override yourself as groupassist and force manual assist')
    echo('splashhurt${lsep}allows clerics to ignore bypass hurtchecks for splashing to keep it on forced cooldown')
    echo('targetclear${lsep}removes target clearing requirement')
    echo('tmpengage${lsep}60 seconds of ignoring the engage %')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maOver')
  elseif option == 't' then
    out('notag')
  end


--[ /cursor --]
elseif command == 'cursor' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w option \aw] [ITEMONCURSOR]')
  elseif option == 'h' then
    echo('destroy${lsep}set item as destroy')
    echo('ignore${lsep}set item as ignore')
    echo('keep${lsep}set item as keep [system default]')
    echo('leave${lsep}set item as leave on corpse')
  elseif option == 's' then
    out('nomap')
  elseif option == 't' then
    out('notag')
  end


--[ /tc --]
elseif command == 'tc' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo('break${lsep}"attempt" to stop everything to start tie')
    echo('mode${lsep}method to use for tie [\a-w nav|stick \ax]')
    echo('toon${lsep}name of the character you want to tie too')
    echo('variance${lsep}min distance to try and keep to tie toon')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maTie')
  elseif option == 't' then
    out('notag')
  end
  

--[ /tie --]
elseif command == 'tie' then
  if option == 'u' then
    echo('\aw/'..command..'')
  elseif option == 'h' then
    echo('enables or disables tie')
    echo('see \a-w/tc\ax for control options ')
  elseif option == 's' then
    out('nomap')
  elseif option == 't' then
    out('notag')
  end

  
--[ /build --]
elseif command == 'build' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w save|load|set \ax] "\a-wnick name\ax" [\a-w all \ax]')
  elseif option == 'h' then
    echo('too much to explain here. see the wiki...')
  elseif option == 's' then
    out('nomap')
  elseif option == 't' then
    out('notag')
  end


--[ /mode --]
elseif command == 'mode' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w option \ax]')
  elseif option == 'h' then
    echo('drag${lsep}drags all the dead people')
    echo('farm${lsep}see wiki. used for repeating missions')
    echo('fish${lsep}golfing with beer')
    echo('forage${lsep}looks through the trash for used tp..')
    echo('harvest${lsep}picks things up off ground')
    echo('hunt${lsep}kills all the mobs')
    echo('lush${lsep}Moes Tavern!')
    echo('petfarm${lsep}stand there while your pet does all the work')
    echo('summon${lsep}summons your corpse in lobby')
    echo('temppet${lsep}[wix|cler] temp pet farming')
    echo('trainskill${lsep}trains skills. use on pet in a hall')
    echo('trainspell${lsep}casts spells.. a lot')
    echo('data${lsep}info needed for other modes such as harvest and hunt')
    echo('stop${lsep}stops all modes')
  elseif option == 's' then
    out('nomap')
  elseif option == 't' then
    out('notag')
  end


--[ /ttl --]
elseif command == 'ttl' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo('logic${lsep}use ttl logic for dps')
    echo('clear${lsep}resets current collected data')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maTTL')
  elseif option == 't' then
    out('notag')
  end


--[ /pred --]
elseif command == 'pred' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo('logic${lsep}enable or disable using preditcion logic')
    echo('prepburn${lsep}start burn routines')
    echo('preclass${lsep}start class specific prediction logic')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maPred')
  elseif option == 't' then
    out('notag')
  end


--[ /lesson --]
elseif command == 'lesson' then
  if option == 'u' then
    echo('\aw/'..command..'')
  elseif option == 'h' then
    echo('casts AA Lesson of the Devoted')
  elseif option == 's' then
    out('nomap')
  elseif option == 't' then
    out('notag')
  end


--[ /cls --]
elseif command == 'cls' then
  if option == 'u' then
    echo('\aw/'..command..'')
  elseif option == 'h' then
    echo('clears your screen of all corpses:')
    echo('sets /hidecorpse looted on')
    echo('sets /hidecorpse npc on')
  elseif option == 's' then
    out('nomap')
  elseif option == 't' then
    out('notag')
  end

  
--[ /see --]
elseif command == 'see' then
  if option == 'u' then
    echo('\aw/'..command..' mapname or listname')
  elseif option == 'h' then
    echo('outputs the content of a map or list')
  elseif option == 's' then
    out('nomap')
  elseif option == 't' then
    out('notag')
  end
  
  
--[ /coh --]
elseif command == 'coh' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w group \aw]')
  elseif option == 'h' then
    echo('/coh will request a call from a mage in your group')
    echo('/coh group will request a mage in your group to call the entire group')
  elseif option == 's' then
    out('nomap')
  elseif option == 't' then
    out('notag')
  end


--[ /buff --]
elseif command == 'buff' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo('master${lsep}the master switch')
    echo('ammo${lsep}things like arrows, axes, small rocks, whatever')
    echo('aura${lsep}feed the cat.. i dunno')
    echo('beg${lsep}answer beg buffing requests')
    echo('crew${lsep}buff your dannet people')
    echo('drop${lsep}auto drop buffs from lsDropBuffs')
    echo('group${lsep}buff mercs and people you dont box')
    echo('now${lsep}self 24/7 always up buffs')
    echo('raid${lsep}buff raid members')
    echo('self${lsep}buf yourself')
    echo('shrink${lsep}yes, you should shrink')
    echo('temp${lsep}temp buffs in vcombat for other players')
    echo('unity${lsep}your unity aa buff')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maBuff')
  elseif option == 't' then
    mq.cmd.call('Bind_noparse_tag see buff')
  end


--[ /chr --]
elseif command == 'chr' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo('/chr options will varry based off class')
    echo('shared commands are:')
    echo('ae${lsep}# of mobs in /env rad to start using AE stuffs')
    echo('fade${lsep}% of health to try and fade an attacker off')
    echo('intensity${lsep}settings for AA Intensity of the Resolute')
    echo('invis${lsep}settings for the types if invisibilities you want to use')
    echo('spire${lsep}how to use your class spire')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maChr')
  elseif option == 't' then
    out('notag')
  end


--[ /debuff --]
elseif command == 'debuff' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo('cripple${lsep}use defined cripples')
    echo('eradicate${lsep}use defined eradicate')
    echo('malo${lsep}use defined malos')
    echo('mez${lsep}use defined mezes')
    echo('slow${lsep}use defined slows')
    echo('snare${lsep}use defined snares')
    echo('tash${lsep}use defined snares')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see madeBuff')
  elseif option == 't' then
    mq.cmd.call('Bind_noparse_tag see debuff')
  end
  
  
--[ /gtfo --]
elseif command == 'gtfo' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo('active${lsep}enable get the fuck out logic')
    echo('dead${lsep}# in your group dead to bail')
    echo('force${lsep}manual force run and hide')
    echo('spawns${lsep}# of mobs in your /env rad to RUNNOFT')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maGTFO')
  elseif option == 't' then
    out('notag')
  end
  

--[ /melee --]
elseif command == 'melee' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo('/melee has no options. just set the skills you want to use')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maMelee')
  elseif option == 't' then
    mq.cmd.call('Bind_noparse_tag see melee')
  end
  

--[ /nomore --]
elseif command == 'nomore' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w option \aw]')
  elseif option == 'h' then
    echo('invis${lsep}remove all invis buffs')
    echo('levi${lsep}remove all levi buffs')
    echo('removeall${lsep}remove all buffs')
    echo('da${lsep}remove divine aura buffs')
    echo('spa##${lsep}remove al buffs with spa NAME or ##')
  elseif option == 's' then
    out('nomap')
  elseif option == 't' then
    out('notag')
  end 


--[ /edit --]
elseif command == 'edit' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w many options... \aw]')
  elseif option == 'h' then
    echo('allows for raw editing of any variable, list or map')
    echo('see wiki for full details')
    echo('really go read the wiki..')
  elseif option == 's' then
    out('nomap')
  elseif option == 't' then
    out('notag')
  end



--[ /entropy --]
elseif command == 'entropy' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w many options... \aw]')
  elseif option == 'h' then
    echo('autolist${lsep}')
    echo('buffcooldownwait${lsep}delays buffing till the gem/cast has completed its colldown')
    echo('buttons${lsep}mq2buttons control')
    echo('castloops${lsep}hold a cast loop until every spell in the loop cast, then release')
    echo('concolor${lsep}shows targets con color in echos')
    echo('dannet${lsep}dannet settings')
    echo('engine${lsep}engine 1|2|3')
    echo('events${lsep}enable using custome events of events.inc file')
    echo('fizzles${lsep}# of retrys on a fizle')
    echo('hook${lsep}enable hooks into entropy routines')
    echo('hud${lsep}hud controls')
    echo('lopdelay${lsep}time to delay nuke and dot loops every cast')
    echo('maintenance${lsep}places entropy into a maintenance mode')
    echo('manautoinv${lsep}autoinventory items according to loot file while in manual mode')
    echo('relaytell${lsep}relays all tells you recieve to tie toon')
    echo('rememberset${lsep}')
    echo('tlp${lsep}alternate mechanics specific for TLP slummers')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maEntropy')
  elseif option == 't' then
    out('notag')
  end



--[ /rest --]
elseif command == 'rest' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w options \aw]')
  elseif option == 'h' then
    echo('buff${lsep}break rest to use buff routines')
    echo('combat${lsep}attempt to rest through combat to full')
    echo('full${lsep}rest to full mana/stamina if able')
    echo('modrod${lsep}pester mages for modrods while resting')
    echo('pct${lsep}min % you want ro rest too')
    echo('rad${lsep}no mob area around to you allow resting')
  elseif option == 's' then
    mq.cmd.call('Bind_command_see maRest')
  elseif option == 't' then
    out('notag')
  end


--[ /coh --]
elseif command == 'coh' then
  if option == 'u' then
    echo('\aw/'..command..' [\a-w group... \aw]')
  elseif option == 'h' then
    echo('w/o option requests a single target coh for you')
    echo('group option requests a full group coh')
  elseif option == 's' then
    out('nomap')
  elseif option == 't' then
    out('notag')
  end




end

