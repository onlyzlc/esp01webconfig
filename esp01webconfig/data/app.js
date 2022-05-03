elm("form#wifiConfig").addEventListener("submit", submitHandller);
elm("form#wifiConfig #cancel").addEventListener("click", function (event) {
  event.preventDefault();
  hide("#submitWifiConfig");
});
elm("#otherWifi").addEventListener("click", function () {
  // 手动填写,弹出名称密码输入框
  hide(elm_tip);
  elm("#submitWifiConfig .title").innerHTML = "其他网络";
  display("#ssid_w");
  elm("#ssid").value = "";
  elm("#password").value = "";
  display("#submitWifiConfig");
  elm("#ssid").focus();
})
elm(".dialogBox .content").addEventListener("click", function (event) {
  event.stopPropagation();
})
elm(".dialogBox").addEventListener("click", function () {
  hide(this);
})

let timmer1, networks, targetWifi;

let elm_tip = document.createElement("div");
elm_tip.setAttribute("class", "tip");
elm("form#wifiConfig").append(elm_tip);
hide(elm_tip);

let elm_networks = elm("#list");
elm_networks.addEventListener("click", clickListHandler);

function hideTip() {
  // 表单输入时,隐藏提示
  hide(elm_tip);
}

function updateList() {
  if (elm(`li[data-connected]`)) {
    elm(`li[data-connected]`).removeAttribute("data-connected");
  }
  if (WiFi.ssid) {
    elm(`li[data-ssid=${WiFi.ssid}]`).setAttribute("data-connected", "");
  }
}


function getWifiList() {
  elm("#listbox").classList.add("loading");
  xmlhttp = new XMLHttpRequest();
  xmlhttp.timeout = 10000;
  xmlhttp.onreadystatechange = function () {
    if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
      const res = JSON.parse(xmlhttp.responseText);
      // console.log(res);
      if (res < 0) {
        elm("#title").innerHTML = "正在扫描附近的网络...";
        setTimeout(getWifiList, 5000);
      } else if (res == 0) {
        elm("#listbox").classList.remove("loading");
        elm("#title").innerHTML = "没有发现网络";
      } else {
        elm("#listbox").classList.remove("loading");
        elm("#title").innerHTML = "附近的网络:";
        networks = res.networks.filter(item => item != null);
        // 按信号强度排序
        networks.sort(function (a, b) {
          return (b.rssi - a.rssi)
        });
        for (let i = 0; i < networks.length; i++) {
          const item = networks[i];
          if (item.ssid) {
            let elm_network = document.createElement("li");
            elm_networks.appendChild(elm_network);
            elm_network.setAttribute("data-ssid", item.ssid);
            elm_network.setAttribute("data-rssi", item.rssi);
            if (item.open) {
              elm_network.setAttribute("data-open", "");
            }
            if (item.ssid == WiFi.ssid) {
              elm_network.setAttribute("data-connected", "");
            }
            elm_network.innerHTML = ` ${item.ssid} `;
          }
        }
      }
    }
  };
  xmlhttp.open("GET", "/wifiList", true);
  xmlhttp.send();
}

function clickListHandler(event) {
  // 找到li
  let li = event.target;
  while (li.nodeName.toLowerCase() != "li") {
    li = li.parentElement;
  }
  if (li.hasAttribute("data-connected")) return
  targetWifi = li.getAttribute("data-ssid");
  elm("#ssid").value = targetWifi;
  if (li.getAttribute("data-open") == null || "") {
    // 用户选择了非开放网络,弹出密码输入框
    hide(elm_tip);
    // disable("#ssid");
    hide("#ssid_w");
    elm("#submitWifiConfig .title").innerHTML = targetWifi;
    elm("#password").value = "";
    display("#submitWifiConfig");
    elm("#password").focus();
  } else {
  }
}

function submitHandller(event) {
  event.preventDefault();

  disable("form#wifiConfig fieldset");
  elm("#submit").innerHTML = "正在测试连接...";
  hide(elm_tip);

  var ssid = elm("#ssid").value;
  var password = elm("#password").value;

  xmlhttp = new XMLHttpRequest();
  xmlhttp.onreadystatechange = function () {
    elm("#wifiStatus").innerHTML = "正在连接到:" + targetWifi;
    if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
      // 校验通过, 稍后测试连接
      setTimeout(function () {
        getWifiInfo(function () {
          if (WiFi.ssid == targetWifi) {
            // 说明ESP连接上了新WiFi
            elm("#submit").innerHTML = "连接成功";
            hide("#submitWifiConfig");
          } else {
            // 说明ESP因为没连上新设置的wifi而重启了, 重启后自动连接了原WiFi
            elm_tip.innerHTML = "连接失败";
            display(elm_tip);
            elm("form#wifiConfig").addEventListener("input", hideTip, { once: true });
          }
          updateList();
          enable("form#wifiConfig fieldset");
          elm("#submit").innerHTML = "确定";
        });
      }, 3000);
    } else if (xmlhttp.readyState == 4 && xmlhttp.status == 403) {
      // 输入的值有问题
      res = xmlhttp.responseText;
      elm("#submit").innerHTML = "确定";
      enable("form#wifiConfig fieldset");
      elm_tip.innerHTML = res;
      display(elm_tip);
      elm("form#wifiConfig").addEventListener("input", hideTip, { once: true });
    }
  };

  xmlhttp.open("POST", "/wificonfig", true);
  xmlhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
  xmlhttp.send(`ssid=${ssid}&password=${password}`);
}