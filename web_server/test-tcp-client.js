const TCPClient = require('./tcp-client');
const fs = require('fs');
let tcp_server = fs.readFileSync('./tcp-server.json', { encoding: 'utf-8' });

tcp_server = JSON.parse(tcp_server);

var client = new TCPClient({ port: tcp_server.port, host: tcp_server.host });
client.get_dir_info('/home').then((file) => {
    console.log(file);
});
