const currentUrl = window.location.href;

$(function () {
    const dirs = cur_dir.split('/');
    let url = '/';
    for (let i = 0; i < dirs.length; i++) {
        let a = $('<a>');
        a.text(dirs[i]);
        if (i === 0) {
            a.attr('href', url);
        } else {
            $('#dir').append($('<span>').text(' > '));
            let encoded = encodeURIComponent('/' + dirs[i]);
            url = url + encoded;
            a.attr('href', url);
        }
        $(a).appendTo('#dir');
    }
    let thead = $('<thead>').appendTo('.table');
    let tr = $('<tr>').appendTo(thead);
    let ico = $('<img>');
    ico.attr('src', '/images/folder.svg');
    ico.attr('alt', 'folder');
    tr.addClass('table-secondary fs-5 fw-bold');
    tr.append($('<td>').addClass('icon').append(ico));
    tr.append($('<td>').addClass('name').text('Name'));
    tr.append($('<td>').addClass('time').text('Modified'));
    tr.append($('<td>').addClass('size').text('File Size'));
    tr.append($('<td>').addClass('download'));
    for (let i = 0; i < json.length; i++) {
        let tr = $('<tr>').appendTo('.table');
        if (json[i]['is_dir'] === '1') {
            let td = $('<td>');
            let ico = $('<img>');
            td.addClass('icon');
            ico.attr('src', '/images/folder.svg');
            ico.attr('alt', 'folder');
            td.append(ico);
            tr.append(td);
        } else {
            let td = $('<td>');
            let ico = $('<img>');
            td.addClass('icon');
            ico.attr('src', '/images/file-earmark.svg');
            ico.attr('alt', 'file');
            td.append(ico);
            tr.append(td);
        }

        if (json[i]['is_dir'] === '1') {
            let td = $('<td>');
            td.addClass('name');
            const encoded = encodeURIComponent('/' + json[i]['name']);
            td.append($(`<a href="${currentUrl}${encoded}"></a>`).text(json[i]['name']));
            tr.append(td);
        } else {
            tr.append($('<td>').text(json[i]['name']).addClass('name'));
        }
        tr.append($('<td>').text(json[i]['time']).addClass('time'));
        if (json[i]['is_dir'] === '0') {
            let td = $('<td>');
            let a = $('<a>');
            let ico = $('<img>');
            const encoded = encodeURIComponent('/' + json[i]['name']);
            let url = currentUrl + encoded + '/download';
            tr.append($('<td>').text(json[i]['size']).addClass('size'));
            td.addClass('download');
            a.attr('href', url);
            ico.attr('src', '/images/arrow-down-square.svg');
            ico.attr('alt', 'file');
            a.append(ico);
            td.append(a);
            tr.append(td);
        } else {
            tr.append($('<td>').text('-').addClass('size'));
            tr.append($('<td>').addClass('download'));
        }
    }
});
