var keys = Object.keys(json[0]);
var currentUrl = window.location.href;

// for upload UI, dynamically make UI
var uploadForm = document.createElement("form");
uploadForm.setAttribute("method",  "post");
uploadForm.setAttribute("action", currentUrl + "/upload");
uploadForm.setAttribute("enctype", "multipart/form-data");
uploadForm.setAttribute("style", "margin: 2% 5%;"); 

var uploadGetFile = document.createElement("input");
var uploadButton = document.createElement("input");
uploadGetFile.setAttribute("type", "file");
uploadGetFile.setAttribute("name", "fileUploaded");

uploadButton.setAttribute("type", "image");
uploadButton.setAttribute("src", "/images/cloud-upload.svg");
uploadForm.appendChild(uploadGetFile);
uploadForm.appendChild(uploadButton);

$(function () {

    document.body.append(uploadForm);
    var $thead = $('<thead>').appendTo('.table');
    var $tr = $('<tr>').appendTo($thead);
    $tr.addClass('table-secondary text-capitalize fs-5 fw-bold');
    $tr.append($('<td>').addClass('icon'));
    for (let i = 0; i < keys.length; i++) {
        if (keys[i] === 'type') {
            continue;
        } else if (keys[i] === 'is_dir') {
            continue;
        }
        let td = $('<td>');
        td.text(keys[i]);
        td.addClass(keys[i]);
        $tr.append(td);

    }
    $tr.append($('<td>').addClass('download'));
    for (let i = 0; i < json.length; i++) {
        var $tr = $('<tr>').appendTo('.table');
        if (json[i]['is_dir'] === '1') {
            let td = $('<td>');
            let ico = $('<img>');
            td.addClass('icon');
            ico.attr('src', '/images/folder.svg');
            ico.attr('alt', 'folder');
            td.append(ico);
            $tr.append(td);
        } else {
            let td = $('<td>');
            let ico = $('<img>');
            td.addClass('icon');
            ico.attr('src', '/images/file-earmark.svg');
            ico.attr('alt', 'file');
            td.append(ico);
            $tr.append(td);
        }
        for (let j = 0; j < keys.length; j++) {
            if (keys[j] === 'type') {
                continue;
            } else if (keys[j] === 'is_dir') {
                if (json[i][keys[j]] === '1') {
                    let $td = $('<td>');
                    $td.addClass('name');
                    const encoded = encodeURIComponent('/' + json[i]['name']);
                    $td.append($(`<a href="${currentUrl}${encoded}"></a>`).text(json[i]['name']));
                    $tr.append($td);
                } else {
                    $tr.append($('<td>').text(json[i]['name']).addClass('name'));
                }
                continue;
            } else if (keys[j] === 'name') {
                continue;
            }
            $tr.append($('<td>').text(json[i][keys[j]]).addClass(keys[j]));
        }
        if (json[i]['is_dir'] === '0') {
            let td = $('<td>');
            let a = $('<a>');
            let ico = $('<img>');
            const encoded = encodeURIComponent('/' + json[i]['name']);
            let url = currentUrl + encoded + '/download';
            td.addClass('download');
            a.attr('href', url);
            ico.attr('src', '/images/arrow-down-square.svg');
            ico.attr('alt', 'file');
            a.append(ico);
            td.append(a);
            $tr.append(td);
        } else {
            $tr.append($('<td>').addClass('download'));
        }
    }

});
