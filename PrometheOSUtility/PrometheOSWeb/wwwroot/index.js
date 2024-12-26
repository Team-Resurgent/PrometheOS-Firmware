async function screenshot() {
    await fetch("http://192.168.1.66/api/screenshot").catch(error => {
        content.innerHTML = "Failed to connect."
    }).then(response => response.blob()).then(blob => {
        var url = window.URL.createObjectURL(blob);
        var a = document.createElement('a');
        a.href = url;
        a.download = "screenshot.png";
        document.body.appendChild(a);
        a.click();
        a.remove();
    });
}
async function reboot() {
    await fetch("http://192.168.1.66/api/reboot");
}
async function shutdown() {
    await fetch("http://192.168.1.66/api/shutdown");
}