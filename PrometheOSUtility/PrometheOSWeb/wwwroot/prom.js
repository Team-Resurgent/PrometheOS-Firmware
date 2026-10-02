(function () {
    const toastRootId = "prom-toast-root";

    function ensureToastRoot() {
        let root = document.getElementById(toastRootId);
        if (root != null) {
            return root;
        }

        root = document.createElement("div");
        root.id = toastRootId;
        root.className = "prom-toast-root";
        document.body.appendChild(root);
        return root;
    }

    function api(path) {
        if (path.startsWith("/")) {
            return fetch(path);
        }
        return fetch("/api/" + path);
    }

    function toast(message, type) {
        const root = ensureToastRoot();
        const item = document.createElement("div");
        item.className = "prom-toast" + (type ? " prom-toast-" + type : "");
        item.textContent = message;
        root.appendChild(item);

        window.setTimeout(function () {
            item.classList.add("prom-toast-hide");
            window.setTimeout(function () {
                item.remove();
            }, 300);
        }, 3200);
    }

    async function waitForOnline(timeoutMs) {
        const deadline = Date.now() + (timeoutMs || 120000);
        while (Date.now() < deadline) {
            try {
                const response = await api("status.json");
                if (response.ok) {
                    const data = await response.json();
                    if (data.online === true) {
                        return data;
                    }
                }
            } catch (error) {
            }
            await new Promise(function (resolve) {
                window.setTimeout(resolve, 1500);
            });
        }
        throw new Error("Timed out waiting for console.");
    }

    async function rebootAndWait() {
        toast("Rebooting console...");
        await api("reboot");
        toast("Waiting for console to come back online...");
        await waitForOnline();
        toast("Console is back online.", "success");
    }

    async function shutdownConsole() {
        toast("Shutting down console...");
        await api("shutdown");
    }

    function formatBytes(bytes) {
        if (bytes < 1024) {
            return bytes + " B";
        }
        if (bytes < 1024 * 1024) {
            return (bytes / 1024).toFixed(1) + " KB";
        }
        if (bytes < 1024 * 1024 * 1024) {
            return (bytes / (1024 * 1024)).toFixed(1) + " MB";
        }
        return (bytes / (1024 * 1024 * 1024)).toFixed(2) + " GB";
    }

    window.Promethe = {
        api: api,
        toast: toast,
        waitForOnline: waitForOnline,
        rebootAndWait: rebootAndWait,
        shutdownConsole: shutdownConsole,
        formatBytes: formatBytes
    };
})();
