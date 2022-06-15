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
            this.socket.on('data', (data) => {
                recvData.push(data);
                let data_size = recvData[0].slice(0, 4);
                data_size = data_size.readInt32LE(0);
                if (data_size == 0) {
                    let ackVal = parseInt(recvData.pop()) >>> 0;
                    if (ackVal != 0) {
                        reject('Get Directory Information failed on Server side');
                    }
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


    download_file_get_size(dir ,file) {
        return new Promise((resolve, reject) => {

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

            let fTotalSize = -1;

            this.socket.write(buf);

            this.socket.on('data', function (data) {
                let fTotalSize = data.slice(0, 4);
                fTotalSize = fTotalSize.readInt32LE(0);

                if (fTotalSize < 0) {
                    reject("Not wanted file Size");
                } else {
                    resolve(fTotalSize);
                }
            });
        });
    }
    // Download a file
    // ex) download_file('/home/', 'test');
    download_file(dir, file, fileSize) {
        return new Promise((resolve, reject) => {
                        
            let fProcessedSize = 0;
            
            let fileName = 'downloads/' + file;

            let buf = new Buffer.alloc(4);
            buf.writeInt32LE(0);
            console.log(fileName);

            this.socket.write(buf);

            console.log("Sent ACK : " + buf);

            let writeSeq = 0; 
            this.socket.on('data', function (data) {
                fProcessedSize += data.length;

                if (writeSeq == 0) {
                    /*fs.writeFile(fileName, data, (err) => {
                        if (err) {
                            console.log(err);
                            throw err;
                        }     
                        console.log("[" + fProcessedSize + "/" + fileSize + "]" +"\nwritten to " + fileName);
                
                    });*/
                    fs.writeFileSync(fileName, data);
                    console.log("[" + fProcessedSize + "/" + fileSize + "]" +"\nwritten to " + fileName);

                    writeSeq++;
                } else {
/*                     fs.appendFile(fileName, data, (err) => {
                        if (err) {
                            console.log(err);
                            throw err;
                        }     
                        console.log("[" + fProcessedSize + "/" + fileSize + "]" +"\nappended to " + fileName);
                
                    });             */
                    fs.appendFileSync(fileName, data);
                    console.log("[" + fProcessedSize + "/" + fileSize + "]" +"\nappended to " + fileName);

                } 
                if (fProcessedSize  >= fileSize) {
                    console.log("fProcess is Done ! "); 
                    resolve(0);
                } 
            });
        });
    }

    // Copy file
    // ex) copy_file('/home/test', '/home/pi/test');
    copy_file(src_dir, dst_dir) {
        return new Promise((resolve, reject) => {
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

            this.socket.on('data', function (data) {
                let ackVal = parseInt(data) >>> 0;
                if (ackVal == 0) {
                    resolve('File Copy Success');
                } else {
                    reject('File Copy failed on Server side');
                }
            });
        });
    }

    // Move file
    // ex) move_file('/home/test', '/home/test2');
    move_file(src_dir, dst_dir) {
        return new Promise((resolve, reject) => {
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

            this.socket.on('data', function (data) {
                let ackVal = parseInt(data) >>> 0;
                if (ackVal == 0) {
                    resolve('File Move Success');
                } else {
                    reject('File Move failed on Server side');
                }
            });
        });
    }

    // Delete file
    // ex) delete_file('/home/', 'test');
    delete_file(dir, file) {
        return new Promise((resolve, reject) => {
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

            this.socket.on('data', function (data) {
                let ackVal = parseInt(data) >>> 0;
                if (ackVal == 0) {
                    resolve('File Delete Success');
                } else {
                    reject('File Delete failed on Server side');
                }
            });
        });
    }

    // Rename file
    // ex) rename_file('/home/test', '/home/test2');
    rename_file(src_dir, new_file_dir) {
        return new Promise((resolve, reject) => {
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

            this.socket.on('data', function (data) {
                let ackVal = parseInt(data) >>> 0;
                if (ackVal == 0) {
                    resolve('File Rename Success');
                } else {
                    reject('File Rename failed on Server side');
                }
            });
        });
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
            this.socket.on('data', (data) => {
                let ackVal = parseInt(data) >>> 0;
                // make to unsigned
                if (ackVal == 0) {
                    var fstats = fs.statSync('./uploads/' + file);
                    var fileSize = fstats.size >>> 0;
                    var fSizeBuf = Buffer.alloc(4);
                    fSizeBuf.writeInt32LE(fileSize, 0);
                    this.socket.write(fSizeBuf);
                    console.log('FILE SIZE : ' + fileSize);
                    fs.readFile('./uploads/' + file, (err, data) => {
                        if (err) {
                            throw err;
                        }
                        this.socket.write(data);
                        resolve('Upload Process Success');
                    });
                } else {
                    reject('Upload Process Failed');
                }
            });
        });
    }
}

module.exports = TCPClient;
