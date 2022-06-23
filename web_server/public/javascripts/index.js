const currentUrl = window.location.href;
const currentPath = window.location.pathname;

$(function () {
    $('#upload').attr('action', currentUrl + '/upload');

    // Check if upload file is empty
    $('#fileSubmit').click(() => {
        if ($('#fileInput').val() === '') {
            event.preventDefault();
            alert('File not selected');
            return;
        }
        let fileName = $('#fileInput')[0].files[0].name;
        const result = json.find((e) => {
            return e['name'] == fileName;
        });
        if (result !== undefined) {
            const overwrite = confirm('Do you want to overwrite ' + fileName + '?');
            if (overwrite == false) {
                return;
            } else {
                $.ajax({
                    url: currentUrl + '%2F' + fileName + '/delete',
                    method: 'DELETE',
                })
                    .done(() => {
                        $('#upload').submit();
                    })
                    .fail(() => {
                        alert('Overwrite failed ' + fileName);
                    });
            }
        }
    });

    // Move Button
    let move_flag = sessionStorage.getItem('move_flag');
    $('#move_btn').click(() => {
        let move_src = sessionStorage.getItem('move_src');
        let move_name = sessionStorage.getItem('move_name');
        let move_type = sessionStorage.getItem('move_type');
        const result = json.find((e) => {
            return e['name'] === move_name;
        });
        if (result !== undefined && move_type === 'file') {
            const overwrite = confirm('Do you want to overwrite ' + move_name + '?');
            if (overwrite == false) {
                return;
            }
        } else if (result !== undefined && move_type === 'dir') {
            alert(move_name + ' exists in current directory.');
            return;
        }
        $.ajax({
            url: currentUrl + move_src + '/move',
            method: 'PUT',
        })
            .done(() => {
                alert('Moved ' + move_name);
                sessionStorage.removeItem('move_flag');
                sessionStorage.removeItem('move_src');
                sessionStorage.removeItem('move_name');
                sessionStorage.removeItem('move_type');
                location.reload();
            })
            .fail(() => {
                alert('Move failed ' + move_name);
            });
    });
    if (move_flag === 'true') {
        $('#move_btn').show();
    }

    // Copy Button
    let copy_flag = sessionStorage.getItem('copy_flag');
    $('#copy_btn').click(() => {
        let copy_src = sessionStorage.getItem('copy_src');
        let copy_path = sessionStorage.getItem('copy_path');
        let copy_name = sessionStorage.getItem('copy_name');
        let copy_type = sessionStorage.getItem('copy_type');
        const result = json.find((e) => {
            return e.name === copy_name;
        });
        if (result !== undefined && copy_path === currentPath && copy_type === 'file') {
            const make_dup = confirm('Do you want to make a  duplicate of ' + copy_name + '?');
            if (make_dup == true) {
                $.ajax({
                    url: currentUrl + copy_src + '/dup',
                    method: 'POST',
                })
                    .done(() => {
                        alert('Duplicated ' + copy_name);
                        sessionStorage.removeItem('copy_flag');
                        sessionStorage.removeItem('copy_src');
                        sessionStorage.removeItem('copy_name');
                        location.reload();
                    })
                    .fail(() => {
                        alert('Duplicate failed ' + copy_name);
                    });
                return;
            } else {
                return;
            }
        } else if (result !== undefined && copy_type === 'file') {
            const overwrite = confirm('Do you want to overwrite ' + copy_name + '?');
            if (overwrite == false) {
                return;
            }
        } else if (result !== undefined && copy_type === 'dir') {
            alert(copy_name + ' exists in current directory.');
            return;
        }
        $.ajax({
            url: currentUrl + copy_src + '/copy',
            method: 'POST',
        })
            .done(() => {
                alert('Copied ' + copy_name);
                sessionStorage.removeItem('copy_flag');
                sessionStorage.removeItem('copy_src');
                sessionStorage.removeItem('copy_name');
                location.reload();
            })
            .fail(() => {
                alert('Copy failed ' + copy_name);
            });
    });
    if (copy_flag === 'true') {
        $('#copy_btn').show();
    }

    // Check if new name is empty or exists
    $('#newName').focusout(() => {
        let newName = $('#newName').val() + $('#fileType').text();
        const result = json.find((e) => {
            return e['name'] === newName;
        });
        if ($('#newName').val() == '') {
            $('#newName').addClass('is-invalid');
            $('#newName').removeClass('is-valid');
            if ($('newName').attr('data-type') === 'file') {
                $('#invalidName').text('File name empty!');
            } else {
                $('#invalidName').text('Directory name empty!');
            }
            $('#renameBtn').prop('disabled', true);
        } else if (result !== undefined) {
            $('#newName').addClass('is-invalid');
            $('#newName').removeClass('is-valid');
            if ($('newName').attr('data-type') === 'file') {
                $('#invalidName').text('New file name exists!');
            } else {
                $('#invalidName').text('New directory name exists!');
            }
            $('#renameBtn').prop('disabled', true);
        } else {
            $('#newName').removeClass('is-invalid');
            $('#newName').addClass('is-valid');
            $('#renameBtn').prop('disabled', false);
        }
    });

    // Rename button
    $('#renameBtn').click(() => {
        let src_file = $('#modalTitle').text();
        let newName = $('#newName').val() + $('#fileType').text();
        src_file = src_file.replace('Rename ', '');

        if (sessionStorage.getItem('copy_flag') == 'true') {
            sessionStorage.removeItem('copy_flag');
            sessionStorage.removeItem('copy_src');
            sessionStorage.removeItem('copy_name');
            $('#copy_btn').hide();
        }
        if (sessionStorage.getItem('move_flag') == 'true') {
            sessionStorage.removeItem('move_flag');
            sessionStorage.removeItem('move_src');
            sessionStorage.removeItem('move_name');
            $('#move_btn').hide();
        }
        $.ajax({
            url: currentUrl + '%2F' + src_file + '/' + newName + '/rename',
            method: 'PUT',
        })
            .done(() => {
                alert('Renanmed ' + src_file);
                location.reload();
            })
            .fail(() => {
                alert(src_file + ' rename failed ');
            });
    });

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
            url = url + dirs[i];
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
        <a class="btn dropdown-toggle rounded-0" href="#" role="button" id="dropdownMenuLink" data-bs-toggle="dropdown" aria-expanded="false">
            <img src="/images/three-dots-vertical.svg" alt="dropdown"/>
        </a>`);
        let dropdown_menu = $(`<ul class="dropdown-menu" aria-labelledby="dropdownMenuButton1"></ul>`);

        let li_move = $('<li>').append($('<button>').text('Move to').addClass('dropdown-item').attr('type', 'button'));
        let li_delete = $('<li>').append($('<button>').text('Delete').addClass('dropdown-item').attr('type', 'button'));
        let li_copy = $('<li>').append($('<button>').text('Copy to').addClass('dropdown-item').attr('type', 'button'));
        let li_rename = $('<li>').append(
            $('<button>').text('Rename').addClass('dropdown-item').attr('type', 'button').attr('data-bs-toggle', 'modal').attr('data-bs-target', '#renameModal')
        );
        const encoded = encodeURIComponent('/' + json[i]['name']);
        let download_url = currentUrl + encoded + '/download';
        let delete_url = currentUrl + encoded + '/delete';

        // Delete button
        li_delete.click(() => {
            const delete_confirm = confirm('Do you want to delete ' + json[i]['name'] + '?');
            if (delete_confirm == false) {
                return;
            } else {
                $.ajax({
                    url: delete_url,
                    method: 'DELETE',
                }).done(() => {
                    alert('Deleted ' + json[i]['name']);
                    location.reload();
                });
            }
        });

        // Move button
        li_move.click(() => {
            const toast = new bootstrap.Toast($('#toast'));
            $('.toast-body').text('Move ' + json[i]['name']);
            toast.show();
            $('#move_btn').show('fast');
            sessionStorage.setItem('move_flag', true);
            sessionStorage.setItem('move_src', currentPath + '%2F' + decodeURI(json[i]['name']));
            sessionStorage.setItem('move_name', json[i]['name']);
            if (json[i]['is_dir'] === '0') {
                sessionStorage.setItem('move_type', 'file');
            } else {
                sessionStorage.setItem('move_type', 'dir');
            }
            if (sessionStorage.getItem('copy_flag') == 'true') {
                sessionStorage.removeItem('copy_flag');
                sessionStorage.removeItem('copy_src');
                sessionStorage.removeItem('copy_name');
                sessionStorage.removeItem('copy_type');
                $('#copy_btn').hide();
            }
        });

        // Copy button
        li_copy.click(() => {
            const toast = new bootstrap.Toast($('#toast'));
            $('.toast-body').text('Copy ' + json[i]['name']);
            toast.show();
            $('#copy_btn').show('fast');
            sessionStorage.setItem('copy_flag', true);
            sessionStorage.setItem('copy_src', currentPath + '%2F' + decodeURI(json[i]['name']));
            sessionStorage.setItem('copy_path', currentPath);
            sessionStorage.setItem('copy_name', json[i]['name']);
            if (json[i]['is_dir'] === '0') {
                sessionStorage.setItem('copy_type', 'file');
            } else {
                sessionStorage.setItem('copy_type', 'dir');
            }
            if (sessionStorage.getItem('move_flag') == 'true') {
                sessionStorage.removeItem('move_flag');
                sessionStorage.removeItem('move_src');
                sessionStorage.removeItem('move_name');
                sessionStorage.removeItem('move_type');
                $('#move_btn').hide();
            }
        });

        if (json[i]['is_dir'] === '0') {
            // Rename file
            li_rename.click(() => {
                $('#modalTitle').text('Rename ' + json[i]['name']);
                if (json[i]['name'].lastIndexOf('.') !== 0 && json[i]['name'].lastIndexOf('.') !== -1) {
                    $('#fileType').text(json[i]['name'].substring(json[i]['name'].lastIndexOf('.'), json[i]['name'].length));
                }
                $('#newName').attr('placeholder', json[i]['name'].substring(0, json[i]['name'].lastIndexOf('.')));
                $('#newName').attr('data-type', 'file');
                $('#renameBtn').prop('disabled', true);
                $('#newName').val('');
                $('#newName').removeClass('is-invalid');
                $('#newName').removeClass('is-valid');
            });

            // Download a file
            dropdown_menu.append($('<li>').append($('<a>').text('Download').addClass('dropdown-item').attr('href', download_url)));

            dropdown_menu.append(li_delete);
            dropdown_menu.append(li_move);
            dropdown_menu.append(li_copy);
            dropdown_menu.append(li_rename);
        } else {
            // Rename dir
            li_rename.click(() => {
                $('#modalTitle').text('Rename ' + json[i]['name']);
                $('#newName').attr('placeholder', json[i]['name'].substring(0, json[i]['name'].lastIndexOf('.')));
                $('#newName').attr('data-type', 'dir');
                $('#renameBtn').prop('disabled', true);
                $('#newName').val('');
                $('#newName').removeClass('is-invalid');
                $('#newName').removeClass('is-valid');
            });
            dropdown_menu.append(li_delete);
            dropdown_menu.append(li_move);
            dropdown_menu.append(li_copy);
            dropdown_menu.append(li_rename);
        }

        td_dropdown.append(dropdown_menu);
        tr.append(td_dropdown);
    }
});
