#include "State.h"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/asio.hpp>
#include <boost/serialization/string.hpp>
#include <iostream>
#include <thread>
#include <vector>

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session>
{
public:
	Session(tcp::socket socket) : _socket(std::move(socket))
	{
		_data.resize(1024);
	}
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
									{
										std::cerr << "Error occurred: " << ec.message() << " (code: " << ec.value() << ")" << std::endl;
										return;
									}
									try
									{
										std::string rawData(_data.data(), length);
										std::cout << "Raw data: " << rawData << std::endl;
										std::istringstream archiveStream(rawData);
										if (!archiveStream)
										{
											std::cerr << "Failed to initialize input stream." << std::endl;
											return;
										}
										boost::archive::text_iarchive archive(archiveStream);
										archive >> _state;
										std::cout << "Received data: " << "text=" << _state.text << ", number=" << _state.number << ", flag=" << _state.flag << std::endl;
									}
									catch (const boost::archive::archive_exception &ex)
									{
										std::cerr << "Archive exception: " << ex.what() << std::endl;
										return;
									}
									catch (const std::exception &e)
									{
										std::cerr << "Deserialization failed: " << e.what() << std::endl;
										return;
									}
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
	Server(boost::asio::io_context &io_context, short port) : _acceptor(io_context, tcp::endpoint(tcp::v4(), port))
	{
		std::cout << "Server start\n";
		doAccept();
	}

private:
	void doAccept()
	{
		_acceptor.async_accept([this](boost::system::error_code ec, tcp::socket socket)
							   {
				if (!ec)
				{
					std::make_shared<Session>(std::move(socket))->start();
					std::cout<<"New session\n";
				}
				doAccept(); });
	}
	tcp::acceptor _acceptor;
};

int main(int argc, char *argv[])
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
	catch (const std::exception &e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}
	return 0;
}