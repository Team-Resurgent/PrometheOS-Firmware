let nonce = 0;
window.onload = function () {
    querydrive();
}
async function querydrive() {
    nonce = (Math.random() * 4294967295) - 2147483647;
    let content = document.getElementById("content");
    await fetch(`http://192.168.1.66/api/drive.json?0&${nonce}`).catch(error => {
        content.innerHTML = "Failed to connect.";
    }).then(async response => {
        const json = await response.json();
        const lockStatus = json.locked ? "Locked" : "Unlocked";
        const buttonLabel = json.locked ? "Unlock" : "Lock";
        let contentBody = `<label for="model">Model:</span><div id="model">${json.model}</div><br /><label for="serial"><b>Serial:</b></label><div id="serial">${json.serial}</div><br /><label for="status"><b>Status:</b></label><div id="status">${lockStatus}</div><br /><hr /><div class="grid"><a id="locktoggle" role="button" href="javascript:void(0)" onclick="setLocked(${!json.locked})">${buttonLabel}</a></div>`;
        content.innerHTML = contentBody;
    });
}
async function setLocked(toStatus) {
    const button = document.getElementById("locktoggle")
    button.setAttribute("disabled", true);
    button.setAttribute("onclick", "javascript:void(0)");
    if (toStatus) {
        button.innerHTML = "Locking...";
    } else {
        button.innerHTML = "Unlocking...";
    }
    await fetch(`http://192.168.1.66/api/hddlockunlock?${toStatus ? 1 : 0}&${nonce}`).then(async response => {
        const json = await response.json();
        switch (json.status) {
            case 0: button.innerHTML = "No result"; break;
            case 1: button.innerHTML = "Unlocked"; break;
            case 2: button.innerHTML = "VSC Unlocked"; break;
            case 3: button.innerHTML = "Failed to unlock HDD"; break;
            case 4: button.innerHTML = "Already unlocked"; break;
            case 5: button.innerHTML = "Locked"; break;
            case 6: button.innerHTML = "Failed to lock HDD"; break;
            case 7: button.innerHTML = "Already locked"; break;
            case 8: button.innerHTML = "Locking not supported"; break;
            case 9: button.innerHTML = "Unable to decrypt EEPROM"; break;
            case 10: button.innerHTML = "Unlocking..."; break;
            case 11: button.innerHTML = "VSC Unlocking..."; break;
            case 12: button.innerHTML = "Locking..."; break;
        }
        setTimeout(() => {
            querydrive();
        }, 2000);
    }).catch(() => {
        button.innerHTML = "Failed";
    })
}