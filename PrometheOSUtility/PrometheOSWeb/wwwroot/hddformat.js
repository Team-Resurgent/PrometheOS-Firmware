let nonce = 0;
window.onload = function () {
    init();
}
function init() {
    nonce = (Math.random() * 4294967295) - 2147483647;
    const queryparams = new URLSearchParams(location.search);
    const drive = parseInt(queryparams.get("drive"));
    if ([0, 1].indexOf(drive) === -1) {
        let content = document.getElementById("content");
        content.innerHTML = "<span>Unknown drive</span>";
        return;
    }
    querydrive(drive);
}
async function querydrive(drive) {
    let content = document.getElementById("content");
    await fetch(`http://192.168.1.66/api/drive.json?${drive}&${nonce}`).catch(error => {
        content.innerHTML = "Failed to connect.";
    }).then(async response => {
        const json = await response.json();
        let driveName = document.getElementById("drivename");
        switch (drive) {
            case 0: driveName.innerHTML = "Primary"; break;
            case 1: driveName.innerHTML = "Secondary"; break;
        }
        let contentBody = `<label for="model">Model:</span><div id="model">${json.model}</div><br /><label for="serial"><b>Serial:</b></label><div id="serial">${json.serial}</div><br /><hr /><input type="hidden" name="drive" value="${drive}"><input type="hidden" name="nonce" value="${nonce}"><div class="grid"><a id="confirm" role="button" href="javascript:void(0)" onclick="veryConfirm()">Please Confirm...</a></div>`;
        content.innerHTML = contentBody;
    });
}
async function veryConfirm() {
    const veryConfirm = confirm("Confirm again");
    if (!veryConfirm) {
        return;
    }
    const form = document.getElementById("formatform");
    const confirmButton = document.getElementById("confirm");
    confirmButton.innerHTML = "Please wait...";
    confirmButton.setAttribute("disabled", true);
    confirmButton.setAttribute("onclick", "javascript:void(0)");
    const data = new FormData(form);
    await fetch("http://192.168.1.66/api/formatdrive", {
        method: 'POST',
        body: data
    }).then(async response => {
        if (response.status == 200) {
            const json = await response.json();
            switch (json.result) {
                case 0: confirmButton.innerHTML = "Unknown"; break;
                case 1: confirmButton.innerHTML = "Done"; break;
                case 2: confirmButton.innerHTML = "Failed"; break;
            }
        } else {
            confirmButton.innerHTML = "Failed";
        }
        setTimeout(() => {
            init();
        }, 2000);

    }).catch(() => {
        confirmButton.innerHTML = "Failed";
    });
}