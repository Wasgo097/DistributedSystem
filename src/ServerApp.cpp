#include "State.h"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/asio.hpp>
#include <boost/serialization/string.hpp>
#include <iostream>
#include <thread>

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session>
{
public:
	Session(tcp::socket socket) : _socket(std::move(socket)) {}
	void start()
	{
		doRead();
	}
private:
	void doRead()
	{
		auto self(shared_from_this());
		boost::asio::async_read(_socket, boost::asio::buffer(_data.data(), _data.size()),
			[this, self](boost::system::error_code ec, std::size_t length)
			{
				if (ec)
					return;
				std::istringstream archiveStream(std::string(_data.data(), length));
				boost::archive::text_iarchive archive(archiveStream);
				archive >> _state;
				std::cout << "Received data: " << "text=" << _state.text << ", number=" << _state.number << ", flag=" << _state.flag << std::endl;
				doRead();
			});
	}
	tcp::socket _socket;
	std::vector<char> _data;
	State _state;
};

class Server
{
public:
	Server(boost::asio::io_context& io_context, short port) : _acceptor(io_context, tcp::endpoint(tcp::v4(), port))
	{
		doAccept();
	}
private:
	void doAccept()
	{
		_acceptor.async_accept([this](boost::system::error_code ec, tcp::socket socket)
			{
				if (!ec)
					std::make_shared<Session>(std::move(socket))->start();
				doAccept();
			});
	}
	tcp::acceptor _acceptor;
};

int main(int argc, char* argv[])
{
	try
	{
		if (argc != 2)
		{
			std::cerr << "Usage: server <port>\n";
			return 1;
		}
		boost::asio::io_context ioContext;
		Server s(ioContext, std::atoi(argv[1]));
		ioContext.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}
	return 0;
}