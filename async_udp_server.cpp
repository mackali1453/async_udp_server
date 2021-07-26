#include <iostream>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <thread>
#include <unistd.h>

using boost::asio::ip::tcp;
//#pragma pack(push,r1,1)
////size of char pointer is 8 bytes
struct PlatformData {
	unsigned char securityId = 0;
	unsigned char position = 0;
	unsigned char velocity = 0;
	unsigned char acceleration = 0;
//	float temperature = 5;
//	float current = 6;
//	float torque = 7;
//	char driver = 8;
//	float voltage = 9;
//	float encoder = 10;
//	char limitSensor1 = 11;
//	char limitSensor2 = 12;
};
//
//#pragma pack(pop,r1)

class tcp_connection
  : public std::enable_shared_from_this<tcp_connection>
{
public:
  typedef boost::shared_ptr<tcp_connection> pointer;

  static pointer create(boost::asio::io_context& io_context)
    {
      return pointer(new tcp_connection(io_context));
    }

  tcp_connection(boost::asio::io_context& io_context)
      : socket_(io_context)
    {
    }
  ~tcp_connection() {
            std::cout << "Destroyed tcp_connection\n";
        }


  void start()
      {
	  boost::system::error_code err;
  	  handle_write(err);
        }
  tcp::socket& socket()
      {
          return socket_;
      }
  void run(const boost::system::error_code &err)
  {
	  platformData.velocity = 20;
	  if(!err)
	  {


	  socket_.async_send( boost::asio::buffer(&platformData,sizeof(platformData)),
	            boost::bind(&tcp_connection::handle_write, this,boost::asio::placeholders::error()));
	  	  std::cout<<"run"<<std::endl;
	  }
	  else
	  {
		  std::cout<<"run delete"<<std::endl;

		  delete this;
	  }
  }
  void handle_write(const boost::system::error_code &err)
    {
	  if(!err)
	  {
	  socket_.async_receive( boost::asio::buffer(&platformData,sizeof(platformData)),
      boost::bind(&tcp_connection::run, this,boost::asio::placeholders::error()));
	  std::cout<<"handle_write:"<<(int)platformData.position<<std::endl;

	  }
	  else
	  {
		  std::cout<<"handle_write delete"<<std::endl;
		  delete this;
	  }
    }
private:
  tcp::socket socket_;
  PlatformData platformData;


};

class tcp_server
{
public:

  tcp_server(boost::asio::io_context& io_context_)
    :
    	io_context(io_context_),
      tcpAcceptor(io_context_, tcp::endpoint(tcp::v4(), 8080))
  {

	  start_accept();
  }

  void start_accept()
  {
	tcp_connection::pointer new_connection =
	        tcp_connection::create(io_context);
    tcpAcceptor.async_accept(new_connection->socket(),
        boost::bind(&tcp_server::handle_accept, this, new_connection,
        		boost::asio::placeholders::error()));
  }

  void handle_accept(tcp_connection::pointer new_connection,
      const boost::system::error_code& error)
  {
    if (!error)
    {
      new_connection->start();
      start_accept();
    }


  }
private:
  tcp::acceptor tcpAcceptor ;
  boost::asio::io_context& io_context;
};

int main()
{
    try
    {

        boost::asio::io_context io_context;

//        boost::asio::io_context::work idleWork(io_context);


        tcp_server server(io_context);
        io_context.run();
//        std::thread thread1 = std::thread([&](){io_context.run();});

//        server.close();
//        usleep(15000000);
//        thread1.join();



//        std::this_thread::sleep_for(boost::asio::chrono::seconds(5));

    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}
