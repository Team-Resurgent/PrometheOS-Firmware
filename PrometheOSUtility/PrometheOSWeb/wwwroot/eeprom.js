window.onload = function () {
    document.getElementById("year").textContent = new Date().getFullYear();
};

async function restoreEeprom() {
    const input = document.getElementById("eepromFile");
    if (!input.files || input.files.length === 0) {
        Promethe.toast("Choose an EEPROM file first.", "error");
        return;
    }

    const file = input.files[0];
    if (file.size < 256) {
        Promethe.toast("EEPROM image must be at least 256 bytes.", "error");
        return;
    }

    const formData = new FormData();
    formData.append("file", file);

    try {
        Promethe.toast("Uploading EEPROM...");
        const response = await fetch("/api/restoreeeprom", {
            method: "POST",
            body: formData
        });
        if (!response.ok) {
            throw new Error("restore failed");
        }

        Promethe.toast("EEPROM restored. Waiting for reboot...");
        await Promethe.waitForOnline(180000);
        Promethe.toast("Console is back online.", "success");
        window.location.href = "index.html";
    } catch (error) {
        Promethe.toast("EEPROM restore failed.", "error");
    }
}
