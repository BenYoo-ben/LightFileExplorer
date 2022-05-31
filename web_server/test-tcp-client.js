const TCPClient = require('./tcp-client');

var client = new TCPClient({ port: 55551, host: '218.48.36.143' });

client.get_dir_info('/home').then((file) => {
    console.log(file);
});
