( function() {  var mapping = [{"appname":"", "appid":"PIC32CX-BZ2;BZ2_APP_DEV.BLE", "path":"GUID-3CC3561E-252E-43B9-BC8A-47DD108A0A46.html"},{"appname":"", "appid":"WBZ451;BZ2_APP_DEV.BLE", "path":"GUID-3CC3561E-252E-43B9-BC8A-47DD108A0A46.html"},{"appname":"", "appid":"BZ2_APP_DEV.BLE", "path":"GUID-3CC3561E-252E-43B9-BC8A-47DD108A0A46.html"},{"appname":"", "appid":"PIC32CX-BZ2;BZ2_APP_DEV.BLE.BUILDING_BLOCK", "path":"GUID-17DABF04-E5D8-4201-A746-2FC244450A19.html"},{"appname":"", "appid":"WBZ451;BZ2_APP_DEV.BLE.BUILDING_BLOCK", "path":"GUID-17DABF04-E5D8-4201-A746-2FC244450A19.html"},{"appname":"", "appid":"BZ2_APP_DEV.BLE.BUILDING_BLOCK", "path":"GUID-17DABF04-E5D8-4201-A746-2FC244450A19.html"},{"appname":"", "appid":"PIC32CX-BZ2;BZ2_APP_DEV.BLE.BUILDING_BLOCK.CENTRAL", "path":"GUID-E8C0DDA5-3AD0-42A4-B4EF-BCB02811BF8C.html"},{"appname":"", "appid":"WBZ451;BZ2_APP_DEV.BLE.BUILDING_BLOCK.CENTRAL", "path":"GUID-E8C0DDA5-3AD0-42A4-B4EF-BCB02811BF8C.html"},{"appname":"", "appid":"BZ2_APP_DEV.BLE.BUILDING_BLOCK.CENTRAL", "path":"GUID-E8C0DDA5-3AD0-42A4-B4EF-BCB02811BF8C.html"},{"appname":"", "appid":"PIC32CX-BZ2;BZ2_APP_DEV.WBZ45_EVB", "path":"GUID-4A40968E-8B6E-4C84-93B9-D31419BBBDA6.html"},{"appname":"", "appid":"WBZ451;BZ2_APP_DEV.WBZ45_EVB", "path":"GUID-4A40968E-8B6E-4C84-93B9-D31419BBBDA6.html"},{"appname":"", "appid":"BZ2_APP_DEV.WBZ45_EVB", "path":"GUID-4A40968E-8B6E-4C84-93B9-D31419BBBDA6.html"},{"appname":"", "appid":"PIC32CX-BZ2;BZ2_APP_DEV_GUIDE", "path":"GUID-7663617B-0DD1-45FA-86B5-EB0778A5A424.html"},{"appname":"", "appid":"WBZ451;BZ2_APP_DEV_GUIDE", "path":"GUID-7663617B-0DD1-45FA-86B5-EB0778A5A424.html"},{"appname":"", "appid":"BZ2_APP_DEV_GUIDE", "path":"GUID-7663617B-0DD1-45FA-86B5-EB0778A5A424.html"},{"appname":"", "appid":"PIC32CX-BZ2;BZ2_APP_DEV.DFU.BOOTLOADER", "path":"GUID-399616BF-E65E-43B3-9831-4B19472A5EF0.html"},{"appname":"", "appid":"WBZ451;BZ2_APP_DEV.DFU.BOOTLOADER", "path":"GUID-399616BF-E65E-43B3-9831-4B19472A5EF0.html"},{"appname":"", "appid":"BZ2_APP_DEV.DFU.BOOTLOADER", "path":"GUID-399616BF-E65E-43B3-9831-4B19472A5EF0.html"},{"appname":"", "appid":"PIC32CX-BZ2;BZ2_APP_DEV.P2P_PHY_APP", "path":"GUID-3E242310-437D-4F50-9E63-3C9EF144CD18.html"},{"appname":"", "appid":"WBZ451;BZ2_APP_DEV.P2P_PHY_APP", "path":"GUID-3E242310-437D-4F50-9E63-3C9EF144CD18.html"},{"appname":"", "appid":"BZ2_APP_DEV.P2P_PHY_APP", "path":"GUID-3E242310-437D-4F50-9E63-3C9EF144CD18.html"}];
            var mchp = (function (mchp) {
                var mchp = mchp || {};
                var mapping = [];
        
                mchp.utils = {};
        
                mchp.utils.getQueryParam = function (name, url = window.location.href) {
                  name = name.replace(/[\[\]]/g, "\\$&");
                  var regex = new RegExp("[?&]" + name + "(=([^&#]*)|&|#|$)"),
                    results = regex.exec(url);
                  if (!results) return null;
                  if (!results[2]) return "";
                  return decodeURIComponent(results[2].replace(/\+/g, " "));
                };

                mchp.utils.isFirefox = typeof InstallTrigger !== 'undefined';
        
                mchp.init = function (options) {
                  mchp.mapping = options.mapping || [];
                  mchp.bindEvents();
                };
        
                mchp.bindEvents = function () {
                    if (mchp.utils.isFirefox) {
                      window.onload = mchp.checkRedirect;
                    } else {
                      document.onreadystatechange = mchp.checkRedirect;
                    }
                };

                mchp.checkRedirect = function() {
                  var contextId = mchp.utils.getQueryParam("contextId") || "";
                  if (contextId && contextId != undefined) {
                    var record = mchp.mapping.find(function(x){
                      return x.appid && x.appid.toLowerCase() == contextId.toLowerCase();
                    });
                    if (record && record.path) {
                      window.stop();
                      window.location = record.path;
                    }
                  }
                };
        
                return {
                  init: mchp.init,
                };
              })();
        
              mchp.init({
                mapping: mapping
              });

        })();