const REMOTE_VIEW_READY_POLL_MS = 50;
const REMOTE_VIEW_INTERVAL_KEY = "prometheRemoteViewIntervalMs";

function setScreenshotStatus(message) {
    const img = document.getElementById("screenshotImage");
    if (img != null) {
        return;
    }

    const status = document.getElementById("screenshotStatus");
    if (status == null) {
        return;
    }

    status.textContent = message;
    status.hidden = false;
}

function showScreenshotImage(blob) {
    const view = document.getElementById("screenshotView");
    const status = document.getElementById("screenshotStatus");
    if (view == null) {
        return;
    }

    const newUrl = window.URL.createObjectURL(blob);
    let img = document.getElementById("screenshotImage");

    if (img == null) {
        if (status != null) {
            status.hidden = true;
        }

        img = document.createElement("img");
        img.id = "screenshotImage";
        img.alt = "Remote view";
        img.style = "width: 100%; height: auto; display: block";
        img.src = newUrl;
        view.appendChild(img);
        return;
    }

    const oldUrl = img.src;
    img.onload = function () {
        if (oldUrl.startsWith("blob:")) {
            window.URL.revokeObjectURL(oldUrl);
        }
        img.onload = null;
    };
    img.src = newUrl;
}

function isRefreshBusy() {
    const refreshButton = document.getElementById("refreshButton");
    return refreshButton != null && refreshButton.getAttribute("aria-busy") === "true";
}

const GAMEPAD_TRIGGER_THRESHOLD = 0.5;

const GAMEPAD_BINDINGS = [
    { type: "button", index: 0 },
    { type: "button", index: 1 },
    { type: "button", index: 2 },
    { type: "button", index: 3 },
    { type: "button", index: 4 },
    { type: "button", index: 5 },
    { type: "button", index: 8 },
    { type: "button", index: 9 },
    { type: "button", index: 10 },
    { type: "button", index: 11 },
    { type: "button", index: 12 },
    { type: "button", index: 15 },
    { type: "button", index: 13 },
    { type: "button", index: 14 },
    { type: "trigger", buttonIndex: 6, axisIndex: 2 },
    { type: "trigger", buttonIndex: 7, axisIndex: 5 }
];

const gamepadPreviousKeys = new Array(GAMEPAD_BINDINGS.length);
let autoRefreshTimer = null;

function sendControllerKey(keyId) {
    Promethe.api("controller?" + keyId);

    const autoRefresh = document.getElementById("autoRefresh");
    if (autoRefresh != null && autoRefresh.checked) {
        refreshRemoteView();
        scheduleAutoRefresh();
    }

    const onScreenButton = document.querySelector(".controller-section a[data-key='" + keyId + "']");
    if (onScreenButton == null) {
        return;
    }

    onScreenButton.classList.add("pressed");
    window.setTimeout(function () {
        onScreenButton.classList.remove("pressed");
    }, 120);
}

function isGamepadKeyPressed(gamepad, keyId) {
    const binding = GAMEPAD_BINDINGS[keyId];
    if (binding == null) {
        return false;
    }

    if (binding.type === "button") {
        const button = gamepad.buttons[binding.index];
        return button != null && button.pressed === true;
    }

    if (binding.type === "trigger") {
        const triggerButton = gamepad.buttons[binding.buttonIndex];
        if (triggerButton != null && triggerButton.pressed === true) {
            return true;
        }

        const axisValue = gamepad.axes[binding.axisIndex];
        return axisValue != null && axisValue > GAMEPAD_TRIGGER_THRESHOLD;
    }

    return false;
}

function getActiveGamepad() {
    if (navigator.getGamepads == null) {
        return null;
    }

    const gamepads = navigator.getGamepads();
    for (let i = 0; i < gamepads.length; i++) {
        if (gamepads[i] != null) {
            return gamepads[i];
        }
    }

    return null;
}

function updateGamepadStatus(gamepad) {
    const status = document.getElementById("gamepadStatus");
    if (status == null) {
        return;
    }

    status.textContent = gamepad != null ? "(" + gamepad.id + ")" : "";
}

function pollGamepad() {
    const gamepad = getActiveGamepad();
    updateGamepadStatus(gamepad);

    if (gamepad != null && document.hidden === false) {
        for (let keyId = 0; keyId < GAMEPAD_BINDINGS.length; keyId++) {
            const pressed = isGamepadKeyPressed(gamepad, keyId);
            const wasPressed = gamepadPreviousKeys[keyId] === true;
            if (pressed && wasPressed === false) {
                sendControllerKey(keyId);
            }
            gamepadPreviousKeys[keyId] = pressed;
        }
    }

    window.requestAnimationFrame(pollGamepad);
}

function initGamepadSupport() {
    if (navigator.getGamepads == null) {
        return;
    }

    window.addEventListener("gamepadconnected", function () {
        updateGamepadStatus(getActiveGamepad());
    });

    window.addEventListener("gamepaddisconnected", function () {
        updateGamepadStatus(getActiveGamepad());
        for (let keyId = 0; keyId < gamepadPreviousKeys.length; keyId++) {
            gamepadPreviousKeys[keyId] = false;
        }
    });

    updateGamepadStatus(getActiveGamepad());
    window.requestAnimationFrame(pollGamepad);
}

function releaseControllerButton(button) {
    button.classList.remove("pressed");
    button.blur();
}

function bindControllerButtons() {
    document.querySelectorAll(".controller-section a[role='button'][data-key]").forEach(function (button) {
        button.addEventListener("pointerdown", function (event) {
            button.classList.add("pressed");
            if (button.setPointerCapture) {
                button.setPointerCapture(event.pointerId);
            }
        });

        button.addEventListener("pointerup", function () {
            releaseControllerButton(button);
        });

        button.addEventListener("pointercancel", function () {
            releaseControllerButton(button);
        });

        button.addEventListener("lostpointercapture", function () {
            releaseControllerButton(button);
        });

        button.addEventListener("click", function (event) {
            event.preventDefault();
            releaseControllerButton(button);
            sendControllerKey(parseInt(button.getAttribute("data-key"), 10));
        });
    });
}

function setRefreshBusy(isBusy) {
    const refreshButton = document.getElementById("refreshButton");
    if (refreshButton != null) {
        refreshButton.setAttribute("aria-busy", isBusy ? "true" : "false");
        refreshButton.setAttribute("aria-disabled", isBusy ? "true" : "false");
        if (isBusy) {
            refreshButton.blur();
        }
    }
}

function getRefreshIntervalMs() {
    const select = document.getElementById("refreshInterval");
    if (select == null) {
        return 1000;
    }
    return parseInt(select.value, 10) || 1000;
}

function saveRefreshInterval() {
    window.localStorage.setItem(REMOTE_VIEW_INTERVAL_KEY, String(getRefreshIntervalMs()));
}

function restoreRefreshInterval() {
    const saved = window.localStorage.getItem(REMOTE_VIEW_INTERVAL_KEY);
    const select = document.getElementById("refreshInterval");
    if (select == null) {
        return;
    }

    let matched = false;
    if (saved != null) {
        for (let i = 0; i < select.options.length; i++) {
            if (select.options[i].value === saved) {
                select.selectedIndex = i;
                matched = true;
                break;
            }
        }
    }

    if (matched === false) {
        select.selectedIndex = 0;
        saveRefreshInterval();
    }
}

function clearAutoRefreshTimer() {
    if (autoRefreshTimer != null) {
        window.clearInterval(autoRefreshTimer);
        autoRefreshTimer = null;
    }
}

function scheduleAutoRefresh() {
    clearAutoRefreshTimer();

    const autoRefresh = document.getElementById("autoRefresh");
    if (autoRefresh == null || autoRefresh.checked === false) {
        return;
    }

    autoRefreshTimer = window.setInterval(function () {
        if (document.getElementById("autoRefresh").checked) {
            refreshRemoteView();
        }
    }, getRefreshIntervalMs());
}

function bindRefreshControls() {
    restoreRefreshInterval();

    const autoRefresh = document.getElementById("autoRefresh");
    const refreshInterval = document.getElementById("refreshInterval");
    if (autoRefresh != null) {
        autoRefresh.addEventListener("change", scheduleAutoRefresh);
    }
    if (refreshInterval != null) {
        refreshInterval.addEventListener("change", function () {
            saveRefreshInterval();
            scheduleAutoRefresh();
        });
    }

    scheduleAutoRefresh();
}

window.onload = function () {
    document.getElementById("year").textContent = new Date().getFullYear();
    bindControllerButtons();
    initGamepadSupport();
    bindRefreshControls();
    refreshRemoteView();
};

async function waitForRemoteViewReady() {
    while (true) {
        const response = await Promethe.api("remoteviewready.json");
        if (!response.ok) {
            throw new Error("Failed to check remote view status.");
        }
        const data = await response.json();
        if (data.ready) {
            return;
        }
        await new Promise(function (resolve) {
            window.setTimeout(resolve, REMOTE_VIEW_READY_POLL_MS);
        });
    }
}

async function refreshRemoteView() {
    const view = document.getElementById("screenshotView");
    if (view == null || isRefreshBusy()) {
        return;
    }

    try {
        setRefreshBusy(true);
        setScreenshotStatus("Requesting remote view...");
        await Promethe.api("takeremoteview");

        setScreenshotStatus("Waiting for remote view...");
        await waitForRemoteViewReady();

        setScreenshotStatus("Downloading remote view...");
        const response = await Promethe.api("downloadremoteview.jpg");
        if (!response.ok) {
            throw new Error("Failed to download remote view.");
        }

        const blob = await response.blob();
        showScreenshotImage(blob);
    } catch (error) {
        if (document.getElementById("screenshotImage") == null) {
            setScreenshotStatus("Failed to connect.");
        }
    } finally {
        setRefreshBusy(false);
    }
}

function screenshot() {
    refreshRemoteView();
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

function setScreenshotButtonBusy(isBusy) {
    const screenshotButton = document.getElementById("screenshotButton");
    if (screenshotButton != null) {
        screenshotButton.setAttribute("aria-busy", isBusy ? "true" : "false");
        screenshotButton.setAttribute("aria-disabled", isBusy ? "true" : "false");
        if (isBusy) {
            screenshotButton.blur();
        }
    }
}

async function takeScreenshot() {
    const screenshotButton = document.getElementById("screenshotButton");
    if (screenshotButton != null && screenshotButton.getAttribute("aria-busy") === "true") {
        return;
    }

    try {
        setScreenshotButtonBusy(true);
        Promethe.toast("Capturing screenshot...");
        await Promethe.api("takescreenshot");
        await waitForScreenshotReady();
        const response = await Promethe.api("downloadscreenshot");
        if (!response.ok) {
            throw new Error("Failed to download screenshot.");
        }
        const blob = await response.blob();
        const url = window.URL.createObjectURL(blob);
        const anchor = document.createElement("a");
        anchor.href = url;
        anchor.download = "screenshot.png";
        document.body.appendChild(anchor);
        anchor.click();
        anchor.remove();
        window.URL.revokeObjectURL(url);
        Promethe.toast("Screenshot saved.", "success");
    } catch (error) {
        Promethe.toast("Screenshot failed.", "error");
    } finally {
        setScreenshotButtonBusy(false);
    }
}
