async function reboot() {
    await fetch("http://192.168.1.151/api/reboot");
}
async function shutdown() {
    await fetch("http://192.168.1.151/api/shutdown");
}