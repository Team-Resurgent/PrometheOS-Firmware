window.onload = function () {
    listdrives();
}
async function listdrives() {
    await fetch("http://192.168.1.66/api/drives.json").catch(error => {
        let content = document.getElementById("content");
        content.innerHTML = "Failed to connect.";
    }).then(async response => {
        const json = await response.json();
        let contentBody = "";
        if (json.primaryDrive) {
            contentBody += `<div class="grid"><a role="button" href="hddformat.html?drive=0">Format Drive Primary</a></div><br />`;
        }
        if (json.secondaryDrive) {
            contentBody += `<div class="grid"><a role="button" href="hddformat.html?drive=1">Format Drive Secondary</a></div><br />`;
        }
        let content = document.getElementById("content");
        content.outerHTML = contentBody;
    });
}