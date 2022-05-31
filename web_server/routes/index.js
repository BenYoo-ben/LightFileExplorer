var express = require('express');
var router = express.Router();
var TCPClient = require('../tcp-client');

/* GET home page. */
router.get('/', function (req, res, next) {
    var client = new TCPClient({ port: 55551, host: '218.48.36.143' });
    client.get_dir_info('/').then((json) => {
        res.render('index', { json: json, cur_dir: '/' });
        client.socket.destroy();
    });
});

module.exports = router;
