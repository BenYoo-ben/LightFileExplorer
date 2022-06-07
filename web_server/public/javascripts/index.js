const currentUrl = window.location.href;

$(function () {
    const dirs = cur_dir.split('/');
    let url = '/';
    let ol = $('<ol>');
    ol.addClass('breadcrumb');
    for (let i = 0; i < dirs.length; i++) {
        let li = $('<li>');
        li.addClass('breadcrumb-item');
        if (i === dirs.length - 1) {
            li.text(dirs[i]);
            li.addClass('active');
        } else if (i === 0) {
            let a = $('<a>').attr('href', url).text(dirs[i]);
            li.append(a);
        } else {
            let encoded = encodeURIComponent('/' + dirs[i]);
            url = url + encoded;
            let a = $('<a>').attr('href', url).text(dirs[i]);
            li.append(a);
        }
        ol.append(li);
    }
    $('#dir').append(ol);
    let thead = $('<thead>').appendTo('.table');
    let tr = $('<tr>').appendTo(thead);
    let ico = $('<img>');
    ico.attr('src', '/images/file-earmark.svg');
    ico.attr('alt', 'folder');
    tr.addClass('table-secondary fw-bold align-middle');
    tr.append($('<td>').addClass('icon').append(ico));
    tr.append($('<td>').addClass('name').text('Name'));
    tr.append($('<td>').addClass('time').text('Modified'));
    tr.append($('<td>').addClass('size').text('File Size'));
    tr.append($('<td>').addClass('download'));
    for (let i = 0; i < json.length; i++) {
        let tr = $('<tr>').appendTo('.table');
        if (json[i]['is_dir'] === '1') {
            let td_icon = $('<td>');
            let ico = $('<img>');
            let td_name = $('<td>');
            const encoded = encodeURIComponent('/' + json[i]['name']);
            td_icon.addClass('icon');
            ico.attr('src', '/images/folder.svg');
            ico.attr('alt', 'folder');
            td_icon.append(ico);
            tr.append(td_icon);
            td_name.addClass('name');
            td_name.append($(`<a href="${currentUrl}${encoded}"></a>`).text(json[i]['name']));
            tr.append(td_name);
            tr.append($('<td>').text(json[i]['time']).addClass('time'));
            tr.append($('<td>').text('-').addClass('size'));
        } else {
            let td_icon = $('<td>');
            let ico = $('<img>');
            td_icon.addClass('icon');
            ico.attr('src', '/images/file-earmark.svg');
            ico.attr('alt', 'file');
            td_icon.append(ico);
            tr.append(td_icon);
            tr.append($('<td>').text(json[i]['name']).addClass('name'));
            tr.append($('<td>').text(json[i]['time']).addClass('time'));
            tr.append($('<td>').text(json[i]['size']).addClass('size'));
        }

        let td_dropdown = $('<td>');
        td_dropdown.addClass('dropdown');
        td_dropdown.html(`
        <a class="btn dropdown-toggle" href="#" role="button" id="dropdownMenuLink" data-bs-toggle="dropdown" aria-expanded="false">
            <img src="/images/three-dots-vertical.svg" alt="dropdown"/>
        </a>`);
        let dropdown_menu = $(`<ul class="dropdown-menu" aria-labelledby="dropdownMenuButton1"></ul>`);
        if (json[i]['is_dir'] === '0') {
            const encoded = encodeURIComponent('/' + json[i]['name']);
            let url = currentUrl + encoded + '/download';
            dropdown_menu.append($('<li>').append($('<a>').text('Download').addClass('dropdown-item').attr('href', url)));
        }
        dropdown_menu.append($('<li>').append($('<a>').text('Delete').addClass('dropdown-item').attr('href', '#')));
        dropdown_menu.append($('<li>').append($('<a>').text('Move to').addClass('dropdown-item').attr('href', '#')));
        dropdown_menu.append($('<li>').append($('<a>').text('Copy to').addClass('dropdown-item').attr('href', '#')));
        dropdown_menu.append($('<li>').append($('<a>').text('Rename').addClass('dropdown-item').attr('href', '#')));

        td_dropdown.append(dropdown_menu);
        tr.append(td_dropdown);
    }
});
