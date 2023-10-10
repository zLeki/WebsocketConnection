#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#include <thread>
#include <chrono>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace url {
    const char* url = "0.0.0.0";
    const char* port = "8080";
}

websocket::stream<asio::ip::tcp::socket> connect() {
    asio::io_context io_context;
    asio::ip::tcp::resolver resolver(io_context);
    auto const results = resolver.resolve(url::url, url::port);
    websocket::stream<asio::ip::tcp::socket> ws(io_context);
    asio::connect(ws.next_layer(), results.begin(), results.end());
    return ws;
}
int main() {
    try {
        auto ws = connect();
        ws.handshake("0.0.0.0:8080", "/ws");
        ws.write(asio::buffer("crashSubscribe"));
        std::cout << "Sent " << std::endl;
        beast::flat_buffer buffer;
        ws.read(buffer);
        while (true) {
            beast::flat_buffer buffer;
            ws.read(buffer);
            std::string msg = beast::buffers_to_string(buffer.data());
            std::cout << "Received: " << msg << std::endl;
        }
//        ws.close(websocket::close_code::normal);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
