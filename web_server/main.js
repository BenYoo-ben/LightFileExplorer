const TCPClient = require('./tcp-client');

var client = new TCPClient({ port: 55551, host: '218.48.36.143' });
var client2 = new TCPClient({ port: 55551, host: '218.48.36.143' });

setTimeout(() => {
    client.get_dir_info('/home');
    setTimeout(() => {
        console.log(client.socket.json);
    }, 1000);
}, 1000);

setTimeout(() => {
    client2.download_file('/home/', 'test');
    setTimeout(() => {
        console.log(client2.socket.file);
    }, 1000);
}, 1000);
