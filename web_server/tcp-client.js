const fs = require('fs');
const net = require('net');

class TCPClient {
    constructor({ port, host, timeout }) {
        this.socket = this.create({ port, host, timeout });
    }

    // Create TCP connection to File Server
    create({ port, host, timeout }) {
        this.timeout = timeout;

        const client = net.connect({ port, host });

        client.on('connect', () => {
            this.socket = client;
            console.log('tcp connect success');
            client.setTimeout(this.timeout || 10000);
        });

        client.on('close', function () {});
        client.on('error', function (err) {
            console.log('Client Socket Error: ' + JSON.stringify(err));
        });

        return client;
    }

    // Get directory's information
    // ex) get_dir_info('home');
    get_dir_info(dir) {
        return new Promise((resolve, reject) => {
            let dir_size = dir.length;
            let buf = new Buffer.alloc(1 + 4 + dir_size + 4);
            let recvData = [];
            buf.writeInt8(6, 0);
            buf.writeInt32LE(dir_size, 1);
            for (let i = 0; i < dir_size; i++) {
                buf.writeInt8(dir[i].charCodeAt(0), i + 5);
            }
            this.socket.write(buf);
            this.socket.on('data', function (data) {
                recvData.push(data);
                let data_size = recvData[0].slice(0, 4);
                data_size = data_size.readInt32LE(0);
                if (data_size == 0) {
                    recvData.pop();
                }
                if (recvData.length > 1) {
                    recvData[0] = Buffer.concat([recvData[0], recvData[1]]);
                    recvData.pop();
                }
                if (recvData.length && data_size + 4 === recvData[0].length) {
                    let str = recvData[0].slice(4).toString();
                    recvData.pop();
                    str = str.replace(/(\r\n|\n|\r)/gm, '');
                    let json = JSON.parse(str);
                    resolve(json);
                }
            });
        });
    }

    // Download a file
    // ex) download_file('/home/', 'test');
    download_file(dir, file) {
        return new Promise((resolve, reject) => {
            let dir_size = dir.length;
            let file_size = file.length;
            let recvData = [];
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
            this.socket.write(buf);

            this.socket.on('data', function (data) {
                recvData.push(data);
                let data_size = recvData[0].slice(0, 4);
                data_size = data_size.readInt32LE(0);
                if (data_size == 0) {
                    recvData.pop();
                }
                if (recvData.length > 1) {
                    recvData[0] = Buffer.concat([recvData[0], recvData[1]]);
                    recvData.pop();
                }
                if (recvData.length && data_size + 4 == recvData[0].length) {
                    let file = recvData[0].slice(4);
                    recvData.pop();
                    resolve(file);
                }
            });
        });
    }

    // Copy file
    // ex) copy_file('/home/test', '/home/pi/test');
    copy_file(src_dir, dst_dir) {
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
        this.socket.write(buf);
    }

    // Move file
    // ex) move_file('/home/test', '/home/test2');
    move_file(src_dir, dst_dir) {
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
        this.socket.write(buf);
    }

    // Delete file
    // ex) delete_file('/home/', 'test');
    delete_file(dir, file) {
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
        this.socket.write(buf);
    }

    // Rename file
    // ex) rename_file('/home/test', '/home/test2');
    rename_file(src_dir, new_file_dir) {
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
        this.socket.write(buf);
    }

    upload_file(dir, file) {
        return new Promise((resolve, reject) => {
            console.log('DIR : ' + dir + '     FILE : ' + file);
            let dir_size = dir.length;
            let file_size = file.length;
            let buf = new Buffer.alloc(1 + 4 + dir_size + 4 + file_size);

            // protocol 7: UploadFile
            buf.writeInt8(7, 0);
            buf.writeInt32LE(dir_size, 1);
            for (let i = 0; i < dir_size; i++) {
                buf.writeInt8(dir[i].charCodeAt(0), 1 + 4 + i);
            }
            buf.writeInt32LE(file_size, 1 + 4 + dir_size);
            for (let i = 0; i < file_size; i++) {
                buf.writeInt8(file[i].charCodeAt(0), 1 + 4 + dir_size + 4 + i);
            }
            this.socket.write(buf);
            let recvBuf = new Buffer.alloc(4);
            // get ack from file server
            this.socket.on('data', function (data) {
                let ackVal = parseInt(data[0]) >>> 0;
                // make to unsigned
                if (ackVal == 0) {
                    var fstats = fs.statSync('./uploads/' + file);
                    var fileSize = fstats.size >>> 0;
                    var fSizeBuf = Buffer.alloc(4);
                    fSizeBuf.writeInt32LE(fileSize, 0);
                    this.write(fSizeBuf);
                    console.log('FILE SIZE : ' + fileSize);
                    fs.readFile('./uploads/' + file, (err, data) => {
                        if (err) {
                            throw err;
                        }
                        this.write(data);
                        resolve(0);
                    });
                } else {
                    resolve(1);
                }
            });
        });
    }
}

module.exports = TCPClient;
