#include <stdio.h>      // 标准输入输出库
#include <stdlib.h>     // 标准库
#include <string.h>     // 字符串操作库
#include <unistd.h>     // UNIX 标准库
#include <sys/socket.h> // socket 库
#include <netinet/in.h> // 网络库
#include <sys/epoll.h>  // epoll 库

#define MAX_EVENTS 10    // 最大事件数量
#define BUFFER_SIZE 1024 // 缓冲区大小

int main()
{
  // --------------------------------
  // 变量
  int server_fd, new_socket, epoll_fd, event_count, i;
  // 地址
  struct sockaddr_in address;
  // epoll 事件
  struct epoll_event event, events[MAX_EVENTS];
  // 缓冲区
  char buffer[BUFFER_SIZE];
  // --------------------------------

  // 创建监听 socket
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  // 监听失败
  if (server_fd == -1)
  {
    perror("Failed to create socket"); // 失败打印错误信息
    exit(EXIT_FAILURE);                // 退出程序
  }

  // 设置地址信息
  address.sin_family = AF_INET;         // 地址族为 IPv4
  address.sin_addr.s_addr = INADDR_ANY; // 接受任意来源的连接
  address.sin_port = htons(3000);       // 监听3000端口

  // 绑定地址
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1)
  {
    perror("Failed to bind address");
    exit(EXIT_FAILURE);
  }

  // 监听连接
  if (listen(server_fd, 5) == -1)
  {
    perror("Failed to listen");
    exit(EXIT_FAILURE);
  }

  // 创建 epoll 实例
  epoll_fd = epoll_create1(0);
  if (epoll_fd == -1)
  {
    perror("Failed to create epoll instance");
    exit(EXIT_FAILURE);
  }

  // 将监听 socket 添加到 epoll 实例中
  // 这部分代码将监听 socket 添加到 epoll 实例中，关联的事件为可读事件(EPOLLIN)。
  event.events = EPOLLIN;    // 监听可读事件
  event.data.fd = server_fd; // 关联文件描述符

  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1)
  {
    // 添加监听 socket 失败打印错误信息
    perror("Failed to add server socket to epoll");
    exit(EXIT_FAILURE);
  }

  printf("Server listening on port 3000\n");

  while (1)
  {
    // 监听事件
    event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1); // 等待事件发生

    // 等待失败打印错误信息
    if (event_count == -1)
    {
      perror("Failed to wait for events");
      exit(EXIT_FAILURE);
    }

    // 处理所有事件
    for (i = 0; i < event_count; i++)
    {

      // 如果是监听 socket 上有事件发生
      if (events[i].data.fd == server_fd)
      {
        // 新连接
        new_socket = accept(server_fd, NULL, NULL); // 接受新的连接
        if (new_socket == -1)
        {
          perror("Failed to accept connection");
          exit(EXIT_FAILURE);
        }

        // 将新连接的 socket 添加到 epoll 实例中
        event.events = EPOLLIN;     // 监听可读事件
        event.data.fd = new_socket; // 关联新连接的文件描述符

        // 添加新连接失败打印错误信息
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_socket, &event) == -1)
        {
          perror("Failed to add new socket to epoll");
          exit(EXIT_FAILURE);
        }

        printf("New connection accepted\n");
      }
      else
      {
        // 如果是其他 socket 上有事件发生
        // 已连接 socket 有数据可读
        int socket_fd = events[i].data.fd;
        int bytes_read = recv(socket_fd, buffer, BUFFER_SIZE, 0); // 读取数据

        // 读取数据失败打印错误信息
        if (bytes_read <= 0)
        {
          // 连接关闭或出错
          epoll_ctl(epoll_fd, EPOLL_CTL_DEL, socket_fd, NULL);
          close(socket_fd);
          printf("Connection closed\n");
        }
        else
        {
          // 处理接收到的数据
          printf("Received data: %s\n", buffer);
        }
      }
    }
  }
  // 关闭监听 socket
  close(server_fd);
  close(epoll_fd);

  return 0;
}
