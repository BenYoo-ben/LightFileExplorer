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
        let req_dir = path.dirname(dir) + '/';
        let file_name = dir.replace(req_dir, '');
        let client = new TCPClient({ port: tcp_server.port, host: tcp_server.host });
        let file = await client.download_file('/' + req_dir, file_name);
        res.writeHead(200, {
            'Content-disposition': 'attachment;filename=' + decodeURI(file_name),
            'Content-Length': file.length,
        });
        res.end(file);
        client.socket.destroy();
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
    let redirect_dir = encodeURIComponent(dir);
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
            res.redirect('/' + redirect_dir);
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

module.exports = router;
