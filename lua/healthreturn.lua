--[[ 

testing group health return
  
--]]


require "short"
require "common"

sql = require('lsqlite3complete')
--echo(sqlite3.version())






local healthCheck = function (memberPct, PctCheck)
  return math.min(memberPct, PctCheck) 
end

local rangeCheck = function (memberRange, range)
  return memberRange < math.min(unpack(range))
end






local range = {}
range[1] = 200
range[2] = mq.TLO.Me.Spell('Sincere Remedy Rk. III').MyRange()  

local hurt = {}
hurt['id'] = mq.TLO.Me.ID()
hurt['pct'] = mq.TLO.Me.PctHPs()
hurt['cls'] = mq.TLO.Me.Class.ShortName()
hurt['count'] = 0

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
echo(hurt['cls']..'\a-r::\ax\a-w'..hurt['pct']..'\ax')






