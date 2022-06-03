var express = require('express');
var router = express.Router();
var fs = require('fs');
var path = require('path');
var TCPClient = require('../tcp-client');

var tcp_server = fs.readFileSync('tcp-server.json');
tcp_server = JSON.parse(tcp_server);

/* GET home page. */
router.get('/', function (req, res, next) {
    var client = new TCPClient({ port: tcp_server.port, host: tcp_server.host });
    client.get_dir_info('/home').then((json) => {
        json.sort((a, b) => {
            if (a.is_dir == b.is_dir) {
                return ('' + a.name).localeCompare(b.name);
            }
            return a.is_dir < b.is_dir ? 1 : -1;
        });
        res.render('index', { json: json, cur_dir: '/home' });
        client.socket.destroy();
    });
});

router.get('/:dir', function (req, res, next) {
    var dir = req.params.dir;
    const decoded = decodeURIComponent(dir);
    var client = new TCPClient({ port: tcp_server.port, host: tcp_server.host });
    client.get_dir_info('/home' + decoded).then((json) => {
        json.sort((a, b) => {
            if (a.is_dir == b.is_dir) {
                return ('' + a.name).localeCompare(b.name);
            }
            return a.is_dir < b.is_dir ? 1 : -1;
        });
        res.render('index', { json: json, cur_dir: '/home' + decoded });
        client.socket.destroy();
    });
});

router.get('/:dir/download', function (req, res, next) {
    var dir = req.params.dir;
    var decoded = decodeURIComponent(dir);
    var req_dir = path.dirname(decoded) + '/';
    if (req_dir === '//') {
        req_dir = '/';
    }
    decoded = decoded.replace(req_dir, '');
    // console.log('dir : ' + req_dir + typeof req_dir + ' file : ' + decoded + typeof decoded);
    var client = new TCPClient({ port: tcp_server.port, host: tcp_server.host });
    client.download_file('/home' + req_dir, decoded).then((file) => {
        res.writeHead(200, {
            'Content-disposition': 'attachment;filename=' + decoded,
            'Content-Length': file.length,
        });
        res.end(file);
        client.socket.destroy();
    });
});

module.exports = router;
