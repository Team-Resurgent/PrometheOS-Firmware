let statusTimer = null;

window.onload = function () {
    document.getElementById("year").textContent = new Date().getFullYear();
    refreshStatus();
    statusTimer = window.setInterval(refreshStatus, 5000);
};

async function refreshStatus() {
    try {
        const response = await Promethe.api("status.json");
        if (!response.ok) {
            throw new Error("status failed");
        }

        const data = await response.json();
        document.getElementById("statVersion").textContent = data.version || "--";
        document.getElementById("statModchip").textContent = data.modchip || "--";
        document.getElementById("statSkin").textContent = data.skin || "--";
        document.getElementById("statCpuTemp").textContent = (data.cpuTemp != null ? data.cpuTemp + " C" : "--");
        document.getElementById("statMbTemp").textContent = (data.mbTemp != null ? data.mbTemp + " C" : "--");
        document.getElementById("statFan").textContent = (data.fanSpeed != null ? data.fanSpeed + "%" : "--");
        document.getElementById("statMem").textContent = (data.freeMemKb != null ? Math.round(data.freeMemKb / 1024) + " MB" : "--");
        document.getElementById("statSpeed").textContent = (data.cpuMhz != null && data.gpuMhz != null)
            ? data.cpuMhz + " / " + data.gpuMhz + " MHz"
            : "--";
        document.getElementById("statAutoBoot").textContent = data.autoBoot || "None";
        document.getElementById("statAvRes").textContent = data.avRes || "--";
    } catch (error) {
        document.getElementById("statVersion").textContent = "Offline";
    }
}

async function waitForScreenshotReady() {
    while (true) {
        const response = await Promethe.api("screenshotready.json");
        if (!response.ok) {
            throw new Error("Failed to check screenshot status.");
        }
        const data = await response.json();
        if (data.ready) {
            return;
        }
        await new Promise(function (resolve) {
            window.setTimeout(resolve, 100);
        });
    }
}

async function takeScreenshot() {
    try {
        Promethe.toast("Capturing screenshot...");
        await Promethe.api("takescreenshot");
        await waitForScreenshotReady();
        const response = await Promethe.api("downloadscreenshot");
        const blob = await response.blob();
        const url = window.URL.createObjectURL(blob);
        const anchor = document.createElement("a");
        anchor.href = url;
        anchor.download = "screenshot.png";
        document.body.appendChild(anchor);
        anchor.click();
        anchor.remove();
        Promethe.toast("Screenshot saved.", "success");
    } catch (error) {
        Promethe.toast("Screenshot failed.", "error");
    }
}

async function rebootConsole() {
    try {
        await Promethe.rebootAndWait();
        refreshStatus();
    } catch (error) {
        Promethe.toast("Reboot timed out.", "error");
    }
}

async function shutdownConsole() {
    try {
        await Promethe.shutdownConsole();
    } catch (error) {
        Promethe.toast("Shutdown failed.", "error");
    }
}
