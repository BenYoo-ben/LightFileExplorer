var net = require('net');
var socket = new net.Socket();
socket.connect({ port: 55551, host: '218.48.36.143' }, function () {
    socket.on('data', function (data) {
        var str = data.toString();
        console.log(str);
    });
});

const get_dir_info = function (dir) {
    let dir_size = dir.length;

    let buf = new Uint8Array(1 + 4 + dir_size + 4);
    buf[0] = 6;
    buf[1] = dir_size;
    for (let i = 0; i < dir_size; i++) {
        buf[i + 5] = dir[i].charCodeAt(0);
    }
    socket.write(buf);
};

get_dir_info('/home');
