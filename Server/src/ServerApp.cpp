//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2024 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <ctime>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <chrono>
#include <thread>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

std::string make_daytime_string()
{
    using namespace std; // For time_t, time and ctime;
    time_t now = time(0);
    return ctime(&now);
}

class tcp_connection
    : public std::enable_shared_from_this<tcp_connection>
{
public:
    typedef std::shared_ptr<tcp_connection> pointer;

    static pointer create(boost::asio::io_context &io_context)
    {
        return pointer(new tcp_connection(io_context));
    }

    tcp::socket &socket()
    {
        return socket_;
    }

    void start()
    {
        message_ = make_daytime_string();
        std::cout << "Message to send: " << message_ << std::endl;

        boost::asio::async_write(socket_, boost::asio::buffer(message_),
                                 [this, self = shared_from_this()](const boost::system::error_code &error, std::size_t bytes_transferred)
                                 {
                                     if (!error)
                                     {
                                         std::cout << "Wysłano: " << bytes_transferred << " bajtów." << std::endl;
                                         this->wait_to_write();
                                         std::this_thread::sleep_for(std::chrono::seconds(1));
                                     }
                                     else
                                     {
                                         std::cerr << "Błąd w czasie zapisu: " << error.message() << std::endl;
                                     }
                                 });
    }

private:
    tcp_connection(boost::asio::io_context &io_context)
        : socket_(io_context)
    {
    }

    void handle_write(const boost::system::error_code & /*error*/,
                      size_t bytes_transferred)
    {
        std::cout << "Send " << bytes_transferred << "bytes\n";
    }

    void wait_to_write()
    {
        socket_.async_wait(boost::asio::ip::tcp::socket::wait_write,
                           [this, self = shared_from_this()](const boost::system::error_code &error)
                           {
                               if (!error)
                               {
                                   this->start();
                               }
                               else
                               {
                                   std::cerr << "Błąd w czasie oczekiwania na zapis: " << error.message() << std::endl;
                               }
                           });
    }

    tcp::socket socket_;
    std::string message_;
};

class tcp_server
{
public:
    tcp_server(boost::asio::io_context &io_context)
        : io_context_(io_context),
          acceptor_(io_context, tcp::endpoint(tcp::v4(), 13))
    {
        start_accept();
    }

private:
    void start_accept()
    {
        tcp_connection::pointer new_connection =
            tcp_connection::create(io_context_);

        acceptor_.async_accept(new_connection->socket(),
                               [this, new_connection](const boost::system::error_code &error)
                               {
                                   std::cout << "New connection!\n";
                                   this->handle_accept(new_connection, error);
                               });
    }

    void handle_accept(tcp_connection::pointer new_connection,
                       const boost::system::error_code &error)
    {
        if (!error)
        {
            new_connection->start();
        }

        start_accept();
    }

    boost::asio::io_context &io_context_;
    tcp::acceptor acceptor_;
};

int main()
{
    try
    {
        boost::asio::io_context io_context;
        tcp_server server(io_context);
        io_context.run();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}