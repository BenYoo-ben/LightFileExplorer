const currentUrl = window.location.href;

let move_src = sessionStorage.getItem('move_src');
let file_name = sessionStorage.getItem('file_name');
console.log(move_src);
console.log(file_name);

// for upload UI, dynamically make UI
var uploadForm = document.createElement('form');
uploadForm.setAttribute('method', 'post');
uploadForm.setAttribute('action', currentUrl + '/upload');
uploadForm.setAttribute('enctype', 'multipart/form-data');
uploadForm.setAttribute('style', 'margin: 2% 5%;');

var uploadGetFile = document.createElement('input');
var uploadButton = document.createElement('input');
uploadGetFile.setAttribute('type', 'file');
uploadGetFile.setAttribute('name', 'fileUploaded');

uploadButton.setAttribute('type', 'image');
uploadButton.setAttribute('src', '/images/cloud-upload.svg');
uploadForm.appendChild(uploadGetFile);
uploadForm.appendChild(uploadButton);

$(function () {
    document.body.append(uploadForm);

    // Show directory hierarchy using bootstrap breadcrumb
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

    // With json file from tcp file server, dynamically make table

    // Create table headers
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

    // Filling table data
    for (let i = 0; i < json.length; i++) {
        let tr = $('<tr>').appendTo('.table');
        if (json[i]['is_dir'] === '1') {
            let td_icon = $('<td>');
            let ico = $('<img>');
            let td_name = $('<td>');
            const encoded = encodeURIComponent('/' + json[i]['name']);

            // Icon for a directory
            td_icon.addClass('icon');
            ico.attr('src', '/images/folder.svg');
            ico.attr('alt', 'folder');
            td_icon.append(ico);
            tr.append(td_icon);

            // Directory name with its url
            td_name.addClass('name');
            td_name.append($(`<a href="${currentUrl}${encoded}"></a>`).text(json[i]['name']));
            tr.append(td_name);

            // Modified time
            tr.append($('<td>').text(json[i]['time']).addClass('time'));

            // Directory size
            tr.append($('<td>').text('-').addClass('size'));
        } else {
            let td_icon = $('<td>');
            let ico = $('<img>');

            // Icon for a file
            td_icon.addClass('icon');
            ico.attr('src', '/images/file-earmark.svg');
            ico.attr('alt', 'file');
            td_icon.append(ico);
            tr.append(td_icon);

            // File name, modified time, size
            tr.append($('<td>').text(json[i]['name']).addClass('name'));
            tr.append($('<td>').text(json[i]['time']).addClass('time'));
            tr.append($('<td>').text(json[i]['size']).addClass('size'));
        }

        // Dropdown buttons for each directories or files

        let td_dropdown = $('<td>');
        td_dropdown.addClass('dropdown');
        td_dropdown.html(`
        <a class="btn dropdown-toggle" href="#" role="button" id="dropdownMenuLink" data-bs-toggle="dropdown" aria-expanded="false">
            <img src="/images/three-dots-vertical.svg" alt="dropdown"/>
        </a>`);
        let dropdown_menu = $(`<ul class="dropdown-menu" aria-labelledby="dropdownMenuButton1"></ul>`);
        let li_move = $('<li>').append($('<button>').text('Move to').addClass('dropdown-item').attr('type', 'button'));

        if (json[i]['is_dir'] === '0') {
            const encoded = encodeURIComponent('/' + json[i]['name']);
            let download_url = currentUrl + encoded + '/download';
            let delete_url = currentUrl + encoded + '/delete';

            // Download a file
            dropdown_menu.append($('<li>').append($('<a>').text('Download').addClass('dropdown-item').attr('href', download_url)));

            // Delete a file
            dropdown_menu.append($('<li>').append($('<a>').text('Delete').addClass('dropdown-item').attr('href', delete_url)));

            li_move.click(() => {
                const toast = new bootstrap.Toast($('#toast'));
                const decoded = decodeURIComponent(currentUrl);
                $('.toast-body').text('Move ' + json[i]['name']);
                toast.show();
                sessionStorage.setItem('move_src', decoded + '/' + json[i]['name']);
                sessionStorage.setItem('file_name', json[i]['name']);
            });
        } else {
            dropdown_menu.append($('<li>').append($('<a>').text('Delete').addClass('dropdown-item').attr('href', '')));
        }

        dropdown_menu.append(li_move);
        // Buttons for later purposes
        dropdown_menu.append($('<li>').append($('<a>').text('Copy to').addClass('dropdown-item').attr('href', '')));
        dropdown_menu.append($('<li>').append($('<a>').text('Rename').addClass('dropdown-item').attr('href', '')));

        td_dropdown.append(dropdown_menu);
        tr.append(td_dropdown);
    }
});
