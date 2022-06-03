var express = require('express');
var router = express.Router();
var fs = require('fs');
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

module.exports = router;
