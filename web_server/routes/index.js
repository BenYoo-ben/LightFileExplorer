const express = require('express');
const router = express.Router();
const fs = require('fs');
const path = require('path');
const TCPClient = require('../tcp-client');

let tcp_server = fs.readFileSync('tcp-server.json');
tcp_server = JSON.parse(tcp_server);

// Get Home directory's info
router.get('/', async (req, res, next) => {
    res.redirect('/home');
});

// Get :dir's info
router.get('/:dir', async (req, res, next) => {
    let dir = req.params.dir;
    const decoded = decodeURIComponent(dir);
    let client = new TCPClient({ port: tcp_server.port, host: tcp_server.host });
    let json = await client.get_dir_info('/' + decoded);
    json.sort((a, b) => {
        if (a.is_dir == b.is_dir) {
            return ('' + a.name).localeCompare(b.name);
        }
        return a.is_dir < b.is_dir ? 1 : -1;
    });
    res.render('index', { json: json, cur_dir: decoded });
    client.socket.destroy();
});

// Download File
router.get('/:dir/download', async (req, res, next) => {
    let dir = req.params.dir;
    let decoded = decodeURIComponent(dir);
    let req_dir = path.dirname(decoded) + '/';
    decoded = decoded.replace(req_dir, '');
    let client = new TCPClient({ port: tcp_server.port, host: tcp_server.host });
    let file = await client.download_file('/' + req_dir, decoded);
    res.writeHead(200, {
        'Content-disposition': 'attachment;filename=' + decoded,
        'Content-Length': file.length,
    });
    res.end(file);
    client.socket.destroy();
});

// File Upload Route
var fileupload = require('express-fileupload');
router.use(fileupload());
router.post('/:dir/upload', (req, res, next) => {
    var dir = req.params.dir;
    var decoded = decodeURIComponent(dir);
    var req_dir = decoded + '/';

    console.log('req_dir: ' + req_dir);
    console.log('dir : ' + dir);
    console.log('decoded:' + decoded);

    var file;

    if (!req.files) {
        res.send('File was not found');
        return;
    }

    file = req.files.fileUploaded;
    console.log('File Uploaded ! ');
    file.mv('uploads/' + file.name, async (err) => {
        if (err) {
            return res.status(500).send(err);
        }

        var client = new TCPClient({ port: tcp_server.port, host: tcp_server.host });
        let retVal = await client.upload_file('/' + req_dir, file.name);
        // handle err or resolve
        if (retVal == 0) {
            console.log('Upload Process Success');
            client.socket.destroy();
            res.redirect('/' + req.params.dir);
        } else {
            console.log('Upload Process Failed');
            client.socket.destroy();
        }
    });
});

module.exports = router;
