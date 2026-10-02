window.onload = function () {
    document.getElementById("copyYear").textContent = new Date().getFullYear();
    getDateTime();
    updateBrowserTime();
    window.setInterval(updateBrowserTime, 1000);
};

function pad2(n) {
    return (n < 10 ? "0" : "") + n;
}

function updateBrowserTime() {
    document.getElementById("browserTime").textContent = new Date().toLocaleString();
}

// Fill the date/time inputs from this browser's clock (the user then reviews and hits Save).
function useBrowserTime() {
    const dateEl = document.getElementById("dtDate");
    const timeEl = document.getElementById("dtTime");
    if (!dateEl || !timeEl) {
        return;
    }
    const now = new Date();
    dateEl.value = now.getFullYear() + "-" + pad2(now.getMonth() + 1) + "-" + pad2(now.getDate());
    timeEl.value = pad2(now.getHours()) + ":" + pad2(now.getMinutes()) + ":" + pad2(now.getSeconds());
}

async function getDateTime() {
    await fetch("/api/datetime.json").then(async response => {

        const text = await response.text();
        const data = JSON.parse(text);

        // When the X-RTC is present a save writes both it and the system clock; otherwise just the clock.
        document.getElementById("updateTarget").textContent =
            data.target === "xrtc" ? "System Clock & X-RTC" : "System Clock";

        const dateVal = data.year + "-" + pad2(data.month) + "-" + pad2(data.day);
        const timeVal = pad2(data.hour) + ":" + pad2(data.min) + ":" + pad2(data.sec);

        const content = document.getElementById("content");
        content.innerHTML =
            "<label>Date<input type=\"date\" id=\"dtDate\" value=\"" + dateVal + "\" /></label>" +
            "<label>Time<input type=\"time\" id=\"dtTime\" step=\"1\" value=\"" + timeVal + "\" /></label>";

    }).catch(() => {

        document.getElementById("content").innerHTML = "Failed to connect.";

    });
}

async function setDateTime() {
    const dateEl = document.getElementById("dtDate");
    const timeEl = document.getElementById("dtTime");
    if (!dateEl || !timeEl || dateEl.value === "" || timeEl.value === "") {
        alert("Please enter a date and time.");
        return;
    }

    const dateParts = dateEl.value.split("-");   // YYYY-MM-DD
    const timeParts = timeEl.value.split(":");   // HH:MM or HH:MM:SS

    const payload = {
        year: parseInt(dateParts[0], 10),
        month: parseInt(dateParts[1], 10),
        day: parseInt(dateParts[2], 10),
        hour: parseInt(timeParts[0], 10),
        min: parseInt(timeParts[1], 10),
        sec: timeParts.length > 2 ? parseInt(timeParts[2], 10) : 0
    };

    var data = new FormData();
    data.append('body', JSON.stringify(payload));

    await fetch("/api/datetime", {
        method: 'POST',
        body: data
    }).then(response => {
        if (response.status == 200) {
            window.location.href = "/index.html";
        } else {
            alert("Save failed.");
        }
    }).catch(() => {
        alert("Save failed.");
    });
}
