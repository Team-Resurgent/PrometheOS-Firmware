window.onload = function () {
    document.getElementById("year").textContent = new Date().getFullYear();
};

async function waitForScreenshotReady() {
    while (true) {
        const response = await fetch("/api/screenshotready.json");
        if (!response.ok) {
            throw new Error("Failed to check screenshot status.");
        }
        const data = await response.json();
        if (data.ready) {
            return;
        }
        await new Promise(resolve => setTimeout(resolve, 100));
    }
}

async function screenshot() {
    try {
        await fetch("/api/takescreenshot");
        await waitForScreenshotReady();
        await fetch("/api/downloadscreenshot").then(response => response.blob()).then(blob => {
            var url = window.URL.createObjectURL(blob);
            var a = document.createElement('a');
            a.href = url;
            a.download = "screenshot.png";
            document.body.appendChild(a);
            a.click();
            a.remove();
        });
    } catch (error) {
        content.innerHTML = "Failed to connect."
    }
}
async function reboot() {
    await fetch("/api/reboot");
}
async function shutdown() {
    await fetch("/api/shutdown");
}
