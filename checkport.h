#ifndef __CHECK_PORT_H__
#define __CHECK_PORT_H__
#include "asio.hpp"
#include <vector>

#define NAMESPACE_CHECKPORT_START \
    namespace checkport           \
    {

#define NAMESPACE_CHECKPORT_END }

NAMESPACE_CHECKPORT_START
class CheckPort
{
public:
    CheckPort(const std::string& ip, unsigned short begin_port, unsigned short end_port,
              std::uint8_t concurrency_num = 10,
              size_t timeout_sec_ = 1);
    ~CheckPort();
    void SetConcurrency(std::uint8_t num);
    void SetConnectTimeout(size_t timeout_sec);
    void Assign(const std::string& ip, unsigned short begin_port, unsigned short end_port);

    // if return zero,no port is available
    // if >0 target port is available
    std::uint16_t GetOne();

    // return a vector hold accessable ports in the range.
    std::vector<std::uint16_t> GetAll();

protected:
    void CreateGetOneTask();
    void CreateGetAllTask(std::vector<std::uint16_t>& port_list);
    asio::io_service ios_;
    std::string ip_;
    std::uint16_t begin_port_;
    std::uint16_t end_port_;
    std::uint16_t current_port_;
    std::uint8_t concurrency_num_;
    size_t timeout_sec_;
    std::uint16_t target_port_;
};
NAMESPACE_CHECKPORT_END

#ifdef CHECK_PORT_HEADER_ONLY
#include "checkport.cpp"
#endif

#endif