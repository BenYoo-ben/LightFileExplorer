var express = require('express');
var router = express.Router();
var TCPClient = require('../tcp-client');

/* GET home page. */
router.get('/', function (req, res, next) {
    var client = new TCPClient({ port: 55551, host: '218.48.36.143' });
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
    var client = new TCPClient({ port: 55551, host: '218.48.36.143' });
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
