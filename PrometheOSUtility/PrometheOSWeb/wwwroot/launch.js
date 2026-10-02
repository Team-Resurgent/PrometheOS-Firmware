window.onload = function () {
    document.getElementById("year").textContent = new Date().getFullYear();
    getBankInfo();
};

function formatBankLabel(bank) {
    if (bank.autoBoot) {
        return "* " + bank.name + " *";
    }
    return bank.name;
}

function buildBankRow(launchOnClick, safeLaunchOnClick, bank) {
    const label = formatBankLabel(bank);
    return "<p><div class=\"grid\">" +
        "<a role=\"button\" href=\"javascript:void(0)\" onclick=\"" + launchOnClick + "\">" + label + "</a>" +
        "<a role=\"button\" class=\"secondary outline\" href=\"javascript:void(0)\" onclick=\"" + safeLaunchOnClick + "\">Safe Mode</a>" +
        "<a role=\"button\" class=\"secondary\" href=\"javascript:void(0)\" onclick=\"toggleAutoBoot(" + bank.id + "," + bank.category + ")\">Toggle Auto Boot</a>" +
        "</div></p>";
}

async function getBankInfo() {
    await Promethe.api("bankinfo.json").then(async function (response) {
        const json = await response.json();
        let contentBody = "";
        for (let i = 0; i < json.length; i++) {
            if (json[i].slots > 0) {
                if (json[i].category == 0) {
                    contentBody += buildBankRow("launchBank(" + json[i].id + ")", "launchBank(" + json[i].id + ", true)", json[i]);
                } else if (json[i].category == 1) {
                    contentBody += buildBankRow("launchTsop()", "launchTsop(true)", json[i]);
                } else if (json[i].category == 2) {
                    contentBody += buildBankRow("launchRecovery()", "launchRecovery(true)", json[i]);
                } else if (json[i].category == 3) {
                    contentBody += buildBankRow("launchSdCard()", "launchSdCard(true)", json[i]);
                }
            }
        }

        document.getElementById("content").innerHTML = contentBody;
    }).catch(function () {
        document.getElementById("content").innerHTML = "Failed to connect.";
    });
}

async function toggleAutoBoot(id, category) {
    await Promethe.api("toggleautoboot?" + id + "," + category).then(function (response) {
        if (response.status == 200) {
            getBankInfo();
            Promethe.toast("Auto boot updated.", "success");
        } else {
            Promethe.toast("Failed to toggle auto boot.", "error");
        }
    }).catch(function () {
        Promethe.toast("Failed to connect.", "error");
    });
}

async function launchBank(id, safeMode) {
    const query = safeMode ? id + ",1" : String(id);
    await Promethe.api("launchbank?" + query).then(function (response) {
        if (response.status == 200) {
            document.getElementById("content").innerHTML = safeMode ? "Launching in safe mode..." : "Launching...";
            Promethe.toast("Launch command sent.", "success");
        } else {
            Promethe.toast("Failed to launch.", "error");
        }
    }).catch(function () {
        Promethe.toast("Failed to connect.", "error");
    });
}

async function launchTsop(safeMode) {
    const query = safeMode ? "1" : "";
    await Promethe.api("launchtsop" + (query ? "?" + query : "")).then(function (response) {
        if (response.status == 200) {
            document.getElementById("content").innerHTML = safeMode ? "Launching TSOP in safe mode..." : "Launching TSOP...";
            Promethe.toast("Launch command sent.", "success");
        } else {
            Promethe.toast("Failed to launch.", "error");
        }
    }).catch(function () {
        Promethe.toast("Failed to connect.", "error");
    });
}

async function launchRecovery(safeMode) {
    const query = safeMode ? "1" : "";
    await Promethe.api("launchrecovery" + (query ? "?" + query : "")).then(function (response) {
        if (response.status == 200) {
            document.getElementById("content").innerHTML = safeMode ? "Launching recovery in safe mode..." : "Launching recovery...";
            Promethe.toast("Launch command sent.", "success");
        } else {
            Promethe.toast("Failed to launch.", "error");
        }
    }).catch(function () {
        Promethe.toast("Failed to connect.", "error");
    });
}

async function launchSdCard(safeMode) {
    const query = safeMode ? "1" : "";
    await Promethe.api("launchsdcard" + (query ? "?" + query : "")).then(function (response) {
        if (response.status == 200) {
            document.getElementById("content").innerHTML = safeMode ? "Launching sdcard in safe mode..." : "Launching sdcard...";
            Promethe.toast("Launch command sent.", "success");
        } else {
            Promethe.toast("Failed to launch.", "error");
        }
    }).catch(function () {
        Promethe.toast("Failed to connect.", "error");
    });
}
