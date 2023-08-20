
const net = require('net');

const server = net.createServer((socket) => {
  // 处理新连接
  socket.on('data', (data) => {
    // 处理接收到的数据
    console.log('Received data:', data.toString());
  });

  socket.on('end', () => {
    // 连接关闭时的处理
    console.log('Connection closed');
  });
});

server.listen(3000, '127.0.0.1', () => {
  console.log('Server listening on port 3000');
});
