--[[ 

testing file for learning Lua.. noting to see here but me breaking things. 
  
--]]






require "short"
require "common"
out('\at...\ax \a-wtest.lua\ax')

local arg = {...}


-- test passed variables
testsub(arg)
return arg[2]







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



-- mq.cmd.echo(mq.TLO.Me.DisplayName())
-- mq.cmd.echo('Number of args: '..#arg)  
  
  


local debug_u = function () print('1') end
local debug_h = function () print('2') end

help = {
  ['take'] = {
    ['u'] = '\aw/take [\a-w broadcast|all|group|raid|zone \aw]'), 
    ['h'] = '',
    ['s'] = mq.cmd.call('Bind_command_see maDebug')
  
  }
}

help['debug']['u']()




--[ /take --]
elseif command == 'take' then
  if option == 'u' then
    echo(discord..'\aw/'..command..' [\a-w broadcast|all|group|raid|zone \aw]')
  elseif option == 'h' then
    echo(discord..'Accepts, according to broadcast${sep}')
    echo(discord..'${dot}translocate, guild hall port, adventures, challenges, expeditions')
    echo(discord..'${dot}resurrections, highlighted task, trade window, shared task')
  end