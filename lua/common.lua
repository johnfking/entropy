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
  ent['inv'] = ''
  ent['ivu'] = ''

  -- invis 
  if mq.TLO.Me.SPA(12)() ~= 0 or mq.TLO.Me.SPA(314)() ~= 0 then
    ent['inv'] = 'IV'
  end
  
  -- ivu
  if mq.TLO.Me.SPA(28)() ~= 0 or mq.TLO.Me.SPA(315)() ~= 0 then
    ent['ivu'] = 'IVU'
  end
  
  -- base information
  ent['build'] = mq.TLO.Macro.Variable('maEnv').Find('build').Value() or '--'
  ent['mode'] = mq.TLO.Macro.Variable('maData').Find('mode').Value() or ''
  ent['set'] = mq.TLO.Macro.Variable('maEnv').Find('stBuildSetCurrent').Value() or '--'
  ent['body'] = mq.TLO.Target.Body.Name() or ''
  ent['auto'] = mq.TLO.Macro.Variable('maEnv').Find('swAuto').Value() or '0'
  ent['hudoutput'] = mq.TLO.Macro.Variable('HUDOutput')() or '--'

 
  -- target
  target['shortname'] = mq.TLO.Target.Class.ShortName() or '--'
  target['level'] = mq.TLO.Target.Level() or '--'
  target['displayname'] = mq.TLO.Target.DisplayName() or '--'
  target['guild'] = mq.TLO.Target.Guild() or '--'
  target['distance'] = mq.TLO.Target.Distance() or 0
  target['maxrangeto'] = mq.TLO.Target.MaxRangeTo() or 0 


end




 
  
  


