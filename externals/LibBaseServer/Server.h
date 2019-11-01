#pragma once

#include <vector>
#include <deque>
#include <algorithm>
#include <string>
#include <vector>
#include "Session.h"


class Server
{
public:
	Server (int threadPoolSize, int port);
	virtual ~Server ();

	virtual void Init (int nMaxSessionCount)= 0;
	void Start ();
	void MoveSessionToQueue (int sessionID);
	boost::shared_ptr<Session> GetSession (int sessionId) { return m_SessionList[sessionId]; }

private:
	bool Accept ();
	void OnAccept (int sessionId, const boost::system::error_code& error);

protected:
	virtual void OnAccept (int sessionId) = 0;

	boost::asio::io_service io_service;
	boost::asio::ip::tcp::acceptor m_Acceptor;
	std::vector<boost::shared_ptr<boost::thread>> m_ThreadPool;
	int m_ThreadPoolSize;
	bool m_IsAccepting;
	std::vector<boost::shared_ptr<Session>> m_SessionList;
	std::deque<int> m_SessionQueue;
};

