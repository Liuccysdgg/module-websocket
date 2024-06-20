#include "ws.h"
#include "dll_interface.h"
#include "util/system.h"
extern "C" {
#ifdef _WIN32
    DLL_EXPORT
#endif
        int fastweb_module_regist(void* sol2, void* lua)
    {
        sol::state* state = static_cast<sol::state*>(sol2);
        module::ws_server::regist(state);
        return 0;
    }
}

module::ws_server::ws_server()
{
}

module::ws_server::~ws_server()
{
}

bool module::ws_server::start(sol::table table)
{
    if (table["port"].get_type() != sol::type::number)
    {
        m_lastErrorDesc = "`port` must be number";
        return false;
    }
    if (table["accept"].get_type() == sol::type::function)
        m_callback_accept = table["accept"];
    if (table["close"].get_type() == sol::type::function)
        m_callback_close = table["close"];
    if (table["msg"].get_type() == sol::type::function)
        m_callback_recv = table["msg"];


    this->on_accept([&](int64 connid)->bool {
        queue_info info;
        info.type = 0;
        info.connid = connid;
        m_queue.push(info);
        return true;
    });
    this->on_close([&](int64 connid) {
        queue_info info;
        info.type = 2;
        info.connid = connid;
        m_queue.push(info);
    });
    this->on_recv([&](int64 connid,const char* data,int len)->bool {
        queue_info info;
        info.type = 1;
        info.connid = connid;
        info.data.append(data, len);
        m_queue.push(info);
        return true;
    });
    return ::ylib::network::http::wsserver::start(table["port"]);
}

void module::ws_server::stop()
{
    ::ylib::network::http::wsserver::stop();
}

void module::ws_server::exec(int timeout)
{
    for (size_t i = 0; i < timeout; i++)
    {
        queue_info info;
        while (m_queue.pop(info))
        {
            if (info.type == 0)
            {
                if (m_callback_accept.valid())
                    m_callback_accept(info.connid);
            }
            else if (info.type == 1)
            {
                if (m_callback_recv.valid())
                    m_callback_recv(info.connid,std::string_view(info.data.data(),info.data.length()));
            }
            else if (info.type == 2)
            {
                if (m_callback_close.valid())
                    m_callback_close(info.connid);
            }
        }
        system::sleep_msec(1);
    }
}

std::string module::ws_server::last_error()
{
    return m_lastErrorDesc;
}

void module::ws_server::disconn(int64 conn)
{
    ::network::http::wsserver::disconn(conn);
}

bool module::ws_server::send(int64 conn, std::string_view value)
{
    return ::network::http::wsserver::send(conn, value);
}


void module::ws_server::regist(sol::state* lua)
{
    lua->new_usertype<module::ws_server>("fw_ws_server",
        "new", sol::constructors<module::ws_server()>(),
        "stop", &module::ws_server::stop,
        "start", &module::ws_server::start,
        "last_error", &module::ws_server::last_error,
        "self", &module::ws_server::self,
        "exec", &module::ws_server::exec,
        "send", &module::ws_server::send,
        "disconn", &module::ws_server::disconn
    );
}

void module::ws_server::regist_global(const char* name, sol::state* lua)
{
    lua->registry()[name] = this;
    (*lua)[name] = this;
}
