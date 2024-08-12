#include "State.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/asio.hpp>
#include <boost/serialization/string.hpp>
#include <chrono>
#include <iostream>
#include <random>
#include <thread>

using boost::asio::ip::tcp;

class Client
{
public:
	Client(boost::asio::io_context &ioContext, const std::string &host, const std::string &port) : _socket(ioContext)
	{
		tcp::resolver resolver(ioContext);
		auto endpoints = resolver.resolve(host, port);
		doConnect(endpoints);
	}

	void sendData()
	{
		while (_run)
		{
			try
			{
				State state = generateRandomState();
				std::ostringstream archiveStream;
				boost::archive::text_oarchive archive(archiveStream);
				archive << state;
				std::string outbound_data = archiveStream.str();

				// if (!_socket.is_open())
				// {
				// 	std::cerr << "Socket is closed, cannot send data." << std::endl;
				// 	return;
				// }

				auto size = boost::asio::write(_socket, boost::asio::buffer(outbound_data));
				std::cout << "Transferred: " << size << " bytes" << std::endl;
			}
			catch (const boost::archive::archive_exception &ex)
			{
				std::cerr << "Archive exception: " << ex.what() << std::endl;
				return;
			}
			catch (const boost::system::system_error &e)
			{
				std::cerr << "System error: " << e.what() << std::endl;
				return;
			}
			catch (const std::exception &e)
			{
				std::cerr << "Exception: " << e.what() << std::endl;
				return;
			}

			std::this_thread::sleep_for(std::chrono::seconds(2));
		}
	}

	void close()
	{
		_run = false;
	}

private:
	void doConnect(const tcp::resolver::results_type &endpoints)
	{
		boost::asio::async_connect(_socket, endpoints, [this](boost::system::error_code ec, tcp::endpoint)
								   {
							if (ec)
							{
								std::cerr << "Error occurred: " << ec.message() << " (code: " << ec.value() << ")" << std::endl;
								return;
							}							
				sendData(); });
	}

	State generateRandomState()
	{
		static std::default_random_engine engine(std::random_device{}());
		static std::uniform_int_distribution<int> distText(1, 10);
		static std::uniform_int_distribution<long long> distNumber(1, 1000000);
		static std::bernoulli_distribution distFlag(0.5);
		State data;
		data.text = std::string(distText(engine), 'A');
		data.number = distNumber(engine);
		data.flag = distFlag(engine);

		return data;
	}

	tcp::socket _socket;
	bool _run{true};
};

int main(int argc, char *argv[])
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
		// 		std::thread inputThread([&c]()
		// 								{
		//             std::cout << "Press any key to stop the client..." << std::endl;

		// #ifdef _WIN32
		//             _getch();
		// #else
		//             system("stty raw -echo");
		//             getchar();
		//             system("stty cooked echo");
		// #endif
		//             c.close(); });

		ioContext.run();
		// inputThread.join();
	}
	catch (std::exception &e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}