#pragma once
#include "basemodule.h"
#include "net/http_wsserver.h"
#include "util/queue.hpp"
namespace module
{
	struct queue_info {
		// 0=连接 1=接收 2=断开
		int type = 0;
		int64 connid = 0;
		ylib::buffer data;
	};
	/// <summary>
	/// websocket server
	/// </summary>
	class ws_server : private ylib::network::http::wsserver,public module::base {
	public:
		ws_server();
		~ws_server() override;

	
		bool start(sol::table table);
		void stop();
		void exec(int timeout);
		std::string last_error();
		bool send(int conn,std::string_view value);

		static void regist(sol::state* lua);
	private:
		sol::function m_callback_accept;
		sol::function m_callback_recv;
		sol::function m_callback_close;

		ylib::queue<queue_info> m_queue;
	private:
		// 通过 imodule 继承
		virtual void regist_global(const char* name, sol::state* lua);
		virtual void delete_global() { delete this; }
	};
}

