const express = require('express');
const router = express.Router();
const fs = require('fs');
const path = require('path');
const TCPClient = require('../tcp-client');

const tcp_server = JSON.parse(fs.readFileSync('tcp-server.json'));

// Get Home directory's info
router.get('/', async (req, res, next) => {
    res.redirect('/home');
});

// Get :dir's info
router.get('/:dir', async (req, res, next) => {
    try {
        let client = new TCPClient({ port: tcp_server.port, host: tcp_server.host });
        let json = await client.get_dir_info('/' + req.params.dir);
        json.sort((a, b) => {
            if (a.is_dir == b.is_dir) {
                return ('' + a.name).localeCompare(b.name);
            }
            return a.is_dir < b.is_dir ? 1 : -1;
        });
        res.render('index', { json: json, cur_dir: req.params.dir });
        client.socket.destroy();
    } catch (err) {
        console.log(err);
        client.socket.destroy();
    }
});

// Download File
router.get('/:dir/download', async (req, res, next) => {
    try {
        let dir = req.params.dir;
        let decoded = decodeURIComponent(dir);
        let req_dir = path.dirname(decoded) + '/';
        decoded = decoded.replace(req_dir, '');
        let client = new TCPClient({ port: tcp_server.port, host: tcp_server.host });
        let fileSize = await client.download_file_get_size('/' + req_dir, decoded);

        await client.download_file('/' + req_dir, decoded, fileSize);

        console.log('Download Start...');
        res.download('./downloads/' + decoded, (err) => {
            if (err) {
                console.log(err);
            }
            client.socket.destroy();
            fs.unlink('./downloads/' + decoded, (err) => {
                if (err && err.code == 'ENOENT') {
                    console.log('File Unlink Err: File Does not Exist');
                } else if (err) {
                    console.log('File Unlink Err: Other');
                }
            });
            console.log('Download Done... ! ');
        });
    } catch (err) {
        console.log(err);
        client.socket.destroy();
    }
});

// Delete Method
router.delete('/:dir/delete', async (req, res, next) => {
    try {
        let dir = req.params.dir;
        let req_dir = path.dirname(dir) + '/';
        let file = dir.replace(req_dir, '');
        let client = new TCPClient({ port: tcp_server.port, host: tcp_server.host });
        let msg = await client.delete_file('/' + req_dir, file);
        console.log(msg);
        client.socket.destroy();
        res.status(200).end();
    } catch (err) {
        console.log(err);
        client.socket.destroy();
    }
});

// File Upload Route
router.post('/:dir/upload', (req, res, next) => {
    let dir = req.params.dir;
    let req_dir = dir + '/';
    let file = req.files.fileUploaded;

    console.log('File Uploaded ! ');
    file.mv('uploads/' + file.name, async (err) => {
        if (err) {
            return res.status(500).send(err);
        }

        // handle err or resolve
        try {
            var client = new TCPClient({ port: tcp_server.port, host: tcp_server.host });
            let msg = await client.upload_file('/' + req_dir, file.name);
            console.log(msg);
            client.socket.destroy();
            fs.unlink('uploads/' + file.name, function (err) {
                if (err && err.code == 'ENOENT') {
                    console.log('File Unlink Err: File Does not Exist');
                } else if (err) {
                    console.log('File Unlink Err: Other');
                }
            });
            res.redirect('/' + req.params.dir);
        } catch {
            console.log(err);
            client.socket.destroy();
        }
    });
});

// File Move Route
router.put('/:dst/:src/move', async (req, res, next) => {
    try {
        const src_dir = '/' + req.params.src;
        const dst_dir = '/' + req.params.dst + req.params.src.replace(path.dirname(req.params.src), '');
        let client = new TCPClient({ port: tcp_server.port, host: tcp_server.host });
        let msg = await client.move_file(src_dir, dst_dir);
        console.log(msg);
        client.socket.destroy();
        res.status(200).end();
    } catch (err) {
        console.log(err);
        client.socket.destroy();
    }
});

// File Copy Route
router.post('/:dst/:src/copy', async (req, res, next) => {
    try {
        const src_dir = '/' + req.params.src;
        const dst_dir = '/' + req.params.dst + req.params.src.replace(path.dirname(req.params.src), '');
        let client = new TCPClient({ port: tcp_server.port, host: tcp_server.host });
        let msg = await client.copy_file(src_dir, dst_dir);
        console.log(msg);
        client.socket.destroy();
        res.status(200).end();
    } catch (err) {
        console.log(err);
        client.socket.destroy();
    }
});

// File duplicate Route
router.post('/:dst/:src/dup', async (req, res, next) => {
    try {
        const src_dir = '/' + req.params.src;
        let fileName = req.params.src.replace(path.dirname(req.params.src), '');
        let fileType = fileName.substring(fileName.lastIndexOf('.') + 1, fileName.length);
        fileName = fileName.substring(0, fileName.lastIndexOf('.'));
        const dst_dir = '/' + req.params.dst + fileName + '_dup.' + fileType;
        console.log(dst_dir);
        let client = new TCPClient({ port: tcp_server.port, host: tcp_server.host });
        let msg = await client.copy_file(src_dir, dst_dir);
        console.log(msg);
        client.socket.destroy();
        res.status(200).end();
    } catch (err) {
        console.log(err);
        client.socket.destroy();
    }
});

// File rename Route
router.put('/:src/:dst/rename', async (req, res, next) => {
    try {
        const src_dir = '/' + req.params.src;
        const dst_dir = '/' + path.dirname(req.params.src) + '/' + req.params.dst;
        let client = new TCPClient({ port: tcp_server.port, host: tcp_server.host });
        let msg = await client.rename_file(src_dir, dst_dir);
        console.log(msg);
        client.socket.destroy();
        res.status(200).end();
    } catch (err) {
        console.log(err);
        client.socket.destroy();
    }
});

module.exports = router;
