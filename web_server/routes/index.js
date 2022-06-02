var express = require('express');
var router = express.Router();
var TCPClient = require('../tcp-client');

/* GET home page. */
router.get('/', function (req, res, next) {
    var client = new TCPClient({ port: 55551, host: '218.48.36.143' });
    client.get_dir_info('/home').then((json) => {
        res.render('index', { json: json, cur_dir: '/home' });
        client.socket.destroy();
    });
});

router.get('/:dir', function (req, res, next) {
    var dir = req.params.dir;
    dir = dir.replaceAll('+', '/');
    console.log(dir);
    var client = new TCPClient({ port: 55551, host: '218.48.36.143' });
    client.get_dir_info('/home' + dir).then((json) => {
        res.render('index', { json: json, cur_dir: '/home' + dir });
        client.socket.destroy();
    });
});

module.exports = router;
