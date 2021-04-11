--[[ 

testing group health return
  
--]]




require "common"
--sql = require('lsqlite3complete')



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
hurt['pct'] = mq.TLO.Me.PctHPs()


for i = 0, mq.TLO.Group.Members()
do
  local group_member = mq.TLO.Group.Member(i) 
  if group_member.Dead() ~= 1 and group_member.Present() ~= 0 and rangeCheck(group_member.Distance(), range) and healthCheck(group_member.PctHPs(), hurt['pct']) then
    mq.cmd.vardata('hurtData', group_member.ID())
    hurt['pct'] = group_member.PctHPs()
    mq.cmd.varcalc('hurtCount ${hurtCount} + 1')
  end

end












