#include "State.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/asio.hpp>
#include <boost/serialization/string.hpp>
#include <chrono>
#include <iostream>
#include <random>
#include <thread>
#include <windows.h>

using boost::asio::ip::tcp;

class Client
{
public:
	Client(boost::asio::io_context& ioContext, const std::string& host, const std::string& port) : _socket(ioContext)
	{
		tcp::resolver resolver(ioContext);
		auto endpoints = resolver.resolve(host, port);
		doConnect(endpoints);
	}

	void sendData()
	{
		while (_run)
		{
			State state = generateRandomState();
			std::ostringstream archiveStream;
			boost::archive::text_oarchive archive(archiveStream);
			archive << state;
			std::string outbound_data = archiveStream.str();
			boost::asio::write(_socket, boost::asio::buffer(outbound_data));
			std::this_thread::sleep_for(std::chrono::seconds(2));
		}
	}

	void close()
	{
		_run = false;
	}

private:
	void doConnect(const tcp::resolver::results_type& endpoints)
	{
		boost::asio::async_connect(_socket, endpoints, [this](boost::system::error_code ec, tcp::endpoint)
			{
				if (ec)
					return;
				sendData();
			});
	}

	State generateRandomState()
	{
		static std::default_random_engine engine(std::random_device{}());
		static std::uniform_int_distribution<int> distText(1, 10);
		static std::uniform_int_distribution<long long> distNumber(1, 1000000);
		static std::bernoulli_distribution distFlag(0.5);
		State data;
		data.text = std::string(distText(engine), 'A');
		data.number = distNumber(engine);		data.flag = distFlag(engine);

		return data;
	}

	tcp::socket _socket;
	bool _run{ true };
};

int main(int argc, char* argv[])
{
	try
	{
		if (argc != 3)
		{
			std::cerr << "Usage: client <host> <port>\n";
			return 1;
		}
		boost::asio::io_context ioContext;
		Client c(ioContext, argv[1], argv[2]);
		ioContext.run();
		while (true) 
		{
			if (GetAsyncKeyState('X') & 0x8000) 
			{
				c.close();
				break;
			}
			Sleep(100);
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}