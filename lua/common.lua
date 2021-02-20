--[[
- entropoy.mac
- common.lua
- 
- lua shared _stuffs_
-
--]]

mq = require('mq')
echo = mq.cmd.echo
DisplayName = mq.TLO.Me.DisplayName()
ShortName = mq.TLO.Me.Class.ShortName()
discord = '${If[${Bool[${Plugin[MQ2Discord]}]},\at-\ax,]}'




--[[

for i = 0, mq.TLO.Group.Members()
do
  local group_member = mq.TLO.Group.Member(i) 
  if group_member.Dead() ~= 1 and group_member.Present() ~= 0 and rangeCheck(group_member.Distance(), range) and healthCheck(group_member.PctHPs(), hurt['pct']) then
    hurt['id'] = group_member.ID()
    hurt['pct'] = group_member.PctHPs()
    hurt['cls'] = group_member.Class.ShortName()
    hurt['count'] = hurt['count'] +1
  end
end

--]]