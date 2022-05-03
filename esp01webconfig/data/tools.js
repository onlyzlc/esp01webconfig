const menu = {
  home: {
    title: "Home",
    href: "/",
  },
  wifiConfig: {
    title: "WiFi",
    href: "/wifi",
  },
  update: {
    title: "更新固件",
    href: "/update",
  },
};

const title = document.createElement("h1");
const statusCol = document.createElement("p");
const nav = document.createElement("nav");
for (const key in menu) {
  if (Object.hasOwnProperty.call(menu, key)) {
    const element = menu[key];
    const item = document.createElement("a");
    item.id = key;
    item.href = element.href;
    item.innerText = element.title;
    nav.append(item);
  }
}

elm("header").append(title, statusCol, nav);

const WifiStatus = [
  "WL_IDLE_STATUS",
  "WL_NO_SSID_AVAIL",
  "WL_SCAN_COMPLETED",
  "WL_CONNECTED",
  "WL_CONNECT_FAILED ",
  "WL_CONNECTION_LOST",
  "WL_WRONG_PASSWORD ",
  "WL_DISCONNECTED ",
];

var WiFi = {
  get status() {
    return this._s;
  },
  set status(val) {
    this._s = val;
    if (val == 3) {
      statusCol.innerText = `已连接: ${this.ssid} (IP: ${this.ip})`;
    } else {
      statusCol.innerText =
        "没有连接 Wifi, 错误代码: " + WifiStatus[this.status];
    }
  },
  get hostName() {
    return this._h;
  },
  set hostName(val) {
    this._h = val;
    title.innerText = val;
  },
  _s: "",
  _h: "",
  ssid: "",
  ip: "",
};

function getWifiInfo(callback) {
  xmlhttp = new XMLHttpRequest();
  xmlhttp.timeout = 20000;
  xmlhttp.onreadystatechange = function () {
    if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
      let res = JSON.parse(xmlhttp.responseText);
      WiFi.ssid = res.ssid;
      WiFi.ip = res.ip;
      WiFi.hostName = res.hostName;
      WiFi.status = res.status;
      console.log("WiFi info: %o", WiFi);
      // elm("#hostName").innerText = WiFi.hostName
      // if (WiFi.status == 3) {
      //   // connectedWifi = WiFi.ssid;
      //   elm("#wifiStatus").innerHTML = `已连接: ${WiFi.ssid} (IP: ${WiFi.ip})` ;
      // } else {
      //   // connectedWifi = ""
      //   elm("#wifiStatus").innerHTML = "没有登录 Wifi, 错误代码: " + WifiStatus[WiFi.status];
      // }
      if (callback != null) {
        callback(WiFi);
      }
    }
  };
  xmlhttp.open("GET", "/wifiInfo", true);
  xmlhttp.send();
}

// 工具函数
function elm(selector) {
  return document.querySelector(selector);
}

function disable(selector) {
  let _e = selector.nodeType == 1 ? selector : elm(selector);
  _e.setAttribute("disabled", "disabled");
}

function enable(selector) {
  let _e = selector.nodeType == 1 ? selector : elm(selector);
  _e.removeAttribute("disabled");
}

function display(selector) {
  let _e = selector.nodeType == 1 ? selector : elm(selector);
  _e.classList.remove("hidden");
}

function hide(selector) {
  let _e = selector.nodeType == 1 ? selector : elm(selector);
  _e.classList.add("hidden");
}
