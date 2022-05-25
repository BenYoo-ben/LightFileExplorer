var net = require('net');

function getConnection(port, ip) {
    var recvData = [];

    var client = net.connect({ port: port, host: ip });

    client.on('data', function (data) {
        recvData.push(data);
        let data_size = recvData[0].slice(0, 4);
        data_size = data_size.readInt32LE(0);
        if (recvData.length > 1) {
            recvData[0] = Buffer.concat([recvData[0], recvData[1]]);
            recvData.pop();
        }
        if (data_size + 4 == recvData[0].length) {
            let str = recvData[0].slice(4).toString();
            recvData.pop();
            str = str.replace(/\s/g, '');
            // let json = JSON.parse(str);
            // console.log(json);
            console.log(str);
        }
    });

    return client;
}

function get_dir_info(socket, dir) {
    let dir_size = dir.length;
    let buf = new Buffer.alloc(1 + 4 + dir_size + 4);
    buf.writeInt8(6, 0);
    buf.writeInt32LE(dir_size, 1);
    for (let i = 0; i < dir_size; i++) {
        buf.writeInt8(dir[i].charCodeAt(0), i + 5);
    }
    socket.write(buf);
}

function download_file(socket, dir, file) {
    let dir_size = dir.length;
    let file_size = file.length;
    let buf = new Buffer.alloc(1 + 4 + dir_size + 4 + file_size);
    buf.writeInt8(1, 0);
    buf.writeInt32LE(dir_size, 1);
    for (let i = 0; i < dir_size; i++) {
        buf.writeInt8(dir[i].charCodeAt(0), i + 5);
    }
    buf.writeInt32LE(file_size, 1 + 4 + dir_size);
    for (let i = 0; i < file_size; i++) {
        buf.writeInt8(file[i].charCodeAt(0), i + 9 + dir_size);
    }
    socket.write(buf);
}

function copy_file(socket, src_dir, dst_dir) {
    let src_dir_size = src_dir.length;
    let dst_dir_size = dst_dir.length;
    let buf = new Buffer.alloc(1 + 4 + src_dir_size + 4 + dst_dir_size);
    buf.writeInt8(2, 0);
    buf.writeInt32LE(src_dir_size, 1);
    for (let i = 0; i < src_dir_size; i++) {
        buf.writeInt8(src_dir[i].charCodeAt(0), i + 5);
    }
    buf.writeInt32LE(dst_dir_size, 1 + 4 + src_dir_size);
    for (let i = 0; i < dst_dir_size; i++) {
        buf.writeInt8(dst_dir[i].charCodeAt(0), i + 9 + src_dir_size);
    }
    socket.write(buf);
}

function move_file(socket, src_dir, dst_dir) {
    let src_dir_size = src_dir.length;
    let dst_dir_size = dst_dir.length;
    let buf = new Buffer.alloc(1 + 4 + src_dir_size + 4 + dst_dir_size);
    buf.writeInt8(3, 0);
    buf.writeInt32LE(src_dir_size, 1);
    for (let i = 0; i < src_dir_size; i++) {
        buf.writeInt8(src_dir[i].charCodeAt(0), i + 5);
    }
    buf.writeInt32LE(dst_dir_size, 1 + 4 + src_dir_size);
    for (let i = 0; i < dst_dir_size; i++) {
        buf.writeInt8(dst_dir[i].charCodeAt(0), i + 9 + src_dir_size);
    }
    socket.write(buf);
}

function delete_file(socket, dir, file) {
    let dir_size = dir.length;
    let file_size = file.length;
    let buf = new Buffer.alloc(1 + 4 + dir_size + 4 + file_size);
    buf.writeInt8(4, 0);
    buf.writeInt32LE(dir_size, 1);
    for (let i = 0; i < dir_size; i++) {
        buf.writeInt8(dir[i].charCodeAt(0), i + 5);
    }
    buf.writeInt32LE(file_size, 1 + 4 + dir_size);
    for (let i = 0; i < file_size; i++) {
        buf.writeInt8(file[i].charCodeAt(0), i + 9 + dir_size);
    }
    socket.write(buf);
}

function rename_file(socket, src_dir, new_file_dir) {
    let src_dir_size = src_dir.length;
    let new_dir_size = new_file_dir.length;
    let buf = new Buffer.alloc(1 + 4 + src_dir_size + 4 + new_dir_size);
    buf.writeInt8(5, 0);
    buf.writeInt32LE(src_dir_size, 1);
    for (let i = 0; i < src_dir_size; i++) {
        buf.writeInt8(src_dir[i].charCodeAt(0), i + 5);
    }
    buf.writeInt32LE(new_dir_size, 1 + 4 + src_dir_size);
    for (let i = 0; i < new_dir_size; i++) {
        buf.writeInt8(new_file_dir[i].charCodeAt(0), i + 9 + src_dir_size);
    }
    socket.write(buf);
}

var client = getConnection(55551, '218.48.36.143');
// copy_file(client, '/home/pi/test.go', '/home');
// rename_file(client, '/home/test.go', '/home/test2.go');
get_dir_info(client, '/home');
