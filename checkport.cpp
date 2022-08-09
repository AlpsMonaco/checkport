#ifndef CHECK_PORT_HEADER_ONLY
#include "checkport.h"
#else
#define NAMESPACE_CHECKPORT_START \
    namespace checkport           \
    {

#define NAMESPACE_CHECKPORT_END }
#endif

NAMESPACE_CHECKPORT_START

CheckPort::CheckPort(const std::string& ip, unsigned short begin_port, unsigned short end_port,
                     std::uint8_t concurrency_num,
                     size_t timeout_sec)
    : ios_(),
      ip_(ip),
      begin_port_(begin_port),
      end_port_(end_port),
      current_port_(begin_port),
      concurrency_num_(concurrency_num),
      timeout_sec_(timeout_sec),
      target_port_(0)
{
}

CheckPort::~CheckPort()
{
}

void CheckPort::SetConcurrency(std::uint8_t num)
{
    concurrency_num_ = num;
}

void CheckPort::SetConnectTimeout(size_t timeout_sec)
{
    timeout_sec_ = timeout_sec;
}

void CheckPort::Assign(const std::string& ip, unsigned short begin_port, unsigned short end_port)
{
    ip_ = ip;
    begin_port_ = begin_port;
    end_port_ = end_port;
    current_port_ = begin_port;
    target_port_ = 0;
}

void CheckPort::CreateGetOneTask()
{
    std::uint16_t port = current_port_++;
    if (port > end_port_)
        return;
    std::shared_ptr<asio::ip::tcp::socket> socket_ptr = std::make_shared<asio::ip::tcp::socket>(ios_);
    std::shared_ptr<asio::steady_timer> timer_ptr = std::make_shared<asio::steady_timer>(ios_, std::chrono::seconds(timeout_sec_));
    timer_ptr->async_wait([socket_ptr, timer_ptr, this](const asio::error_code& ec) -> void
                          {
                              if (!ec) socket_ptr->cancel();
                          });
    socket_ptr->async_connect(asio::ip::tcp::endpoint(
                                  asio::ip::address::from_string(ip_), port),
                              [socket_ptr, timer_ptr, this](const asio::error_code& ec) -> void
                              {
                                  if (ec)
                                  {
                                      if (target_port_ > 0)
                                          return;
                                      timer_ptr->cancel();
                                      CreateGetOneTask();
                                  }
                                  else
                                  {
                                      timer_ptr->cancel();
                                      if (target_port_ > 0)
                                          return;
                                      target_port_ = socket_ptr->remote_endpoint().port();
                                  }
                              });
}

void CheckPort::CreateGetAllTask(std::vector<std::uint16_t>& port_list)
{
    std::uint16_t port = current_port_++;
    if (port > end_port_)
        return;
    std::shared_ptr<asio::ip::tcp::socket> socket_ptr = std::make_shared<asio::ip::tcp::socket>(ios_);
    std::shared_ptr<asio::steady_timer> timer_ptr = std::make_shared<asio::steady_timer>(ios_, std::chrono::seconds(timeout_sec_));
    timer_ptr->async_wait([socket_ptr, timer_ptr, this](const asio::error_code& ec) -> void
                          {
                              if (!ec) socket_ptr->cancel();
                          });
    socket_ptr->async_connect(asio::ip::tcp::endpoint(
                                  asio::ip::address::from_string(ip_), port),
                              [socket_ptr, timer_ptr, this, &port_list](const asio::error_code& ec) -> void
                              {
                                  if (ec)
                                      timer_ptr->cancel();
                                  else
                                      port_list.emplace_back(socket_ptr->remote_endpoint().port());
                                  CreateGetAllTask(port_list);
                              });
}

std::uint16_t CheckPort::GetOne()
{
    current_port_ = begin_port_;
    for (size_t i = 0; i < concurrency_num_; i++)
        CreateGetOneTask();
    ios_.reset();
    ios_.run();
    return target_port_;
}

std::vector<std::uint16_t> CheckPort::GetAll()
{
    current_port_ = begin_port_;
    std::vector<std::uint16_t> result;
    for (size_t i = 0; i < concurrency_num_; i++)
        CreateGetAllTask(result);
    ios_.reset();
    ios_.run();
    return result;
}

NAMESPACE_CHECKPORT_END