(window["webpackJsonp"]=window["webpackJsonp"]||[]).push([["chunk-72247be4"],{"0f145":function(e,t,n){},"1f75":function(e,t,n){"use strict";var r=n("1da1"),a=(n("96cf"),n("d3b7"),n("6062"),n("3ca3"),n("ddb0"),n("caad"),n("2532"),n("99af"),n("25f0"),n("cee4")),c=n("2295"),o=n("5530"),u=(n("a9e3"),n("b680"),n("159b"),n("3ef4")),i=n("e466"),s=n("2ef0"),l=n.n(s),p=25,f=function(e){var t=1e3*Number((e.length/p).toFixed(3));return t<3e3?3e3:t>8e3?8e3:t},d=function(e){var t=!e||Object(s["isString"])(e)||Object(s["isFunction"])(e)?{message:e}:e;return t},b=function(e){var t=d(e),n=t.message,r=t.duration,a=t.showClose,c=t.type,i=r||n&&f(n)||void 0,s=a||"error"===c,l=Object(u["a"])(Object(o["a"])(Object(o["a"])({},e),{},{duration:i,showClose:s}));return window.setTimeout((function(){var e;return null===l||void 0===l||null===(e=l.close)||void 0===e?void 0:e.call(l)}),i?i+2e3:8e3),l};i["d"].forEach((function(e){b[e]=function(){var t=arguments.length>0&&void 0!==arguments[0]?arguments[0]:{},n=d(t);return b(Object(o["a"])(Object(o["a"])({},n),{},{type:e}))}}));var m=b,g=n("323e"),O=n.n(g),v=(n("a5d8"),n("afbc")),j=n("0613"),h=n("2fc2"),w="BAD_TOKEN",k="TOKEN_TIME_OUT",N="BAD_USERNAME_OR_PWD",x=n("88c3");O.a.configure({showSpinner:!1,trickleSpeed:200});var C=new Set,y=function(){return C=new Set};Object.assign(a["a"].defaults,{baseURL:h["b"],timeout:2e4}),a["a"].interceptors.request.use((function(e){var t=j["a"].state.user;e.headers={Authorization:"Bearer "+t.token};var n=new AbortController;return e.signal=n.signal,e.controller=n,j["a"].commit("ADD_ABORT_CONTROLLER",n),e}),(function(e){Promise.reject(e)})),a["a"].interceptors.request.use(function(){var e=Object(r["a"])(regeneratorRuntime.mark((function e(t){return regeneratorRuntime.wrap((function(e){while(1)switch(e.prev=e.next){case 0:if(t.doNotTriggerProgress){e.next=4;break}return j["a"].state.request_queue||O.a.start(),e.next=4,j["a"].dispatch("SET_REQ_CHANGE",!0);case 4:return e.abrupt("return",t);case 5:case"end":return e.stop()}}),e)})));return function(t){return e.apply(this,arguments)}}());var R=function(e,t){return 401===e&&[w,k].includes(t.code)},_=function(){var e=Object(r["a"])(regeneratorRuntime.mark((function e(t){var n;return regeneratorRuntime.wrap((function(e){while(1)switch(e.prev=e.next){case 0:return e.prev=0,e.next=3,t.text();case 3:return n=e.sent,e.abrupt("return",JSON.parse(n));case 7:return e.prev=7,e.t0=e["catch"](0),e.abrupt("return",{});case 10:case"end":return e.stop()}}),e,null,[[0,7]])})));return function(t){return e.apply(this,arguments)}}();function E(){return S.apply(this,arguments)}function S(){return S=Object(r["a"])(regeneratorRuntime.mark((function e(){var t;return regeneratorRuntime.wrap((function(e){while(1)switch(e.prev=e.next){case 0:return e.next=2,j["a"].dispatch("SET_REQ_CHANGE",!1);case 2:t=j["a"].state.request_queue,t>0?O.a.inc():O.a.done();case 4:case"end":return e.stop()}}),e)}))),S.apply(this,arguments)}a["a"].interceptors.response.use((function(e){if(e.config.doNotTriggerProgress||E(),e.data instanceof Blob)return e;var t=e.config.controller;return j["a"].commit("REMOVE_ABORT_CONTROLLER",t),e.data||e.status}),function(){var e=Object(r["a"])(regeneratorRuntime.mark((function e(t){var n,r,a,o,u,i,s,p,f,d,b;return regeneratorRuntime.wrap((function(e){while(1)switch(e.prev=e.next){case 0:if(t.config.doNotTriggerProgress||E(),!t.response){e.next=20;break}if(!(t.response.data instanceof Blob)){e.next=6;break}return e.next=5,_(t.response.data);case 5:t.response.data=e.sent;case 6:if(n=t.response,r=n.data,a=n.status,C.has(a)){e.next=18;break}if(C.add(a),u=401===a&&j["a"].state.afterCurrentUserPwdChanged,!R(a,r)){e.next=15;break}return u?j["a"].commit("SET_AFTER_CURRENT_USER_PWD_CHANGED",!1):c["a"].error(x["a"].global.t("Base.tokenExpiredMsg")),Object(v["c"])(),window.setTimeout(y,1e3),e.abrupt("return",Promise.reject(t));case 15:i=(null===(o=t.config)||void 0===o?void 0:o.errorsHandleCustom)&&Array.isArray(t.config.errorsHandleCustom)&&t.config.errorsHandleCustom.includes(a),i||((null===r||void 0===r?void 0:r.code)===N?c["a"].error(x["a"].global.t("Base.namePwdError")):null!==r&&void 0!==r&&r.code||null!==r&&void 0!==r&&r.message?m.error("".concat(a," ").concat(null!==(s=null===r||void 0===r?void 0:r.code)&&void 0!==s?s:"",": ").concat(null!==(p=null===r||void 0===r||null===(f=r.message)||void 0===f?void 0:f.toString())&&void 0!==p?p:"")):m.error(a+" Network error")),401===a&&Object(v["c"])();case 18:e.next=24;break;case 20:if(d=t.code===h["F"]&&t.config.handleTimeoutSelf,"ERR_CANCELED"!==t.code||"canceled"!==t.message){e.next=23;break}return e.abrupt("return");case 23:C.has(0)||(d||m.error(x["a"].global.t("Base.networkError")),C.add(0));case 24:return 0===j["a"].state.request_queue&&(C=new Set),l.a.throttle(y,2e3,{trailing:!1}),b=t.config.controller,j["a"].commit("REMOVE_ABORT_CONTROLLER",b),e.abrupt("return",Promise.reject(t));case 29:case"end":return e.stop()}}),e)})));return function(t){return e.apply(this,arguments)}}());t["a"]=a["a"]},3286:function(e,t,n){},"44ea":function(e,t,n){"use strict";var r=n("5530"),a=(n("a9e3"),n("7a23")),c=function(e){return Object(a["pushScopeId"])("data-v-78a08d2d"),e=e(),Object(a["popScopeId"])(),e},o=c((function(){return Object(a["createElementVNode"])("i",{class:"iconfont icon-question"},null,-1)})),u=Object(a["defineComponent"])({name:"InfoTooltip"}),i=Object(a["defineComponent"])(Object(r["a"])(Object(r["a"])({},u),{},{props:{content:{type:String},popperClass:{type:String},place:{type:String,default:"top"},showAfter:{type:Number,default:500}},setup:function(e){var t=e,n=Object(a["computed"])((function(){return"info-tooltip ".concat(t.popperClass)}));return function(t,r){var c=Object(a["resolveComponent"])("el-tooltip");return Object(a["openBlock"])(),Object(a["createBlock"])(c,{effect:"dark","popper-class":Object(a["unref"])(n),placement:e.place,content:e.content,"show-after":e.showAfter},Object(a["createSlots"])({default:Object(a["withCtx"])((function(){return[o]})),_:2},[t.$slots.content?{name:"content",fn:Object(a["withCtx"])((function(){return[Object(a["renderSlot"])(t.$slots,"content")]}))}:void 0]),1032,["popper-class","placement","content","show-after"])}}})),s=(n("6368"),n("afbc6"),n("6b0d")),l=n.n(s);const p=l()(i,[["__scopeId","data-v-78a08d2d"]]);t["a"]=p},"50a4":function(e,t,n){"use strict";var r=n("7a23"),a=function(){return{limit:20,count:0,page:1,hasnext:!1}};t["a"]=function(){var e=Object(r["ref"])(a()),t=Object(r["computed"])((function(){return{limit:e.value.limit,page:e.value.page}})),n=function(){e.value=a()},c=function(t){void 0===t.count?(e.value.count=-1,e.value.hasnext=t.hasnext):e.value.count=t.count,e.value.page=t.page};return{pageMeta:e,pageParams:t,initPageMeta:n,setPageMeta:c}}},"583f":function(e,t,n){"use strict";var r=n("7a23"),a=(n("a9e3"),Object(r["defineComponent"])({props:{currentPage:{type:Number,required:!0},hasnext:{type:Boolean,required:!0}},emits:["current-change"],setup:function(e,t){var n=t.emit,a=e,c=Object(r["computed"])((function(){return a.hasnext?a.currentPage+1:a.currentPage})),o=function(e){n("current-change",e)};return function(t,n){var a=Object(r["resolveComponent"])("el-pagination");return Object(r["openBlock"])(),Object(r["createBlock"])(a,{background:"",layout:"prev, next","current-page":e.currentPage,"page-count":Object(r["unref"])(c),onCurrentChange:o},null,8,["current-page","page-count"])}}}));const c=a;var o=c,u={class:"common-pagination"},i=Object(r["defineComponent"])({props:{metaData:{type:Object,required:!0,default:function(){return{}}}},emits:["loadPage","update:metaData"],setup:function(e,t){var n,a,c=t.emit,i=e,s=Object(r["computed"])((function(){return i.metaData}));(n=s.value).limit||(n.limit=20),(a=s.value).page||(a.page=1);var l=[20,50,100,500];Object(r["watch"])(s,(function(e){c("update:metaData",e)}));var p=function(e){s.value.page=1,c("loadPage",{page:s.value.page,limit:e})},f=function(e){s.value.page!==e&&(s.value.page=e),c("loadPage",{page:e,limit:s.value.limit})};return function(e,t){var n=Object(r["resolveComponent"])("el-pagination");return Object(r["openBlock"])(),Object(r["createElementBlock"])("div",u,[Object(r["unref"])(s).count&&Object(r["unref"])(s).count>l[0]?(Object(r["openBlock"])(),Object(r["createBlock"])(n,{key:0,background:"",layout:"total, sizes, prev, pager, next","page-sizes":l,"page-size":Object(r["unref"])(s).limit,"onUpdate:page-size":t[0]||(t[0]=function(e){return Object(r["unref"])(s).limit=e}),"current-page":Object(r["unref"])(s).page,"onUpdate:current-page":t[1]||(t[1]=function(e){return Object(r["unref"])(s).page=e}),total:Object(r["unref"])(s).count,onSizeChange:p,onCurrentChange:f},null,8,["page-size","current-page","total"])):-1===Object(r["unref"])(s).count?(Object(r["openBlock"])(),Object(r["createBlock"])(o,{key:1,"current-page":Object(r["unref"])(s).page,hasnext:Object(r["unref"])(s).hasnext,onCurrentChange:f},null,8,["current-page","hasnext"])):Object(r["createCommentVNode"])("",!0)])}}});const s=i;t["a"]=s},6368:function(e,t,n){"use strict";n("0f145")},"6b0d":function(e,t,n){"use strict";Object.defineProperty(t,"__esModule",{value:!0}),t.default=(e,t)=>{const n=e.__vccOpts||e;for(const[r,a]of t)n[r]=a;return n}},8444:function(e,t,n){},9129:function(e,t,n){var r=n("23e7");r({target:"Number",stat:!0},{isNaN:function(e){return e!=e}})},"9d39":function(e,t,n){"use strict";n.d(t,"a",(function(){return a}));n("99af");var r=n("47e2");function a(e){var t=Object(r["b"])(),n=t.t,a=function(t,r){return r?n("".concat(e,".").concat(t),r):n("".concat(e,".").concat(t))};return{t:n,tl:a}}},"9e69":function(e,t,n){"use strict";n.d(t,"a",(function(){return p}));var r=n("7a23"),a=n("9ee5");const c=Object(r["defineComponent"])({name:"Remove"}),o={viewBox:"0 0 1024 1024",xmlns:"http://www.w3.org/2000/svg"},u=Object(r["createElementVNode"])("path",{fill:"currentColor",d:"M352 480h320a32 32 0 1 1 0 64H352a32 32 0 0 1 0-64z"},null,-1),i=Object(r["createElementVNode"])("path",{fill:"currentColor",d:"M512 896a384 384 0 1 0 0-768 384 384 0 0 0 0 768zm0 64a448 448 0 1 1 0-896 448 448 0 0 1 0 896z"},null,-1),s=[u,i];function l(e,t,n,a,c,u){return Object(r["openBlock"])(),Object(r["createElementBlock"])("svg",o,s)}var p=Object(a["a"])(c,[["render",l]])},"9ee5":function(e,t,n){"use strict";n.d(t,"a",(function(){return r}));var r=(e,t)=>{const n=e.__vccOpts||e;for(const[r,a]of t)n[r]=a;return n}},afbc6:function(e,t,n){"use strict";n("8444")},b06c:function(e,t,n){"use strict";var r,a=n("ade3"),c=(n("d81d"),n("fb6a"),n("d3b7"),n("9129"),n("a9e3"),n("c740"),n("99af"),n("498a"),n("ca5a")),o=n("9d39"),u=[c["J"].Millisecond,c["J"].Second,c["J"].Minute,c["J"].Hour,c["J"].Day],i=(r={},Object(a["a"])(r,c["J"].Millisecond,1e3),Object(a["a"])(r,c["J"].Second,60),Object(a["a"])(r,c["J"].Minute,60),Object(a["a"])(r,c["J"].Hour,24),r),s=function(e){return u.slice(e,-1).map((function(t,n){for(var r=i[u[e]],a=e+1;a<u.length-1-n;a++)r*=i[u[a]];return r}))};t["a"]=function(){var e,t=Object(o["a"])("General"),n=t.t,r=(e={},Object(a["a"])(e,c["J"].Millisecond,"General.ms"),Object(a["a"])(e,c["J"].Second,"General.sec"),Object(a["a"])(e,c["J"].Minute,"General.min"),Object(a["a"])(e,c["J"].Hour,"General.hour"),Object(a["a"])(e,c["J"].Day,"General.day"),e),i=function(e,t){if(void 0===t||!u.some((function(e){return e===t}))||Number.isNaN(Number(e))||0===Number(e))return e;for(var a=Number(e),c=u.findIndex((function(e){return e===t})),o=s(c),i=u.slice(c).reverse(),l="",p=a,f=0;f<o.length;f++){var d=Math.floor(p/o[f]);d>=1&&(l+=" ".concat(d," ").concat(n(r[i[f]],d)),p-=d*o[f])}return 0!==p&&(l+=" ".concat(p," ").concat(n(r[i[i.length-1]],p))),l.trim()},l=function(e){return i(Math.floor(e/1e3),c["J"].Second)},p=function(e){return i(e,c["J"].Second)};return{getDurationStr:i,transMsNumToSimpleStr:l,transSecondNumToSimpleStr:p}}},b66b:function(e,t,n){"use strict";n.r(t);var r=n("1da1"),a=n("5530"),c=(n("b64b"),n("96cf"),n("7a23")),o=n("f727"),u=n("4c61"),i=n("583f"),s=n("44ea"),l=n("9e69"),p=n("c9a1"),f=n("3ef4"),d=n("b06c"),b=n("50a4"),m=n("47e2"),g={class:"alarm app-wrapper"},O={class:"section-header"},v=Object(c["createElementVNode"])("div",null,null,-1),j={class:"message-with-icon"},h=Object(c["createElementVNode"])("br",null,null,-1),w={class:"emq-table-footer"},k=Object(c["defineComponent"])({name:"currentAlarm"}),N=Object(c["defineComponent"])(Object(a["a"])(Object(a["a"])({},k),{},{setup:function(e){var t=Object(c["ref"])(!1),n=Object(c["ref"])([]),k=Object(m["b"])(),N=k.t,x=Object(b["a"])(),C=x.pageMeta,y=x.pageParams,R=x.initPageMeta,_=x.setPageMeta,E=Object(d["a"])(),S=E.transMsNumToSimpleStr,B=function(){var e=Object(r["a"])(regeneratorRuntime.mark((function e(){var r,c,u,i,s,l=arguments;return regeneratorRuntime.wrap((function(e){while(1)switch(e.prev=e.next){case 0:return r=l.length>0&&void 0!==l[0]?l[0]:{},t.value=!0,c=Object(a["a"])(Object(a["a"])({},y.value),r),e.prev=3,e.next=6,Object(o["f"])(!0,c);case 6:u=e.sent,i=u.data,s=u.meta,n.value=i,_(s),e.next=17;break;case 13:e.prev=13,e.t0=e["catch"](3),n.value=[],R();case 17:return e.prev=17,t.value=!1,e.finish(17);case 20:case"end":return e.stop()}}),e,null,[[3,13,17,20]])})));return function(){return e.apply(this,arguments)}}(),T=function(){var e=Object(r["a"])(regeneratorRuntime.mark((function e(){var t;return regeneratorRuntime.wrap((function(e){while(1)switch(e.prev=e.next){case 0:return e.prev=0,e.next=3,p["a"].confirm(N("Alarm.clearConfirm"),{confirmButtonText:N("Base.confirm"),cancelButtonText:N("Base.cancel"),confirmButtonClass:"confirm-danger",type:"warning"});case 3:return e.next=5,Object(o["a"])().catch((function(){}));case 5:t=e.sent,t&&(f["a"].success(N("Alarm.clearSuccess")),B()),e.next=12;break;case 9:e.prev=9,e.t0=e["catch"](0),console.error(e.t0);case 12:case"end":return e.stop()}}),e,null,[[0,9]])})));return function(){return e.apply(this,arguments)}}();return B(),function(e,r){var a=Object(c["resolveComponent"])("el-button"),o=Object(c["resolveComponent"])("el-table-column"),p=Object(c["resolveComponent"])("el-table"),f=Object(c["resolveDirective"])("loading");return Object(c["openBlock"])(),Object(c["createElementBlock"])("div",g,[Object(c["createElementVNode"])("div",O,[v,Object(c["createElementVNode"])("div",null,[Object(c["createVNode"])(a,{type:"danger",plain:"",icon:Object(c["unref"])(l["a"]),onClick:T,disabled:!n.value.length},{default:Object(c["withCtx"])((function(){return[Object(c["createTextVNode"])(Object(c["toDisplayString"])(e.$t("Alarm.clearHistory")),1)]})),_:1},8,["icon","disabled"])])]),Object(c["withDirectives"])((Object(c["openBlock"])(),Object(c["createBlock"])(p,{data:n.value},{default:Object(c["withCtx"])((function(){return[Object(c["createVNode"])(o,{prop:"name",label:e.$t("Alarm.alarmName")},null,8,["label"]),Object(c["createVNode"])(o,{label:e.$t("Alarm.alarmMsg")},{default:Object(c["withCtx"])((function(e){var t=e.row;return[Object(c["createElementVNode"])("div",j,[Object.keys(t.details).length?(Object(c["openBlock"])(),Object(c["createBlock"])(s["a"],{key:0},{content:Object(c["withCtx"])((function(){return[(Object(c["openBlock"])(!0),Object(c["createElementBlock"])(c["Fragment"],null,Object(c["renderList"])(t.details,(function(e,t){return Object(c["openBlock"])(),Object(c["createElementBlock"])("div",{key:t},Object(c["toDisplayString"])(t)+": "+Object(c["toDisplayString"])(e),1)})),128))]})),_:2},1024)):Object(c["createCommentVNode"])("",!0),Object(c["createElementVNode"])("span",null,Object(c["toDisplayString"])(t.message),1)])]})),_:1},8,["label"]),Object(c["createVNode"])(o,{prop:"node",label:e.$t("Alarm.triggerNode")},null,8,["label"]),Object(c["createVNode"])(o,{label:e.$t("Alarm.alarmLevel")},{default:Object(c["withCtx"])((function(){return[Object(c["createElementVNode"])("span",null,Object(c["toDisplayString"])(e.$t("Alarm.system")),1)]})),_:1},8,["label"]),Object(c["createVNode"])(o,{prop:"activate_at",label:e.$t("Alarm.activateTime")},{default:Object(c["withCtx"])((function(e){var t=e.row;return[Object(c["createTextVNode"])(Object(c["toDisplayString"])(t.activate_at&&Object(c["unref"])(u["n"])(t.activate_at)),1),h,Object(c["createTextVNode"])(" "+Object(c["toDisplayString"])(t.deactivate_at&&Object(c["unref"])(u["n"])(t.deactivate_at)),1)]})),_:1},8,["label"]),Object(c["createVNode"])(o,{prop:"deactivate_at",label:e.$t("Alarm.duration")},{default:Object(c["withCtx"])((function(e){var t=e.row;return[Object(c["createTextVNode"])(Object(c["toDisplayString"])(Object(c["unref"])(S)(t.duration)),1)]})),_:1},8,["label"])]})),_:1},8,["data"])),[[f,t.value,void 0,{lock:!0}]]),Object(c["createElementVNode"])("div",w,[Object(c["createVNode"])(i["a"],{metaData:Object(c["unref"])(C),"onUpdate:metaData":r[0]||(r[0]=function(e){return Object(c["isRef"])(C)?C.value=e:null}),onLoadPage:B},null,8,["metaData"])])])}}}));n("d7dd");const x=N;t["default"]=x},d7dd:function(e,t,n){"use strict";n("3286")},f727:function(e,t,n){"use strict";n.d(t,"n",(function(){return o})),n.d(t,"i",(function(){return u})),n.d(t,"h",(function(){return i})),n.d(t,"g",(function(){return s})),n.d(t,"c",(function(){return l})),n.d(t,"p",(function(){return p})),n.d(t,"b",(function(){return f})),n.d(t,"o",(function(){return d})),n.d(t,"l",(function(){return b})),n.d(t,"j",(function(){return g})),n.d(t,"m",(function(){return v})),n.d(t,"k",(function(){return h})),n.d(t,"f",(function(){return w})),n.d(t,"a",(function(){return N})),n.d(t,"e",(function(){return C})),n.d(t,"d",(function(){return y}));var r=n("5530"),a=n("1da1"),c=(n("96cf"),n("1f75"));function o(e){return c["a"].post("/login",e)}function u(e){return c["a"].get("/metrics",{params:{aggregate:e}})}function i(){return c["a"].get("/monitor_current",{doNotTriggerProgress:!0})}function s(e){return c["a"].get("/monitor",{params:{latest:e}})}function l(){return c["a"].get("/prometheus")}function p(e){return c["a"].put("/prometheus",e)}function f(){return c["a"].get("/opentelemetry")}function d(e){return c["a"].put("/opentelemetry",e)}function b(){return m.apply(this,arguments)}function m(){return m=Object(a["a"])(regeneratorRuntime.mark((function e(){var t,n,r=arguments;return regeneratorRuntime.wrap((function(e){while(1)switch(e.prev=e.next){case 0:return t=r.length>0&&void 0!==r[0]&&r[0],n=r.length>1&&void 0!==r[1]?r[1]:2e4,e.abrupt("return",c["a"].get("/nodes",{doNotTriggerProgress:t,timeout:n}));case 3:case"end":return e.stop()}}),e)}))),m.apply(this,arguments)}function g(e){return O.apply(this,arguments)}function O(){return O=Object(a["a"])(regeneratorRuntime.mark((function e(t){return regeneratorRuntime.wrap((function(e){while(1)switch(e.prev=e.next){case 0:return e.abrupt("return",c["a"].get("/nodes/".concat(encodeURIComponent(t))));case 1:case"end":return e.stop()}}),e)}))),O.apply(this,arguments)}function v(){return j.apply(this,arguments)}function j(){return j=Object(a["a"])(regeneratorRuntime.mark((function e(){return regeneratorRuntime.wrap((function(e){while(1)switch(e.prev=e.next){case 0:return e.abrupt("return",c["a"].get("/stats",{doNotTriggerProgress:!0}));case 1:case"end":return e.stop()}}),e)}))),j.apply(this,arguments)}function h(e){return c["a"].get("/nodes/".concat(encodeURIComponent(e),"/stats"))}function w(){return k.apply(this,arguments)}function k(){return k=Object(a["a"])(regeneratorRuntime.mark((function e(){var t,n,a=arguments;return regeneratorRuntime.wrap((function(e){while(1)switch(e.prev=e.next){case 0:return t=a.length>0&&void 0!==a[0]&&a[0],n=a.length>1&&void 0!==a[1]?a[1]:{},e.abrupt("return",c["a"].get("/alarms",{params:Object(r["a"])({activated:String(!t)},n)}));case 3:case"end":return e.stop()}}),e)}))),k.apply(this,arguments)}function N(){return x.apply(this,arguments)}function x(){return x=Object(a["a"])(regeneratorRuntime.mark((function e(){return regeneratorRuntime.wrap((function(e){while(1)switch(e.prev=e.next){case 0:return e.abrupt("return",c["a"].delete("/alarms"));case 1:case"end":return e.stop()}}),e)}))),x.apply(this,arguments)}var C=function(){var e=arguments.length>0&&void 0!==arguments[0]?arguments[0]:{};return e.topic=e.topic||void 0,c["a"].get("/topics",{params:e})};function y(){var e=arguments.length>0&&void 0!==arguments[0]?arguments[0]:{};return c["a"].get("/subscriptions",{params:e})}}}]);