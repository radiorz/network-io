const net = require("net");

const client = new net.Socket();
const host = "127.0.0.1"; // 服务器的 IP 地址
const port = 3000; // 服务器监听的端口号

// 连接到服务器
client.connect(port, host, () => {
  console.log("Connected to server");

  // 发送数据给服务器
  client.write("Hello, server!");
});

// 接收服务器发送的数据
client.on("data", (args) => {
  console.log("Received data:", data.toString());
  console.log(`...args`, ...args);
});

// 监听连接关闭事件
client.on("close", () => {
  console.log("Connection closed");
});
