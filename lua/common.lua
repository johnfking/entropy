--
-- entropoy.mac
-- common.lua
-- 
-- lua shared stuffs
--

mq = require('mq')
echo = mq.cmd.echo
-- DisplayName = mq.TLO.Me.DisplayName()
-- ShortName = mq.TLO.Me.Class.ShortName()
discord = '${If[${Bool[${Plugin[MQ2Discord]}]},\at-\ax,]}'
ent = {}
target = {}


rez = {}
rez['radius'] = tonumber(mq.TLO.Macro.Variable('maRez').Find('stMaxRezRange').Value())




out = function (option, verbage)
  -- help response for no tags for the control
  if option == 'notag' then
    echo('${lsep}no tags for this control.')

  -- help response for no map for this control
  elseif option == 'nomap' then
    echo('${lsep}no map for this control.')
  end
end




hudInfo = function ()
  
  -- base information
  ent['build'] = mq.TLO.Macro.Variable('maEnv').Find('build').Value() or ''
  ent['mode'] = mq.TLO.Macro.Variable('maData').Find('mode').Value() or ''
  ent['set'] = mq.TLO.Macro.Variable('maEnv').Find('stBuildSetCurrent').Value() or '--'
  ent['body'] = mq.TLO.Target.Body.Name() or ''
  ent['auto'] = mq.TLO.Macro.Variable('maEnv').Find('swAuto').Value() or '0'

  -- engages
  ent['engage'] = mq.TLO.Macro.Variable('maCC').Find('stPctEngage').Value() or ''
  ent['petengage'] = mq.TLO.Macro.Variable('maMinion').Find('stPctPetEngage').Value() or ''
  ent['swarmengage'] = mq.TLO.Macro.Variable('maMinion').Find('stPctSwarmEngage').Value() or ''

  -- target
  target['shortname'] = mq.TLO.Target.Class.ShortName() or '--'
  target['level'] = mq.TLO.Target.Level() or '--'
  target['displayname'] = mq.TLO.Target.DisplayName() or '--'
  target['guild'] = mq.TLO.Target.Guild() or '--'
  target['distance'] = mq.TLO.Target.Distance() or 0
  target['maxrangeto'] = mq.TLO.Target.MaxRangeTo() or 0 

  -- tie
  ent['tiemode'] = mq.TLO.Macro.Variable('maTie').Find('stTieMode').Value() or '--'
  ent['tietoon'] = mq.TLO.Macro.Variable('maTie').Find('stTieToon').Value() or '--'

  -- env
  ent['envrad'] = mq.TLO.Macro.Variable('maEnv').Find('stEnvRadius').Value() or '--'

  -- pull
  ent['pullrad'] = mq.TLO.Macro.Variable('maPull').Find('stPullRadius').Value() or '--'
  ent['pullmode'] = mq.TLO.Macro.Variable('maPull').Find('stPullMode').Value() or '--'
  ent['pullactive'] = mq.TLO.Macro.Variable('maPull').Find('swPull').Value() or false


 
end




 
  
  


