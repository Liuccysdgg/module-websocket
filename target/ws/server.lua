local M = {}
M.__index = M

function M.new()
    local instance = setmetatable({}, M)
    instance.module = fw_ws_server.new()
    return instance
end
--[[
FUNCTION： 启动 websocket server
PARAM
    param    :   启动参数
    {
        port = 8888,
        accept = function(connid)
            print("accept: "..connid)
        end,
        close = function(connid)
            print("close: "..connid)
        end,
        msg = function (connid,data)
            print("msg: "..msg)
            server:send(connid,data)
        end
    }
--]]
function M:start(param)
    return self.module:start(param)
end

--[[
FUNCTION： 关闭
--]]
function M:stop()
    return self.module:stop()
end

--[[
FUNCTION： 取最后错误描述
--]]
function M:last_error()
    self.module:last_error()
end

--[[
FUNCTION： 事件循环
DESC：需要关闭时返回后调用stop或任当前虚拟机返回即可
    timeout_msec   :   超时时间(毫秒)
--]]
function M:exec(msec)
    self.module:exec(msec)
end

--[[
FUNCTION： 发送
PARAM
    value    ：  数据
--]]
function M:send(value)
    return self.module:send(value)
end

return M