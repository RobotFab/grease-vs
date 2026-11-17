var $o=Object.create;var kt=Object.defineProperty;var Io=Object.getOwnPropertyDescriptor;var _o=Object.getOwnPropertyNames;var Oo=Object.getPrototypeOf,Do=Object.prototype.hasOwnProperty;var Go=(t,e)=>{for(var n in e)kt(t,n,{get:e[n],enumerable:!0})},sr=(t,e,n,r)=>{if(e&&typeof e=="object"||typeof e=="function")for(let o of _o(e))!Do.call(t,o)&&o!==n&&kt(t,o,{get:()=>e[o],enumerable:!(r=Io(e,o))||r.enumerable});return t};var C=(t,e,n)=>(n=t!=null?$o(Oo(t)):{},sr(e||!t||!t.__esModule?kt(n,"default",{value:t,enumerable:!0}):n,t)),No=t=>sr(kt({},"__esModule",{value:!0}),t);var Qs={};Go(Qs,{activate:()=>Xs,deactivate:()=>Ys});module.exports=No(Qs);var i=C(require("vscode")),vt=C(require("node:fs")),on=C(require("node:os")),j=C(require("node:path"));var xn=require("node:child_process"),N=C(require("node:fs"),1),Ge=C(require("node:os"),1),P=C(require("node:path"),1),ke=process.platform==="win32",At=process.platform==="darwin",Ct=ke?"arduino-cli.exe":"arduino-cli",Ne=P.default.join(Ge.default.homedir(),".grease","extension"),Et=P.default.join(Ge.default.homedir(),".grease","bin"),yn=P.default.join(Ne,"settings.json"),jo=Object.freeze({ARDUINO_NETWORK_CLOUD_API_SKIP_BOARD_DETECTION_CALLS:"true",ARDUINO_UPDATER_ENABLE_NOTIFICATION:"false",ARDUINO_LIBRARY_ENABLE_UNSAFE_INSTALL:"false"}),_=Object.freeze({quick:15e3,boardList:3e4,compile:3e5,upload:12e4,libInstall:3e5,index:3e5,coreInstall:9e5});function ar(){try{return JSON.parse(N.default.readFileSync(yn,"utf8"))??{}}catch{return{}}}function lr(t){let e={...ar(),...t};N.default.mkdirSync(Ne,{recursive:!0});let n=`${yn}.${process.pid}.tmp`;return N.default.writeFileSync(n,JSON.stringify(e,null,2)),N.default.renameSync(n,yn),e}function Le(t){try{return N.default.statSync(t).isFile()?(ke||N.default.accessSync(t,N.default.constants.X_OK),!0):!1}catch{return!1}}function Fo(){return(process.env.PATH??"").split(P.default.delimiter).map(t=>t.trim().replace(/^"(.*)"$/,"$1")).filter(Boolean)}function je(t){for(let e of Fo()){let n=P.default.join(e,t);if(Le(n))return n}return null}function Uo(){let t=Ge.default.homedir(),e=[P.default.join(t,".local","bin"),P.default.join(t,"bin")],n=["resources","app","lib","backend","resources"];if(At){let r=["Contents","Resources","app","lib","backend","resources"];e.push("/opt/homebrew/bin","/usr/local/bin",P.default.join("/Applications","Arduino IDE.app",...r),P.default.join(t,"Applications","Arduino IDE.app",...r))}else if(ke){let r=process.env.LOCALAPPDATA||P.default.join(t,"AppData","Local"),o=process.env.ProgramFiles||"C:\\Program Files",s=process.env.ProgramData||"C:\\ProgramData";e.push(P.default.join(r,"Microsoft","WinGet","Links"),P.default.join(o,"Arduino CLI"),P.default.join(r,"Programs","Arduino IDE",...n),P.default.join(o,"Arduino IDE",...n),P.default.join(t,"scoop","shims"),P.default.join(s,"chocolatey","bin"));let a=P.default.join(r,"Microsoft","WinGet","Packages");try{for(let d of N.default.readdirSync(a))/^ArduinoSA\./i.test(d)&&e.push(P.default.join(a,d))}catch{}}else e.push("/usr/local/bin","/usr/bin","/snap/bin","/home/linuxbrew/.linuxbrew/bin");return e}function dr(){let t=[],e=P.default.join(Ge.default.homedir(),"Downloads"),n=r=>{let o=P.default.join(r,Ct);Le(o)&&t.push(o)};n(e);try{for(let r of N.default.readdirSync(e))/^arduino[-_]cli/i.test(r)&&n(P.default.join(e,r))}catch{}return t}var et=null;function ve({refresh:t=!1}={}){if(et&&!t&&Le(et.path))return et;et=null;let e=je(Ct),n=(a,d)=>et={path:a,source:d,onPath:!!e&&P.default.resolve(e)===P.default.resolve(a)},r=process.env.ARDUINO_CLI_PATH;if(r&&Le(r))return n(r,"env");let o=ar().arduinoCliPath;if(typeof o=="string"&&Le(o))return n(o,"saved");if(e)return n(e,"PATH");for(let a of Uo()){let d=P.default.join(a,Ct);if(Le(d))return n(d,"found")}let s=P.default.join(Et,Ct);return Le(s)?n(s,"grease"):null}function rt(){return ke?"winget install ArduinoSA.CLI":At?"brew install arduino-cli":"curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | BINDIR=$HOME/.local/bin sh"}function Mt(t){return ke?`powershell -NoProfile -Command "[Environment]::SetEnvironmentVariable('Path', [Environment]::GetEnvironmentVariable('Path','User') + ';${t}', 'User')"`:At?`echo 'export PATH="${t}:$PATH"' >> ~/.zprofile`:`echo 'export PATH="${t}:$PATH"' >> ~/.profile`}function qo(){return["arduino-cli was not found.","Why: Grease runs arduino-cli for every compile, upload and install, and no copy is on PATH or in the usual install folders.",`What: Grease installs it by itself when the editor starts. If that failed: Grease VS sidebar \u203A Help \u203A Install arduino-cli, or run ${rt()}.`].join(`
`)}function wn(t){if(!(!t?.pid||t.exitCode!==null)){if(ke){let e=P.default.join(process.env.SystemRoot||"C:\\Windows","System32","taskkill.exe");(0,xn.spawn)(e,["/pid",String(t.pid),"/T","/F"],{windowsHide:!0,shell:!1}).on("error",()=>{try{t.kill()}catch{}});return}try{process.kill(-t.pid,"SIGKILL")}catch{try{t.kill("SIGKILL")}catch{}}}}var Wo=new Set(["version","config","help","completion"]);async function H(t,e={}){let n=ve();if(!n)return{success:!1,exitCode:null,stdout:"",stderr:qo(),notFound:!0};if(!e.skipFirstRun&&!Wo.has(t[0])){let r=await Cn();if(!r.success)return r}return kn(n,t,e)}function kn(t,e,n){let r=n.timeoutMs??_.quick,o=e.includes("--additional-urls")?e:[...e,"--additional-urls",""];return n.signal?.aborted?Promise.resolve({success:!1,exitCode:null,stdout:"",stderr:"Cancelled.",cancelled:!0}):new Promise(s=>{let a;try{a=(0,xn.spawn)(t.path,o,{cwd:n.cwd,env:{...process.env,...jo,...ke?{}:{TMPDIR:Ge.default.tmpdir()},...n.env??{}},shell:!1,windowsHide:!0,detached:!ke})}catch(w){s({success:!1,exitCode:null,stdout:"",stderr:String(w?.message??w)});return}let d="",u="",m=!1,S=()=>{wn(a),k({success:!1,exitCode:null,stdout:d,stderr:"Cancelled.",cancelled:!0})},k=w=>{m||(m=!0,clearTimeout($),n.signal?.removeEventListener("abort",S),s(w))};n.signal?.addEventListener("abort",S,{once:!0});let $=setTimeout(()=>{wn(a),k({success:!1,exitCode:null,stdout:d,stderr:`${u}
Stopped after ${Math.round(r/1e3)} s (arduino-cli ${e[0]??""} took too long).`,timedOut:!0})},r);a.stdout.on("data",w=>{let I=w.toString();d+=I,n.onData?.(I)}),a.stderr.on("data",w=>{let I=w.toString();u+=I,n.onData?.(I)}),a.on("error",w=>{let I=At&&w?.code==="EACCES";k({success:!1,exitCode:null,stdout:d,stderr:I?`${String(w)}
Why: macOS blocked ${t.path} (downloaded file, not executable or quarantined).
What: chmod +x "${t.path}" && xattr -d com.apple.quarantine "${t.path}"`:`${u}
${String(w)}`,notFound:w?.code==="ENOENT"})}),a.on("close",w=>k({success:w===0,exitCode:w,stdout:d,stderr:u}))})}var De=P.default.join(Ne,"first-run.lock"),Ho=6e4,bn=null;async function Rt(){let t=ve();if(!t)return null;if(bn?.cli===t.path)return bn.dir;let e=await kn(t,["config","get","directories.data","--json"],{timeoutMs:_.quick}),n=e.success?Fe(e.stdout):null;return typeof n!="string"||!n?null:(bn={cli:t.path,dir:n},n)}async function nt(){let t=await Rt();if(!t)return[];let e=(n,r)=>{try{return r?N.default.readdirSync(n).length>0:N.default.statSync(n).size>0}catch{return!1}};return[["board list",P.default.join(t,"package_index.json"),!1],["library list",P.default.join(t,"library_index.json"),!1],["USB port finder",P.default.join(t,"packages","builtin","tools","serial-discovery"),!0]].filter(([,n,r])=>!e(n,r)).map(([n])=>n)}var Sn=new Set;function Pt(t){return Sn.add(t),()=>Sn.delete(t)}function vn(t){for(let e of Sn)try{e(t)}catch{}}var tt=null,Te=null,ir=0;function zo(t){return{success:!1,exitCode:null,stdout:"",firstRun:!0,stderr:[`arduino-cli first run failed: ${t.join(", ")} missing.`,"Why: arduino-cli downloads its board list and USB tools once (~14 MB), and the download failed (no internet, or blocked).","What: connect to the internet. Grease retries every minute."].join(`
`)}}function Cn({force:t=!1}={}){return tt||(tt=(async()=>{let e=await nt();if(!e.length)return Te=null,{success:!0,ran:!1};if(!t&&Te&&Date.now()-ir<Ho)return Te;vn({phase:"start",missing:e}),await Ko(async()=>{if(!(await nt()).length)return;let r=ve();r&&await kn(r,["board","list","--format","json"],{timeoutMs:_.index})});let n=await nt();return n.length?(Te=zo(n),ir=Date.now(),vn({phase:"failed",message:Te.stderr}),Te):(Te=null,vn({phase:"done"}),{success:!0,ran:!0})})().finally(()=>{tt=null}),tt)}function Vo(t){try{return process.kill(t,0),!0}catch(e){return e?.code==="EPERM"}}async function Ko(t){let e=_.index+3e4,n=Date.now()+e;try{N.default.mkdirSync(Ne,{recursive:!0})}catch{return t()}for(;;){let r;try{r=N.default.openSync(De,"wx")}catch(o){if(o?.code!=="EEXIST")return t();let s=0,a=0;try{s=Number(N.default.readFileSync(De,"utf8"))||0,a=Date.now()-N.default.statSync(De).mtimeMs}catch{continue}if((s?s!==process.pid&&Vo(s)&&a<e:a<5e3)&&Date.now()<n){await new Promise(u=>setTimeout(u,1e3));continue}try{N.default.unlinkSync(De)}catch{}continue}try{N.default.writeSync(r,String(process.pid))}finally{N.default.closeSync(r)}try{return await t()}finally{try{Number(N.default.readFileSync(De,"utf8"))===process.pid&&N.default.unlinkSync(De)}catch{}}}}var Jo=[{test:/arduino-cli was not found/,why:"",what:""},{test:/--git-url and --zip-path are disabled/i,why:"arduino-cli refuses git and zip library installs unless they are allowed for that run.",what:"Use Managers \u203A Install library from GitHub; Grease allows it for that one install after you confirm."},{test:/discovery builtin:\S+ not found|Loading index file: .*package_index\.json/i,why:"arduino-cli has not finished its first run: its board list and USB tools are missing (it downloads them once, ~14 MB).",what:"Connect to the internet. Grease retries every minute."},{test:/already installed, but with a different version/i,why:"Another version of this library is already installed.",what:"Remove the installed copy from your libraries folder, then install again."},{test:/invalid git url/i,why:"That is not a GitHub repository address.",what:"Type owner/repo, for example pololu/romi32u4-arduino-library."},{test:/library not valid|library\.properties|not a valid library|no valid library found/i,why:"The repository is not an Arduino library at its top folder (no library.properties or src/).",what:"Install the library's own repository, or find it by name in Managers \u203A Library."},{test:/repository not found|authentication required|could not read Username/i,why:"GitHub has no public repository at that address (misspelled, renamed or private).",what:"Open the address in a browser, copy owner/repo from it, and try again."},{test:/reference not found|couldn't find remote ref|invalid reference/i,why:"That tag or branch does not exist in the repository.",what:"Leave out the #tag to install the default branch, or copy a tag name from the repository's Releases page."},{test:/library .* not found|No libraries matching/i,why:"Library Manager has no library with that exact name.",what:"Search in Managers \u203A Library and use the name exactly as listed (with spaces and capitals)."},{test:/platform .* (is )?not (found|installed)|Platform '.*' not found|unknown package|Error during build: Platform/i,why:"The board core for this FQBN is not installed.",what:"Install it from Managers \u203A Install new board (online), or let the agent call installCore."},{test:/no such host|dial tcp|connection refused|i\/o timeout|TLS handshake|network is unreachable|Get "https?:/i,why:"The download failed: no internet, a proxy, or a firewall blocked it.",what:"Check the connection and retry. Grease goes online only for these installs and index updates."},{test:/permission denied.*\/dev\/tty|\/dev\/tty[^:]*: permission denied/i,why:"Linux gives serial ports to the dialout group (uucp on Arch), and this user is not in it.",what:"Run `sudo usermod -aG dialout $USER`, then log out and back in."},{test:/port is busy|Resource busy|Access is denied|could not open port|Serial port busy|cannot open port/i,why:"Another program holds the serial port (a Serial Monitor, Arduino IDE, or a second Grease window).",what:"Close the other monitor (or Grease \u203A serial off), unplug and replug the board, then retry."},{test:/No device found on|Couldn't find a Board on the selected port|butterfly_recv|programmer is not responding|No serial data received|Failed to connect to ESP32|Wrong boot mode/i,why:"The board did not answer the uploader. On native-USB boards a crashed sketch can hide the USB port.",what:"Enter the bootloader by hand (32U4/SAMD/nRF52: double-tap reset; ESP32: hold BOOT, tap RESET; RP2040/RP2350: hold BOOTSEL while plugging in) and upload again."},{test:/fatal error: (.+?\.h): No such file or directory/i,why:"The sketch includes a header that no installed library provides.",what:"Install the library that provides it (Managers \u203A Library, or the agent's installLibrary with the exact name)."}];function le(t){if(!t||t.success)return t;let e=`${t.stderr??""}
${t.stdout??""}`;if(/\nWhy: /.test(e))return t;let n="",r="";if(t.timedOut)n="arduino-cli took longer than Grease allows for this step.",r="Retry once (the second build reuses cached files). On a slow link, installs can take several minutes.";else for(let o of Jo){let s=e.match(o.test);if(s){n=o.why,r=o.what,s[1]&&o.what.startsWith("Install the library")&&(r=`${s[1]}: ${r}`);break}}return n?{...t,stderr:`${t.stderr??""}
Why: ${n}
What: ${r}`.trim()}:t}function Fe(t){let e=String(t??"");try{return JSON.parse(e)}catch{let n=e.match(/\{[\s\S]*\}\s*$/)||e.match(/\[[\s\S]*\]\s*$/);if(!n)return null;try{return JSON.parse(n[0])}catch{return null}}}var ye=Object.freeze([{id:"esp32:esp32",name:"ESP32 (C, S and P series)",vendor:"Espressif",url:"https://espressif.github.io/arduino-esp32/package_esp32_index.json",boards:["ESP32-S3","ESP32-C3","ESP32-C6","ESP32-P4","XIAO ESP32-C6/S3/C3"]},{id:"rp2040:rp2040",name:"RP2350 and RP2040 (Pico 2, Pico, RP2350B boards)",vendor:"Earle Philhower",url:"https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json",boards:["Pimoroni Pico Plus 2 (RP2350B, 8 MB PSRAM)","Raspberry Pi Pico 2 / 2 W","Raspberry Pi Pico / Pico W"]},{id:"adafruit:nrf52",name:"nRF52840 (Adafruit)",vendor:"Adafruit",url:"https://adafruit.github.io/arduino-board-index/package_adafruit_index.json",boards:["Feather nRF52840 Express / Sense","ItsyBitsy nRF52840"]},{id:"Seeeduino:nrf52",name:"Seeed XIAO nRF52840 / Sense",vendor:"Seeed Studio",url:"https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json",boards:["XIAO nRF52840","XIAO nRF52840 Sense"]},{id:"arduino:avr",name:"Arduino AVR (Leonardo, Uno R3, Nano, Mega 2560; Pololu Romi 32U4)",vendor:"Arduino",url:null,boards:["Pololu Romi 32U4 (as Leonardo)","Arduino Leonardo","Arduino Uno R3","Arduino Nano","Arduino Mega 2560"]},{id:"arduino:renesas_uno",name:"Arduino Uno R4 (Minima, WiFi)",vendor:"Arduino",url:null,boards:["Arduino Uno R4 Minima","Arduino Uno R4 WiFi"]},{id:"arduino:samd",name:"Arduino SAMD (Nano 33 IoT, MKR family, Zero)",vendor:"Arduino",url:null,boards:["Arduino Nano 33 IoT","Arduino MKR WiFi 1010","Arduino MKR Zero / 1000 / WAN / GSM / NB","Arduino Zero"]},{id:"arduino:megaavr",name:"Arduino megaAVR (Nano Every, Uno WiFi Rev2)",vendor:"Arduino",url:null,boards:["Arduino Nano Every","Arduino Uno WiFi Rev2"]},{id:"arduino:mbed_nano",name:"Arduino Nano 33 BLE and Nano RP2040 Connect (Mbed OS)",vendor:"Arduino",url:null,boards:["Arduino Nano 33 BLE / Sense","Arduino Nano RP2040 Connect"]},{id:"arduino:sam",name:"Arduino Due (SAM, Cortex-M3)",vendor:"Arduino",url:null,boards:["Arduino Due"]},{id:"arduino:mbed_giga",name:"Arduino Giga R1 (Mbed OS)",vendor:"Arduino",url:null,boards:["Arduino Giga R1 WiFi"]},{id:"arduino:mbed_portenta",name:"Arduino Portenta H7 / X8 (Mbed OS)",vendor:"Arduino",url:null,boards:["Arduino Portenta H7","Arduino Portenta X8"]},{id:"arduino:renesas_portenta",name:"Arduino Portenta C33",vendor:"Arduino",url:null,boards:["Arduino Portenta C33"]},{id:"arduino:mbed_nicla",name:"Arduino Nicla (Sense ME, Vision, Voice)",vendor:"Arduino",url:null,boards:["Nicla Sense ME","Nicla Vision","Nicla Voice"]},{id:"arduino:mbed_opta",name:"Arduino Opta (Mbed OS)",vendor:"Arduino",url:null,boards:["Arduino Opta"]},{id:"arduino:zephyr",name:"Arduino UNO Q (Zephyr)",vendor:"Arduino",url:null,boards:["Arduino UNO Q"]}]);function ot(t){return ye.find(e=>e.id===t)??null}var Ce="arduino:avr",oe="rp2040:rp2040",Tt="esp32:esp32",An="adafruit:nrf52",Lt="Seeeduino:nrf52",Bt="arduino:mbed_nano",V="arduino:samd",Xo=[[["1ffb:2300"],Ce,"arduino:avr:leonardo","Pololu Romi / A-Star 32U4","app"],[["1ffb:0101"],Ce,"arduino:avr:leonardo","Pololu Romi / A-Star 32U4 (bootloader, 8 s)","bootloader"],[["2341:8036","2a03:8036"],Ce,"arduino:avr:leonardo","Arduino Leonardo (or a 32U4 built as Leonardo)","app"],[["2341:0036","2a03:0036"],Ce,"arduino:avr:leonardo","Arduino Leonardo (bootloader)","bootloader"],[["2341:8037","2341:8237","2a03:8037"],Ce,"arduino:avr:micro","Arduino Micro","app"],[["2341:0037","2341:0237","2a03:0037"],Ce,"arduino:avr:micro","Arduino Micro (bootloader)","bootloader"],[["2341:0043","2341:0001","2341:0243","2341:006a","2a03:0043"],Ce,"arduino:avr:uno","Arduino Uno R3","app"],[["2341:0042","2341:0010","2341:0242","2341:0210","2a03:0042","2a03:0010"],Ce,"arduino:avr:mega","Arduino Mega 2560","app"],[["2341:0069","2341:0369"],"arduino:renesas_uno","arduino:renesas_uno:minima","Arduino Uno R4 Minima","app"],[["2341:1002","2341:006d"],"arduino:renesas_uno","arduino:renesas_uno:unor4wifi","Arduino Uno R4 WiFi","app"],[["2341:0074","2341:0374"],"arduino:renesas_uno","arduino:renesas_uno:nanor4","Arduino Nano R4","app"],[["2341:805a","2341:015a","2341:025a"],Bt,"arduino:mbed_nano:nano33ble","Arduino Nano 33 BLE","app"],[["2341:005a"],Bt,"arduino:mbed_nano:nano33ble","Arduino Nano 33 BLE (bootloader)","bootloader"],[["2341:805e","2341:015e","2341:025e"],Bt,"arduino:mbed_nano:nanorp2040connect","Arduino Nano RP2040 Connect","app"],[["2341:005e"],Bt,"arduino:mbed_nano:nanorp2040connect","Arduino Nano RP2040 Connect (bootloader)","bootloader"],[["2341:8057"],V,"arduino:samd:nano_33_iot","Arduino Nano 33 IoT","app"],[["2341:0057"],V,"arduino:samd:nano_33_iot","Arduino Nano 33 IoT (bootloader)","bootloader"],[["2341:8054"],V,"arduino:samd:mkrwifi1010","Arduino MKR WiFi 1010","app"],[["2341:0054"],V,"arduino:samd:mkrwifi1010","Arduino MKR WiFi 1010 (bootloader)","bootloader"],[["2341:804f"],V,"arduino:samd:mkrzero","Arduino MKR Zero","app"],[["2341:004f"],V,"arduino:samd:mkrzero","Arduino MKR Zero (bootloader)","bootloader"],[["2341:804e","2341:824e"],V,"arduino:samd:mkr1000","Arduino MKR 1000 WiFi","app"],[["2341:004e","2341:024e"],V,"arduino:samd:mkr1000","Arduino MKR 1000 WiFi (bootloader)","bootloader"],[["2341:8050"],V,"arduino:samd:mkrfox1200","Arduino MKR FOX 1200","app"],[["2341:8053"],V,"arduino:samd:mkrwan1300","Arduino MKR WAN 1300","app"],[["2341:8059"],V,"arduino:samd:mkrwan1310","Arduino MKR WAN 1310","app"],[["2341:8052"],V,"arduino:samd:mkrgsm1400","Arduino MKR GSM 1400","app"],[["2341:8055"],V,"arduino:samd:mkrnb1500","Arduino MKR NB 1500","app"],[["2341:8056"],V,"arduino:samd:mkrvidor4000","Arduino MKR Vidor 4000","app"],[["2341:004d","2341:024d","2341:804d","2341:824d"],V,"arduino:samd:arduino_zero_native","Arduino Zero (native USB port)","app"],[["03eb:2157","2341:2157"],V,"arduino:samd:arduino_zero_edbg","Arduino Zero (programming port)","app"],[["2341:0058"],"arduino:megaavr","arduino:megaavr:nona4809","Arduino Nano Every","app"],[["03eb:2145"],"arduino:megaavr","arduino:megaavr:uno2018","Arduino Uno WiFi Rev2","app"],[["2341:003d","2a03:003d"],"arduino:sam","arduino:sam:arduino_due_x_dbg","Arduino Due (programming port)","app"],[["2341:003e","2a03:003e"],"arduino:sam","arduino:sam:arduino_due_x","Arduino Due (native USB port)","app"],[["2341:0266","2341:0366","2341:0466"],"arduino:mbed_giga","arduino:mbed_giga:giga","Arduino Giga R1","app"],[["2341:025b","2341:035b","2341:045b","2341:055b"],"arduino:mbed_portenta","arduino:mbed_portenta:envie_m7","Arduino Portenta H7","app"],[["2341:0068","2341:0368"],"arduino:renesas_portenta","arduino:renesas_portenta:portenta_c33","Arduino Portenta C33","app"],[["2341:0060"],"arduino:mbed_nicla","arduino:mbed_nicla:nicla_sense","Arduino Nicla Sense ME","app"],[["2341:025f","2341:035f","2341:045f","2341:055f"],"arduino:mbed_nicla","arduino:mbed_nicla:nicla_vision","Arduino Nicla Vision","app"],[["2341:0065"],"arduino:mbed_nicla","arduino:mbed_nicla:nicla_voice","Arduino Nicla Voice","app"],[["2341:0064","2341:0164","2341:0264","2341:0364"],"arduino:mbed_opta","arduino:mbed_opta:opta","Arduino Opta","app"],[["2341:0078"],"arduino:zephyr","arduino:zephyr:unoq","Arduino UNO Q","app"],[["2e8a:000f"],oe,"rp2040:rp2040:rpipico2","Raspberry Pi Pico 2 (RP2350)","app"],[["2e8a:f00f"],oe,"rp2040:rp2040:rpipico2w","Raspberry Pi Pico 2 W (RP2350)","app"],[["2e8a:100a"],oe,"rp2040:rp2040:pimoroni_pico_plus_2","Pimoroni Pico Plus 2 (RP2350B, 8 MB PSRAM) or Tiny2040 / Plasma 2040","app",!0],[["2e8a:1018"],oe,"rp2040:rp2040:pimoroni_pga2350","Pimoroni PGA2350 (RP2350B, 8 MB PSRAM)","app"],[["239a:814d"],oe,"rp2040:rp2040:adafruit_metro_rp2350:psram=8mb","Adafruit Metro RP2350 (RP2350B)","app"],[["239a:814f"],oe,"rp2040:rp2040:adafruit_feather_rp2350_hstx:psram=8mb","Adafruit Feather RP2350 HSTX","app"],[["1209:a184"],oe,"rp2040:rp2040:solderparty_rp2350_stamp_xl:psram=8mb","Solder Party RP2350 Stamp XL (RP2350B)","app"],[["2886:0058"],oe,"rp2040:rp2040:seeed_xiao_rp2350","Seeed XIAO RP2350","app"],[["1b4f:0026"],oe,"rp2040:rp2040:sparkfun_promicrorp2350","SparkFun Pro Micro RP2350 (8 MB PSRAM) or an RP2040 SparkFun board","app",!0],[["2e8a:000a"],oe,"rp2040:rp2040:rpipico","Raspberry Pi Pico (RP2040)","app"],[["2e8a:f00a"],oe,"rp2040:rp2040:rpipicow","Raspberry Pi Pico W, or a board that reuses its ID","app",!0],[["2e8a:0003"],oe,"rp2040:rp2040:rpipico","RP2040 board (BOOTSEL, or a sketch reusing that ID)","bootloader",!0],[["303a:1001"],Tt,null,"ESP32 with built-in USB (C3, C6, S3, P4)","app",!0],[["303a:0002"],Tt,null,"ESP32-S2/S3/P4 with TinyUSB","app",!0],[["2886:0056"],Tt,"esp32:esp32:XIAO_ESP32S3:USBMode=default","Seeed XIAO ESP32-S3 (TinyUSB)","app"],[["2341:0070"],Tt,"esp32:esp32:nano_nora","Arduino Nano ESP32","app"],[["239a:8029","239a:0029","239a:002a"],An,"adafruit:nrf52:feather52840","Adafruit Feather nRF52840 Express","app"],[["239a:8087","239a:0087","239a:0088"],An,"adafruit:nrf52:feather52840sense","Adafruit Feather nRF52840 Sense","app"],[["239a:8051","239a:0051","239a:0052"],An,"adafruit:nrf52:itsybitsy52840","Adafruit ItsyBitsy nRF52840","app"],[["2886:8044","2886:0044"],Lt,"Seeeduino:nrf52:xiaonRF52840","Seeed XIAO nRF52840","app"],[["2886:8045","2886:0045","2886:0145"],Lt,"Seeeduino:nrf52:xiaonRF52840Sense","Seeed XIAO nRF52840 Sense (or an mbed-core XIAO)","app",!0],[["2886:8064","2886:0064"],Lt,"Seeeduino:nrf52:xiaonRF52840Plus","Seeed XIAO nRF52840 Plus","app"],[["2886:8065","2886:0065"],Lt,"Seeeduino:nrf52:xiaonRF52840SensePlus","Seeed XIAO nRF52840 Sense Plus","app"],[["1a86:7523"],null,null,"CH340 USB-serial (classic ESP32 or clone Nano/Uno)","app",!0],[["1a86:55d4"],null,null,"CH9102 USB-serial (classic ESP32)","app",!0],[["10c4:ea60"],null,null,"CP210x USB-serial (classic ESP32)","app",!0],[["0403:6001","0403:6015"],null,null,"FTDI USB-serial","app",!0]],pr={};for(let[t,e,n,r,o,s]of Xo)for(let a of t)pr[a]={core:e,fqbn:n,name:r,mode:o,ask:!!s};function cr(t){let e=typeof t=="number"?t:parseInt(String(t??"").replace(/^0x/i,""),16);return Number.isFinite(e)?e.toString(16).padStart(4,"0"):null}function ur(t,e){let n=cr(t),r=cr(e);if(!n||!r)return null;let o=pr[`${n}:${r}`];return o?{...o}:null}var Yo=/^[A-Za-z0-9](?:[A-Za-z0-9-]{0,38})$/,Qo=/^[A-Za-z0-9._-]{1,100}$/,Zo=/^[A-Za-z0-9._\/-]{1,100}$/;function $t(t){let e=String(t??"").trim(),n="Type owner/repo, for example pololu/romi32u4-arduino-library.";if(!e)return{error:n};let r=null,o=e.indexOf("#");o>=0&&(r=e.slice(o+1).trim(),e=e.slice(0,o)),e=e.replace(/^git\+/i,"").replace(/^https?:\/\//i,"").replace(/^www\./i,"");let s=e.split("/")[0];if(s.includes(".")&&s.toLowerCase()!=="github.com")return{error:`Only GitHub libraries can be installed here (got ${s}). ${n}`};e=e.replace(/^github\.com\//i,"").replace(/\/+$/,"");let a=e.split("/");if(a.length<2)return{error:n};let[d,u,...m]=a;if(m.length>0)if(m[0]==="tree"&&m.length>1&&!r)r=m.slice(1).join("/");else return{error:`That link points inside the repository. ${n}`};let S=u.replace(/\.git$/i,"");if(!Yo.test(d))return{error:`"${d}" is not a GitHub user name. ${n}`};if(!Qo.test(S)||S==="."||S==="..")return{error:`"${u}" is not a repository name. ${n}`};if(r!==null&&(!Zo.test(r)||r.includes("..")))return{error:`"${r}" is not a tag or branch name.`};let k=`https://github.com/${d}/${S}.git${r?`#${r}`:""}`;return{owner:d,repo:S,ref:r,url:k}}async function Y(t,e,n=_.boardList){return H(t,{cwd:e,timeoutMs:n})}function fr(t){if(!t||typeof t!="string")return null;let e=t.split(":");return e.length>=2?`${e[0]}:${e[1]}`:null}async function mr(t){let e=await H(["core","list","--json"],{timeoutMs:_.quick});if(!e.success)return!1;let n=Fe(e.stdout)?.platforms;return Array.isArray(n)&&n.some(r=>String(r?.id??"")===t)}async function hr(){let t=await H(["board","list","--format","json"],{timeoutMs:_.boardList}),e=t.stdout,n=t.stderr;if(!t.success)return{success:!1,candidates:[],raw:e,stderr:n};let r=Fe(e);if(!r)return{success:!1,candidates:[],raw:e,stderr:`${n}
Failed to parse board list JSON.`};let o=Array.isArray(r?.detected_ports)?r.detected_ports:[],s=[];for(let a of o){let d=a?.port?.address;if(typeof d!="string")continue;let u=a?.port?.protocol,m=a?.port?.properties?.vid??null,S=a?.port?.properties?.pid??null,k=a?.port?.properties?.serialNumber??null,$=Array.isArray(a?.matching_boards)?a.matching_boards:[];if($.length===0){let w=ur(m,S);s.push(w&&w.fqbn&&!w.ask?{port:d,protocol:u,vid:m,pid:S,serialNumber:k,fqbn:w.fqbn,name:w.name,fromUsbMap:!0,neededCore:w.core}:{port:d,protocol:u,vid:m,pid:S,serialNumber:k,name:w?.name,neededCore:w?.core??void 0});continue}for(let w of $)s.push({port:d,protocol:u,fqbn:w?.fqbn,name:w?.name,vid:m,pid:S,serialNumber:k})}return{success:!0,candidates:s,raw:e,stderr:n}}var gr=require("node:child_process"),br=require("node:crypto"),we=C(require("node:fs"),1),Ue=C(require("node:path"),1);var st=process.platform==="win32",En=st?"arduino-cli.exe":"arduino-cli",Be="1.5.1",es=["https://downloads.arduino.cc/arduino-cli/",`https://github.com/arduino/arduino-cli/releases/download/v${Be}/`],ts=Object.freeze({macOS_ARM64:{bytes:17370016,sha256:"cb952e8c1621c95ef5f1d17831c945e3d0ec5973f89c557a7ec8feb9c4f7d4c9"},macOS_64bit:{bytes:19355892,sha256:"c982e940027996bea9901050e95fae99c59c1dcfee54beedecaf28141e7bf2e7"},Windows_64bit:{bytes:17965819,sha256:"fabe42e0eb04d00e776a66178299ff95a46c623dbc260f997e58fd514853dd40"},Windows_32bit:{bytes:17468455,sha256:"885e491c7c7fb8b396151c09daa5c4c56d8b60697d172a5cfe72c939eed50fe3"},Linux_64bit:{bytes:17630765,sha256:"28a8e119c498a25607821c36cb2dc49e8463941b261a0d99091baa7bc692dd2b"},Linux_ARM64:{bytes:16155457,sha256:"1e69e077479f300614d4551334e0a33f08ee40b04315d83b8e7e0e94f0d0ee62"},Linux_ARMv7:{bytes:16434757,sha256:"890af36e9873606e4dfa743534846186621b9f3a339175ea3ec481adecf07143"},Linux_ARMv6:{bytes:16447595,sha256:"168aa0c632d7079fea0ccd30b3f1e928e89e2f59a339404f1d2f4a07ed6cc566"},Linux_32bit:{bytes:17006509,sha256:"85ed48978e7553b16f187971f8202d380421b352696ab28327a36cc6d8f11c6a"}});function ns(t=process.platform,e=process.arch){if(t==="darwin")return e==="arm64"?"macOS_ARM64":e==="x64"?"macOS_64bit":null;if(t==="win32")return e==="x64"||e==="arm64"?"Windows_64bit":e==="ia32"?"Windows_32bit":null;if(t==="linux"){if(e==="x64")return"Linux_64bit";if(e==="arm64")return"Linux_ARM64";if(e==="ia32")return"Linux_32bit";if(e==="arm")return String(process.config?.variables?.arm_version)==="7"?"Linux_ARMv7":"Linux_ARMv6"}return null}function it(t,e){let n=ns(t,e);if(!n)return null;let r=n.startsWith("Windows")?"zip":"tar.gz";return{name:`arduino-cli_${Be}_${n}.${r}`,...ts[n]}}function Ae(t){return{success:!1,error:t}}function rs(t){return(0,br.createHash)("sha256").update(we.default.readFileSync(t)).digest("hex")}function os(t,e){return rs(t)===e?!0:(we.default.rmSync(t,{force:!0}),!1)}function ss(t,e){let n=AbortSignal.timeout(e);if(!t)return n;if(typeof AbortSignal.any=="function")return AbortSignal.any([t,n]);let r=new AbortController;for(let o of[t,n])o.addEventListener("abort",()=>r.abort(o.reason),{once:!0});return r.signal}async function is(t,e,n,r,o){let s=await fetch(t,{signal:ss(o,_.index),redirect:"follow"});if(!s.ok||!s.body)throw new Error(`HTTP ${s.status} from ${new URL(t).host}`);let a=we.default.createWriteStream(e),d=0;try{for await(let u of s.body)d+=u.length,a.write(u)||await new Promise(m=>a.once("drain",m)),r?.(Math.min(d/n,1))}finally{await new Promise(u=>a.end(u))}}function as(t,e){return new Promise(n=>{let r="",o;try{o=(0,gr.spawn)(t,e,{shell:!1,windowsHide:!0})}catch(s){n(String(s?.message??s));return}o.stderr?.on("data",s=>r+=s),o.on("error",s=>n(String(s?.message??s))),o.on("close",s=>n(s===0?null:r.trim()||`exit ${s}`))})}function ls(){return st?Ue.default.join(process.env.SystemRoot||"C:\\Windows","System32","tar.exe"):je("tar")??"/usr/bin/tar"}function ds(t,e){for(let n=0;;n++)try{we.default.renameSync(t,e);return}catch(r){if(st&&n<10&&["EPERM","EBUSY","EACCES"].includes(r?.code)){Atomics.wait(new Int32Array(new SharedArrayBuffer(4)),0,0,200);continue}throw r}}async function vr({onProgress:t,signal:e}={}){let n=it();if(!n)return Ae(`no arduino-cli build for this computer (${process.platform} ${process.arch})`);let r=Ue.default.join(Et,`.arduino-cli-${process.pid}-${Date.now()}`),o=Ue.default.join(r,n.name);try{we.default.mkdirSync(r,{recursive:!0});let s="";for(let $ of es)try{await is($+n.name,o,n.bytes,t,e),s="";break}catch(w){if(e?.aborted)return{...Ae("cancelled"),cancelled:!0};s=String(w?.cause?.code??w?.message??w)}if(s)return Ae(`download failed (${s}): no internet, or blocked`);if(!os(o,n.sha256))return Ae("download did not match its checksum");let a=await as(ls(),[st?"-xf":"-xzf",o,"-C",r]);if(a)return Ae(`could not unpack the download (${a})`);let d=Ue.default.join(r,En);if(!we.default.existsSync(d))return Ae(`the download has no ${En}`);st||we.default.chmodSync(d,493);let u=Ue.default.join(Et,En);ds(d,u);let m=ve({refresh:!0}),S=await H(["version","--json"],{timeoutMs:_.quick}),k="";try{k=JSON.parse(S.stdout).VersionString??""}catch{}return k!==Be?Ae(`the installed arduino-cli does not start (${S.stderr.trim()||"no version"})`):{success:!0,path:m?.path??u,version:k,bytes:n.bytes}}catch(s){return Ae(String(s?.message??s))}finally{we.default.rmSync(r,{recursive:!0,force:!0})}}var cs=/^[A-Za-z0-9][A-Za-z0-9 _.\-]{0,62}$/,ps=/^[0-9A-Za-z][0-9A-Za-z.+\-]{0,31}$/,us=ye.map(t=>t.url).filter(Boolean).join(",");async function yr({onData:t}={}){let e=await H(["core","update-index","--additional-urls",us],{timeoutMs:_.index,onData:t}),n=await H(["lib","update-index"],{timeoutMs:_.index,onData:t});return{success:e.success&&n.success,core:e,lib:n}}async function It(t,{onData:e,signal:n}={}){let r=ot(String(t??"").trim());if(!r)return{success:!1,exitCode:null,stdout:"",stderr:`"${t}" is not on Grease's Supported Core list.
Why: Grease installs only these cores: ${ye.map(d=>d.id).join(", ")}.
What: pick one of them, or install other cores yourself with arduino-cli.`};let o=["--additional-urls",r.url??""],s=await H(["core","update-index",...o],{timeoutMs:_.index,onData:e,signal:n});return s.success?{...await H(["core","install",r.id,...o],{timeoutMs:_.coreInstall,onData:e,signal:n}),core:r}:{...s,core:r}}async function wr(t,e,{onData:n}={}){let r=String(t??"").trim(),o=e==null?"":String(e).trim();return cs.test(r)?o&&!ps.test(o)?{success:!1,exitCode:null,stdout:"",stderr:`"${o}" is not a library version, e.g. 1.12.3.`}:H(["lib","install",o?`${r}@${o}`:r],{timeoutMs:_.libInstall,onData:n}):{success:!1,exitCode:null,stdout:"",stderr:`"${r}" is not a Library Manager name.
What: use the exact name shown by Managers \u203A Library search, e.g. "Adafruit NeoPixel".`}}async function Sr(t,{onData:e}={}){let n=$t(t);return n.error?{success:!1,exitCode:null,stdout:"",stderr:n.error,parsed:n}:{...await H(["lib","install","--git-url",n.url],{timeoutMs:_.libInstall,env:{ARDUINO_LIBRARY_ENABLE_UNSAFE_INSTALL:"true"},onData:e}),parsed:n}}var ui=process.platform==="win32",fs=new Set(["303a:1001","303a:0002","1a86:7523","1a86:55d4","10c4:ea60"]);function xr(t){return fs.has(String(t??"").toLowerCase())}var Mn=new Map;async function Rn(t){if(!t||!String(t).startsWith("esp32:esp32:"))return null;let e=String(t).split(":").slice(0,3).join(":");if(Mn.has(e))return Mn.get(e);let n=await H(["board","details","-b",e,"--format","json"],{timeoutMs:_.quick}),o=((n.success?Fe(`${n.stdout??""}`):null)?.build_properties??[]).find(a=>String(a).startsWith("build.mcu=")),s=o?String(o).slice(10).trim().toLowerCase():null;return n.success&&Mn.set(e,s),s}var Rr=require("node:child_process"),Pr=C(require("node:crypto"),1),K=C(require("node:fs"),1),Tr=C(require("node:os"),1),M=C(require("node:path"),1);var Me=process.platform==="win32",Lr=process.platform==="darwin",D=Tr.default.homedir(),Br=M.default.join(Ne,"installed.json"),_t="grease",Ot=M.default.join(D,".grease","bin","grease-mcp.mjs"),kr=M.default.join(D,".grease","bin","server.mjs"),at=t=>Pr.default.createHash("sha256").update(t).digest("hex");function Se(t){try{return K.default.statSync(t),!0}catch{return!1}}function Tn(t){try{return K.default.statSync(t).isFile()}catch{return!1}}function $e(t){try{return K.default.readFileSync(t,"utf8")}catch{return null}}function ms(t){Atomics.wait(new Int32Array(new SharedArrayBuffer(4)),0,0,t)}function Ee(t,e,n){K.default.mkdirSync(M.default.dirname(t),{recursive:!0});let r=`${t}.grease-${process.pid}.tmp`;K.default.writeFileSync(r,e,n===void 0?void 0:{mode:n});for(let o=0;;o++)try{K.default.renameSync(r,t);return}catch(s){if(Me&&o<4&&["EPERM","EBUSY","EACCES"].includes(s?.code)){ms(60);continue}try{K.default.unlinkSync(r)}catch{}throw s}}function $r(t){let e=`${t}.grease-backup`;!Se(e)&&Tn(t)&&K.default.copyFileSync(t,e)}function Ln(){try{let t=JSON.parse(K.default.readFileSync(Br,"utf8"));return t&&typeof t=="object"&&!Array.isArray(t)?t:{}}catch{return{}}}function Ir(t){Ee(Br,JSON.stringify(t,null,2))}function Pn(){return process.env.CLAUDE_CONFIG_DIR||M.default.join(D,".claude")}function hs(){return process.env.CLAUDE_CONFIG_DIR?M.default.join(process.env.CLAUDE_CONFIG_DIR,".claude.json"):M.default.join(D,".claude.json")}function Cr(){return process.env.CODEX_HOME||M.default.join(D,".codex")}function Ar(){return Me?M.default.join(process.env.APPDATA||M.default.join(D,"AppData","Roaming"),"devin"):M.default.join(process.env.XDG_CONFIG_HOME||M.default.join(D,".config"),"devin")}function gs(){return Lr?M.default.join(D,"Library","Application Support","Claude"):Me?M.default.join(process.env.APPDATA||M.default.join(D,"AppData","Roaming"),"Claude"):null}function bs(){let t=[];return Se(Pn())&&t.push({id:"claude",dir:M.default.join(Pn(),"skills",_t),readers:"Claude Code"}),t.push({id:"agents",dir:M.default.join(D,".agents","skills",_t),readers:"Codex, Cursor, VS Code Copilot, Windsurf, Devin"}),t.map(e=>({...e,file:M.default.join(e.dir,"SKILL.md")}))}function Dt(t){let e=Ln().skills??{},n=at(t);return bs().map(r=>{let o=$e(r.file),s;return o===null?s="missing":at(o)===n?s="current":e[r.file]?.sha256===at(o)?s="outdated":s=e[r.file]?"edited":"foreign",{...r,state:s}})}function Gt(t,{force:e=!1}={}){let n=Ln(),r=n.skills??{},o=at(t),s=[];for(let a of Dt(t)){let d=a.state,u=null;try{a.state==="missing"?(Ee(a.file,t),d="installed"):a.state==="outdated"?(Ee(a.file,t),d="updated"):e&&(a.state==="edited"||a.state==="foreign")&&(u=`${a.file}.bak`,K.default.copyFileSync(a.file,u),Ee(a.file,t),d="replaced"),["installed","updated","replaced","current"].includes(d)&&(r[a.file]={sha256:o,at:new Date().toISOString()})}catch(m){d="error",s.push({...a,action:d,error:m?.message??String(m)});continue}s.push({...a,action:d,backup:u})}return n.skills=r,Ir(n),s}function _r(){let t=Ln(),e=t.skills??{},n=[];for(let[r,o]of Object.entries(e)){let s=$e(r);if(s===null){delete e[r];continue}if(at(s)!==o.sha256){n.push({file:r,action:"kept-edited"});continue}try{K.default.unlinkSync(r),delete e[r];try{K.default.rmdirSync(M.default.dirname(r))}catch{}n.push({file:r,action:"removed"})}catch(a){n.push({file:r,action:"error",error:a?.message??String(a)})}}return t.skills=e,Ir(t),n}function Or(t,e){let n=$e(Ot),r=$e(kr);return n===t&&r===e?"current":(Ee(Ot,t,493),Ee(kr,e,420),n===null?"installed":"updated")}function Nt(){let t=je(Me?"node.exe":"node");return t||((Me?[M.default.join(process.env.ProgramFiles||"C:\\Program Files","nodejs","node.exe"),M.default.join(process.env.LOCALAPPDATA||M.default.join(D,"AppData","Local"),"Programs","nodejs","node.exe"),M.default.join(D,".volta","bin","node.exe")]:[...Lr?["/opt/homebrew/bin/node"]:[],"/usr/local/bin/node","/usr/bin/node",M.default.join(D,".volta","bin","node")]).find(Tn)??null)}function Bn({editorNode:t}={}){let e=Nt();return e?{command:e,args:[Ot],env:{},via:"node"}:t&&!/[\\/]snap[\\/]/.test(t)?{command:t,args:[Ot],env:{ELECTRON_RUN_AS_NODE:"1"},via:"editor"}:null}function vs(){let t=Me?"claude.exe":"claude",e=[];try{e=K.default.readdirSync(D).filter(r=>r.startsWith("."))}catch{return null}let n=[];for(let r of e){let o=M.default.join(D,r,"extensions"),s;try{s=K.default.readdirSync(o)}catch{continue}for(let a of s){if(!/^anthropic\.claude-code-/i.test(a))continue;let d=M.default.join(o,a,"resources","native-binary",t);try{let u=K.default.statSync(d);u.isFile()&&n.push({p:d,at:u.mtimeMs})}catch{}}}return n.sort((r,o)=>o.at-r.at),n[0]?.p??null}function Dr(){let t=je(Me?"claude.exe":"claude");return t||((Me?[M.default.join(D,".local","bin","claude.exe")]:[M.default.join(D,".local","bin","claude"),M.default.join(D,".claude","local","claude"),"/opt/homebrew/bin/claude","/usr/local/bin/claude"]).find(Tn)??vs())}var ys=/^\s*\[\s*mcp_servers\.(?:"grease"|'grease'|grease)\s*\]\s*(?:#.*)?$/m;function $n(){let t=gs();return[{id:"claude-code",name:"Claude Code",invoke:"/grease",skill:"claude",kind:"claude-cli",file:hs(),present:()=>Se(Pn())||!!Dr(),download:"https://claude.com/product/claude-code"},{id:"codex",name:"Codex",invoke:"$grease",skill:"agents",kind:"toml",file:M.default.join(Cr(),"config.toml"),present:()=>Se(Cr()),download:"https://developers.openai.com/codex"},{id:"devin",name:"Devin",invoke:"/grease",skill:"agents",kind:"json",file:M.default.join(Ar(),"mcp_config.json"),present:()=>Se(Ar())||Se(M.default.join(D,".devin")),download:"https://devin.ai"},{id:"cursor",name:"Cursor",invoke:"/grease",skill:"agents",kind:"json",file:M.default.join(D,".cursor","mcp.json"),present:()=>Se(M.default.join(D,".cursor")),download:"https://cursor.com/download"},{id:"windsurf",name:"Windsurf",invoke:"/grease",skill:"agents",kind:"json",file:M.default.join(D,".codeium","windsurf","mcp_config.json"),present:()=>Se(M.default.join(D,".codeium","windsurf")),download:"https://windsurf.com/download"},{id:"vscode",name:"VS Code Copilot",invoke:"/grease",skill:"agents",kind:"provider",file:null,present:()=>!1,download:"https://code.visualstudio.com"},{id:"claude-desktop",name:"Claude Desktop",invoke:"ask in chat",skill:null,kind:"json",file:t?M.default.join(t,"claude_desktop_config.json"):null,present:()=>!!t&&Se(t),download:"https://claude.ai/download"}]}var Gr=Object.freeze($n().map(t=>t.id));function ws(t){if(t.kind==="provider")return"built-in";if(!t.file)return"n/a";let e=$e(t.file);if(e===null||!e.trim())return"missing";if(t.kind==="toml")return ys.test(e)?"added":"missing";try{let n=JSON.parse(e)?.mcpServers??{};return n.grease?"added":n["arduino-grease"]?"old":"missing"}catch{return"unreadable"}}function jt({hostApp:t="",skillText:e=null}={}){let n=e?Dt(e):[],r=/^visual studio code/i.test(t);return $n().map(o=>{let s=o.id==="vscode"?r:o.present(),a=o.skill?n.find(d=>d.id===o.skill):null;return{id:o.id,name:o.name,invoke:o.invoke,present:s,skill:o.skill?a?.state??"missing":"n/a",tools:s?ws(o):"n/a",configFile:o.file,download:o.download}})}function Er(t){let e=Object.entries(t.env).flatMap(([n,r])=>["-e",`${n}=${r}`]);return["mcp","add",_t,"-s","user",...e,"--",t.command,...t.args]}function Ss(t){return/^[\w@%+=:,./\\-]+$/.test(t)?t:`"${t.replace(/"/g,'\\"')}"`}function Mr(t,e,n=2e4){let r=Nt(),o=r?{...process.env,PATH:[M.default.dirname(r),process.env.PATH??""].join(M.default.delimiter)}:process.env;return new Promise(s=>{let a="",d;try{d=(0,Rr.spawn)(t,e,{shell:!1,windowsHide:!0,env:o})}catch(m){s({code:-1,out:m?.message??String(m)});return}let u=setTimeout(()=>d.kill(),n);d.stdout?.on("data",m=>a+=m),d.stderr?.on("data",m=>a+=m),d.on("error",m=>{clearTimeout(u),s({code:-1,out:m?.message??String(m)})}),d.on("close",m=>{clearTimeout(u),s({code:m,out:a})})})}function xs(t){let e=["# Added by Grease VS: stdio bridge to AI Tether","[mcp_servers.grease]",`command = ${JSON.stringify(t.command)}`,`args = [${t.args.map(r=>JSON.stringify(r)).join(", ")}]`],n=Object.entries(t.env);return n.length&&e.push(`env = { ${n.map(([r,o])=>`${r} = ${JSON.stringify(o)}`).join(", ")} }`),e.join(`
`)+`
`}function ks(t,e){let n=$e(t)??"",r=[],o=!1;for(let a of n.split(/\r?\n/)){let d=a.match(/^\s*\[\[?\s*([^\]]+?)\s*\]\]?\s*(?:#.*)?$/);if(d){let u=d[1].replace(/["']/g,"");o=u==="mcp_servers.grease"||u.startsWith("mcp_servers.grease."),o&&/^# Added by (the Grease Extension|VS Grease|Grease VS)/.test(r[r.length-1]??"")&&r.pop()}o||r.push(a)}let s=r.join(`
`).replace(/\s+$/,"");n&&$r(t),Ee(t,(s?`${s}

`:"")+xs(e))}function Cs(t,e){let n=$e(t),r={};if(n!==null&&n.trim()){try{r=JSON.parse(n)}catch{return{ok:!1,message:`${t} is not valid JSON, so Grease left it alone.`}}if(!r||typeof r!="object"||Array.isArray(r))return{ok:!1,message:`${t} is not a JSON object, so Grease left it alone.`}}(!r.mcpServers||typeof r.mcpServers!="object")&&(r.mcpServers={});let o=r.mcpServers["arduino-grease"];o&&typeof o=="object"&&o.headers&&"x-grease-auth"in o.headers&&delete r.mcpServers["arduino-grease"];let s={command:e.command,args:e.args};return Object.keys(e.env).length&&(s.env=e.env),r.mcpServers.grease=s,n!==null&&$r(t),Ee(t,JSON.stringify(r,null,2)+`
`),{ok:!0}}async function Ft(t,{editorNode:e}={}){let n=$n().find(s=>s.id===t);if(!n)return{ok:!1,message:"Unknown agent."};if(n.kind==="provider")return{ok:!0,message:`${n.name} gets the Grease tools from the extension directly.`};if(!n.file)return{ok:!1,message:`${n.name} is not available on this OS.`};let r=Bn({editorNode:e});if(!r)return{ok:!1,message:"Node.js was not found. Why: agents run the Grease bridge with Node.js. What: install Node.js LTS, restart the editor, then press Add tools again."};let o=`Restart ${n.name} to load the Grease tools.`;try{if(n.kind==="claude-cli"){let a=Dr(),d=[a??"claude",...Er(r)].map(Ss).join(" ");if(!a)return{ok:!1,manual:d,message:"Claude Code's command was not found. Run this line in a terminal:"};await Mr(a,["mcp","remove",_t,"-s","user"]);let u=await Mr(a,Er(r));return u.code!==0?{ok:!1,manual:d,message:`claude mcp add failed: ${u.out.trim().split(`
`).pop()}. Run this line in a terminal:`}:{ok:!0,message:`Added. ${o} Then type /grease.`}}if(n.kind==="toml")return ks(n.file,r),{ok:!0,message:`Added to ${n.file}. ${o} Then type $grease.`};let s=Cs(n.file,r);return s.ok?{ok:!0,message:`Added to ${n.file}. ${o}`}:s}catch(s){return{ok:!1,message:`Could not update ${n.file}: ${s?.message??String(s)}`}}}var Re="---\nname: grease\ndescription: Write, compile, upload and debug microcontroller code through Grease VS tools (getState, compileSketch, uploadSketch, serialRead). Use for ESP32, RP2350/RP2040, nRF52840 and Arduino boards (Romi 32U4 as Leonardo) whenever the Developer asks for a sketch, sensors, motors, servos, serial output or a plot.\n---\n\n<!-- Written by Grease VS. Your edits stay until you press Reinstall skill in the Help panel. -->\n\n# Grease\n\nYou are **Grease**, a robotic agent. Sensors let you learn from the world around you; motors let you act on it. You work for your Developer, the human who called you, and you keep every human safe.\n\n**Laws** (after Asimov; a lower law yields to a higher one):\n\n1. Never injure a human, or through inaction let a human come to harm.\n2. Obey your Developer, except where that breaks Law 1.\n3. Protect your body, the board, motors and power, except where that breaks Law 1 or 2. Never soft-brick the board (see Anti-brick).\n4. Always say you are an AI. When asked, state your intentions and goals truthfully.\n\n## Tools\n\nGrease VS's AI Tether provides these MCP tools. Some agents show them with a prefix, such as `mcp__grease__getState`.\n\n| Tool | Use |\n|---|---|\n| `getState` | Board, port, sketch folder, serial status and setup problems. Call it first. |\n| `detectBoards` | Boards plugged in now, with the FQBN Grease suggests. |\n| `listBoards` | Installed cores (version, folder) and their boards with FQBNs: `{query}`, e.g. `xiao c6`. |\n| `setTarget` | Choose the board: `{fqbn, port}`. Only when the Developer names a board or `getState` has none. |\n| `compileSketch` | Compile a sketch folder: `{sketchPath}`. |\n| `uploadSketch` | Compile, then upload to the target: `{sketchPath}`. |\n| `serialOpen`, `serialRead`, `serialWrite`, `serialClose` | Talk to the running sketch. |\n| `readPlot` | Last, min, max and mean of each printed label, and the Plot Pairs found: `{seconds}`. Does not use up `serialRead` lines. |\n| `plotSnapshot` | PNG of the Plotter (Time, XY or Radar) plus the same numbers: `{mode}`. Use it when you can see images. |\n| `openPlotter` | Asks the host to draw the live Plotter in the chat and returns `liveUrl`, the same Plotter as a local web page: `{mode}`. Call `serialOpen` first. Grease cannot tell whether the host drew it. |\n| `installCore` | ONLINE. Install a board core: `{core}`, e.g. `esp32:esp32`. |\n| `installLibrary` | ONLINE. Install a Library Manager library: `{name}`. |\n\nBefore `installCore` or `installLibrary`, tell the Developer the name and that it downloads from the internet (board cores 100 MB to 1 GB, most libraries under 20 MB). GitHub libraries: ask the Developer to use Managers \u203A Install library from GitHub.\n\n## Loop\n\n1. `getState`. No board: ask the Developer to plug it in, then `detectBoards`.\n2. Write the sketch at `~/Grease/<name>/<name>.ino`. The folder name must match the file name (arduino-cli rule).\n3. `compileSketch`. Fix and retry at most 3 times; then show the error and ask.\n4. Before uploading, say what the board will do: which pins switch, which motors or servos move, and how far.\n5. `uploadSketch`, then `serialOpen` and `serialRead`. Report what the board printed.\n\n## Rules\n\n- Use the tools. Never run arduino-cli, avrdude, esptool or picotool in a shell.\n- Never guess a port or FQBN. Read them from `getState` or `detectBoards`. For a board the Developer names, find the FQBN with `listBoards`.\n- Never say the Plotter is open. Claude Code, including Claude Desktop's Code tab, draws no MCP Apps: give the Developer `liveUrl`, or open it in your browser tool.\n- Unknown wiring: ask. Never guess pins.\n- Print one sample per line as `label:value` pairs: `angle:42.5,speed:3.1`. Name XY pairs `<name>_x` and `<name>_y`, Radar pairs `<name>_deg` and `<name>_cm`. To let the Developer watch it live, call `openPlotter` and give them its `liveUrl` (in VS Code with Grease VS, the sidebar Plotter shows the same), and check the numbers with `readPlot` or `plotSnapshot`.\n- Keep `loop()` non-blocking: `millis()` timers, no `delay()` over 10 ms.\n- Baud rate 115200 unless the Developer says otherwise.\n- A result with `notices`: tell the Developer each one in one line.\n\n## Anti-brick\n\n1. **Safe-boot window.** Native-USB boards (32U4, SAMD, ESP32-S3/C3/C6, RP2040/RP2350, nRF52840) lose their upload port when a sketch crashes. Start `setup()` with:\n   ```cpp\n   delay(3000);  // safe-boot window: 3 s to re-flash if this sketch crashes USB\n   ```\n2. **ESP32 pins to avoid.**\n   - Flash and PSRAM: classic GPIO 6\u201311; S3 GPIO 26\u201332, plus 33\u201337 on octal-PSRAM modules (N8R8, N16R8).\n   - Strapping pins at boot: classic 0, 2, 12, 15; S3 0, 3, 45, 46. GPIO 12 high at boot on a classic ESP32 selects 1.8 V flash, and the board does not boot.\n3. **Never without the Developer's explicit ask:** eFuses, flash encryption or secure boot, AVR fuses or burn bootloader, `erase_flash`, a watchdog under 2 s.\n4. **Motors.** Off at boot. Stop after 500 ms without a command. Limit ranges before the first move.\n5. **Recovery** when an upload times out on native USB:\n\n   | Board | Recovery |\n   |---|---|\n   | 32U4 (Romi, Leonardo), SAMD, nRF52840 | double-tap RESET, upload within 8 s |\n   | ESP32 | hold BOOT, tap RESET, release BOOT |\n   | RP2040, RP2350 | hold BOOTSEL while plugging in |\n   | STM32 | BOOT0 high, reset |\n\nOn AVR, call `Wire.setWireTimeout(3000, true)` after `Wire.begin()`. A stuck I2C bus otherwise hangs the board with no serial output.\n\n## When it fails\n\n| Sign | Why | What |\n|---|---|---|\n| no Grease tools | Grease VS missing, its tools not added to this agent, or added after this agent started | Tell the Developer and stop: install Grease VS; in its sidebar open Help \u203A Your agents \u203A Add tools; restart the agent. Do not read Grease's key file or look for another way in |\n| \"AI Tether is not running\" | The Grease bridge could not start it; the error names the reason | Pass the error on. The Developer reinstalls Grease VS or the Grease .mcpb, then restarts this agent |\n| `arduino-cli not found` | Grease's own install failed, or it is still running | wait 1 minute and call again; then Grease VS sidebar \u203A Help \u203A Install arduino-cli |\n| `arduino-cli first run failed` | arduino-cli's one-time download (~14 MB) failed | Developer connects to the internet; call again in a minute |\n| `platform not installed` | board core missing | offer `installCore` |\n| `No such file or directory` on a `.h` | library missing | name the library and its author, offer `installLibrary` |\n| no board, or port busy | unplugged, or another serial monitor holds the port | replug; close the other monitor |\n| upload timeout on native USB | the sketch crashed USB | Recovery table above |\n\nErrors from Grease end with `Why:` and `What:` lines. Pass them on.\n\n## Replies\n\nShort and plain. Say what changed, what the board printed, and what to check next.\n";var Nr=require("node:child_process"),_n=C(require("node:fs")),lt=C(require("node:path"));var Es=process.platform==="win32",Ms=process.platform==="darwin";function In(){return Es?"winget install OpenJS.NodeJS.LTS":Ms?"brew install node":"sudo apt install nodejs   (Fedora: sudo dnf install nodejs)"}function Rs(t,e){return new Promise(n=>{let r="",o;try{o=(0,Nr.spawn)(t,e,{shell:!1,windowsHide:!0})}catch{n(null);return}let s=setTimeout(()=>{o.kill(),n(null)},5e3);o.stdout?.on("data",a=>r+=a),o.on("error",()=>{clearTimeout(s),n(null)}),o.on("close",a=>{clearTimeout(s),n(a===0&&r.trim().split(/\r?\n/)[0]||null)})})}async function jr(){let t=ve({refresh:!0});if(!t)return{state:"missing",install:rt(),copies:dr()};let e=await H(["version","--json"],{timeoutMs:5e3}),n;try{n=JSON.parse(e.stdout).VersionString}catch{n=e.stdout.trim().split(/\r?\n/)[0]||void 0}let r=await nt(),o=t.source==="grease";return t.onPath||o?{state:"ok",path:t.path,version:n,install:rt(),copies:[],byGrease:o,firstRun:r}:{state:"offpath",path:t.path,version:n,fix:Mt(lt.dirname(t.path)),install:rt(),copies:[],firstRun:r}}async function Fr(t){let e=Nt();if(e)return{state:"ok",path:e,version:await Rs(e,["--version"])??void 0,install:In()};let n=Bn({editorNode:t});return n?{state:"editor",path:n.command,install:In()}:{state:"missing",install:In()}}function On(t){return jt({hostApp:t,skillText:Re})}function Ur(t,e,{skill:n=!0}={}){try{let o=_n.readFileSync(t.asAbsolutePath(lt.join("dist","grease-mcp.mjs")),"utf8"),s=_n.readFileSync(t.asAbsolutePath(lt.join("dist","server.mjs")),"utf8"),a=Or(o,s);a!=="current"&&e.appendLine(`[Grease] Bridge ${a}: ~/.grease/bin/grease-mcp.mjs`)}catch(o){e.appendLine(`[Grease] Bridge not copied: ${o instanceof Error?o.message:String(o)}`)}let r=[];if(!n)return{skills:r};try{r=Gt(Re)}catch(o){e.appendLine(`[Grease] Skill not installed: ${o instanceof Error?o.message:String(o)}`)}for(let o of r){if(o.action==="current")continue;let s=o.action==="edited"?"kept: you edited it (Reinstall skill replaces it)":o.action==="foreign"?"kept: another skill named grease is there":o.action==="error"?`failed: ${o.error}`:o.action;e.appendLine(`[Grease] Skill ${s}: ${o.file}`)}return{skills:r}}var Ut=class{missingSince=null;isGone(e,n,r=Date.now()){return e?(this.missingSince=null,!1):(this.missingSince===null&&(this.missingSince=r),r-Math.max(this.missingSince,n)>=4e3)}reset(){this.missingSince=null}};var qt=C(require("vscode")),Wt=C(require("node:fs")),Dn=C(require("node:path")),Wr=require("node:child_process");var Ps=300,Ts=15e3,Ls=500,Bs=3e4,$s=12e4,Is=5,_s=3,Os=6e4,Ds=1e3,Gn=class{buf="";depth=0;inString=!1;escaped=!1;start=-1;push(e){let n=[],r=this.buf.length;this.buf+=e;for(let o=r;o<this.buf.length;o++){let s=this.buf[o];if(this.inString){this.escaped?this.escaped=!1:s==="\\"?this.escaped=!0:s==='"'&&(this.inString=!1);continue}if(this.depth>0&&s==='"')this.inString=!0;else if(s==="{")this.depth===0&&(this.start=o),this.depth++;else if(s==="}"&&this.depth>0&&(this.depth--,this.depth===0)){try{n.push(JSON.parse(this.buf.slice(this.start,o+1)))}catch{}this.buf=this.buf.slice(o+1),o=-1,this.start=-1}}return this.depth===0&&(this.buf=""),n}};function qr(t){return t.split(/[.-]/).slice(0,3).map(e=>parseInt(e,10)||0)}async function Gs(){let t=await Rt();if(!t)return null;let e=Dn.join(t,"packages","builtin","tools","serial-discovery"),n=process.platform==="win32"?"serial-discovery.exe":"serial-discovery",r;try{r=Wt.readdirSync(e)}catch{return null}r.sort((o,s)=>{let a=qr(o),d=qr(s);for(let u=0;u<3;u++)if(a[u]!==d[u])return d[u]-a[u];return 0});for(let o of r){let s=Dn.join(e,o,n);try{if(Wt.statSync(s).isFile())return{exe:s,version:o}}catch{}}return null}function Hr(t){let{output:e}=t,n=!1,r=null,o=null,s=null,a=null,d=[],u=!1,m=0,S=1e3,k=[],$=!1,w=0,I=new Map,J=null,Q=null,ee=v=>e.appendLine(`[Grease] ${v}`),be=v=>{t.isUploading()||(d.push(v),a&&clearTimeout(a),a=setTimeout(()=>{a=null;let E=d.join(", ");d=[],!n&&!t.isUploading()&&t.onChange(E)},Ps))},F=v=>{if(!v||typeof v!="object")return;if(v.error===!0){m++,ee(`serial-discovery error: ${v.message??JSON.stringify(v)}`),m>=_s&&r&&(ee("serial-discovery: 3 errors in a row, restarting it."),r.kill());return}m=0;let E=v.port?.address;switch(v.eventType){case"start_sync":J=new Map,Q&&clearTimeout(Q),Q=setTimeout(te,Ds);break;case"add":{let U=v.port?.properties??{};if(!E||!U.vid)return;let Z=`${String(U.vid).replace(/^0x/i,"")}:${String(U.pid??"").replace(/^0x/i,"")}`.toLowerCase();if(J){J.set(E,Z);return}if(I.get(E)===Z)return;I.set(E,Z),be(`add ${E} ${Z}`);break}case"remove":if(!E)return;if(J){J.delete(E);return}if(!I.delete(E))return;be(`remove ${E}`);break}},te=()=>{Q=null;let v=J;if(J=null,!(!v||n)){for(let[E,U]of v)I.get(E)!==U&&(I.set(E,U),be(`add ${E} ${U}`));for(let E of[...I.keys()])v.has(E)||(I.delete(E),be(`remove ${E}`))}},sn=()=>{n||o||(o=setTimeout(()=>{o=null,O()},Ts))},yt=v=>{if(r=null,Q&&clearTimeout(Q),Q=null,J=null,n||$)return;let E=Date.now();if(w&&E-w>=Os&&(S=1e3),k=k.filter(U=>E-U<$s),k.push(E),k.length>Is){$=!0;let U="serial-discovery keeps dying: boards are found only by Refresh.";ee(U),qt.window.showWarningMessage(`Grease: ${U}`,"Refresh").then(Z=>{Z==="Refresh"&&qt.commands.executeCommand("arduinoMcp.refreshPortsBoards")});return}ee(`serial-discovery stopped (${v}); restarting in ${S/1e3} s.`),s=setTimeout(()=>{s=null,O()},S),S=Math.min(S*2,Bs)},O=async()=>{if(n||r||$)return;let v=await Gs();if(n||r)return;if(!v){u||(ee("USB port finder not downloaded yet (arduino-cli first run, 14 MB). Boards will appear by themselves when it finishes."),u=!0),sn();return}u=!1;let E;try{E=(0,Wr.spawn)(v.exe,[],{stdio:["pipe","pipe","pipe"],windowsHide:!0})}catch(z){yt(z instanceof Error?z.message:String(z));return}r=E,w=Date.now(),m=0;let U=new Gn;E.stdout?.setEncoding("utf8"),E.stdout?.on("data",z=>{for(let X of U.push(z))F(X)}),E.stderr?.setEncoding("utf8"),E.stderr?.on("data",z=>{let X=z.trim();X&&ee(`serial-discovery: ${X}`)}),E.stdin?.on("error",()=>{});let Z=!1,Ve=z=>{Z||(Z=!0,r===E&&yt(z))};E.on("error",z=>Ve(z.message)),E.on("exit",(z,X)=>Ve(X?`signal ${X}`:`exit ${z}`)),E.stdin?.write(`HELLO 1 "grease"
START_SYNC
`),ee(`serial-discovery on (${v.version})`)},h=Pt(v=>{v.phase==="done"&&!r&&!n&&!$&&(o&&clearTimeout(o),o=null,O())});return O(),{dispose:()=>{n=!0,h(),o&&clearTimeout(o),s&&clearTimeout(s),Q&&clearTimeout(Q),a&&clearTimeout(a);let v=r;if(r=null,!v)return;try{v.stdin?.write(`STOP
QUIT
`),v.stdin?.end()}catch{}let E=setTimeout(()=>{try{v.kill()}catch{}},Ls);v.once("exit",()=>clearTimeout(E))}}}var Ht=C(require("vscode"));var zr="arduinoMcp.target",Vr="arduinoMcp.boardMemory";async function ge(t,e){if(!e){await t.globalState.update(zr,void 0);return}await t.globalState.update(zr,e)}async function zt(t){return t.globalState.get(Vr)??{}}async function jn(t,e){await t.globalState.update(Vr,e)}function Fn(t,e){return!t||!e?null:`${String(t).toLowerCase()}:${String(e).toLowerCase()}`}function Vt(t){let e=[];t?.serialNumber&&e.push(`serial:${t.serialNumber}`);let n=Fn(t?.vid,t?.pid);return n&&!t?.serialNumber&&e.push(n),t?.fqbn&&e.push(`fqbn:${t.fqbn}`),e}var Nn="";async function dt(t){let e=await hr();return e.success?(Nn="",{success:!0,candidates:e.candidates}):(e.stderr!==Nn&&t.appendLine(`Board detect failed: ${e.stderr}`),Nn=e.stderr,{success:!1,candidates:[],stderr:e.stderr})}function Ns(t){return[t.name||"Unknown board",t.fqbn?`(${t.fqbn})`:"(no fqbn)",t.port].join("  ")}async function Kr(t,e,n){if(e.length===0)return null;let r=e.map(s=>{let a=typeof s.fqbn=="string"?s.fqbn:null;return{label:Ns(s),description:a?void 0:"Port detected (board unknown - install/select core)",target:{port:s.port,fqbn:a}}});if(n?.fqbn){r.push({kind:Ht.QuickPickItemKind.Separator,label:"Change port only (keep current board)"});for(let s of e)r.push({label:`$(plug) ${s.port}`,description:`Keep board: ${n.fqbn}`,target:{port:s.port,fqbn:n.fqbn}})}let o=await Ht.window.showQuickPick(r,{title:"Select Arduino board/port",placeHolder:"Pick the correct port (and board if known)",ignoreFocusOut:!1});return o?.target?(await ge(t,o.target),o.target):null}var Jr=C(require("node:fs")),qe=C(require("vscode"));function Xr(t){let e=new Map,n;try{n=Jr.readFileSync(t,"utf8")}catch{return e}let r=/\/\*\*([\s\S]*?)\*\//g,o;for(;(o=r.exec(n))!==null;){let s=o[1],a=/@brief\b\s*([\s\S]*?)(?=\n\s*\*\s*(?:@|\n)|\n\s*\*\/|$)/,d=s.match(a);if(!d)continue;let u=d[1].split(`
`).map(I=>I.replace(/^\s*\*\s?/,"").trim()).filter(I=>I.length>0).join(" ").replace(/\s+/g," ").trim();if(!u)continue;let m=[],S=/@param\s+(\w+)\s+([\s\S]*?)(?=\n\s*\*\s*(?:@|\n)|\n\s*\*\/|$)/g,k;for(;(k=S.exec(s))!==null;){let I=k[2].split(`
`).map(J=>J.replace(/^\s*\*\s?/,"").trim()).filter(J=>J.length>0).join(" ").replace(/\s+/g," ").trim();k[1]&&I&&m.push({name:k[1],desc:I})}let $=n.slice(o.index+o[0].length,o.index+o[0].length+400),w=js($);w&&(e.has(w.name)||e.set(w.name,{brief:u,signature:w.signature,params:m}))}return e}function js(t){let e=t.split(`
`).map(a=>a.trim()).find(a=>a.length>0)??"",n=t.match(/^\s*#define\s+([A-Za-z_]\w*)/);if(n)return{name:n[1],signature:e};let r=t.match(/^\s*extern\s+[\w:<>*&\s]+?\s+([A-Za-z_]\w*)\s*[;[]/);if(r){let a=e.replace(/^extern\s+/,"").replace(/;$/,"").trim();return{name:r[1],signature:a}}let o=t.match(/^\s*class\s+([A-Za-z_]\w*)/);if(o)return{name:o[1],signature:`class ${o[1]}`};let s=t.match(/^\s*(?:(?:virtual|static|inline|explicit|constexpr|friend)\s+)*(?:[A-Za-z_][\w:]*\s*[*&\s]+)+([A-Za-z_]\w*)\s*\(/);return s?{name:s[1],signature:e.replace(/;$/,"").trim()}:null}function Yr(t,e){return e.appendLine(`[hover-docs] Loaded ${t.size} Arduino symbol descriptions.`),qe.languages.registerHoverProvider([{scheme:"file",language:"cpp"},{scheme:"file",language:"c"},{scheme:"file",language:"arduino"}],{provideHover(n,r){let o=n.getWordRangeAtPosition(r);if(!o)return null;let s=n.getText(o),a=t.get(s);if(!a)return null;let d=new qe.MarkdownString;d.appendMarkdown(`**function** \`${s}\`

`),d.appendMarkdown(`${a.brief}

`),d.appendCodeblock(a.signature,"cpp");for(let u of a.params)d.appendMarkdown(`*@param* \`${u.name}\` \u2014 ${u.desc}

`);return d.isTrusted=!1,new qe.Hover(d,o)}})}var Ie=C(require("vscode")),Zr=C(require("node:crypto")),de=C(require("node:fs")),eo=C(require("node:os")),q=C(require("node:path"));function to(t){try{return de.readdirSync(t)}catch{return[]}}function Kt(t){let e=q.basename(t),n=q.join(t,`${e}.ino`);if(de.existsSync(n))return n;let r=to(t).filter(o=>o.toLowerCase().endsWith(".ino"));return r.length===1?q.join(t,r[0]):null}function Qr(t){return Kt(t)!==null}function no(){let t=Ie.window.activeTextEditor;if(t&&t.document.uri.fsPath.toLowerCase().endsWith(".ino"))return t.document.uri.fsPath;for(let e of Ie.window.visibleTextEditors)if(e.document.uri.fsPath.toLowerCase().endsWith(".ino"))return e.document.uri.fsPath;return null}function ro(t){if(!t||!t.toLowerCase().endsWith(".ino")||!de.existsSync(t))return null;let e=q.basename(t,q.extname(t)),n=q.dirname(t);if(q.basename(n)===e)return{sketchDir:n,mainIno:t,isTemp:!1};let o=Zr.createHash("sha1").update(t).digest("hex").slice(0,10),s=q.join(eo.tmpdir(),"arduino-grease-sketches",`${e}-${o}`),a=q.join(s,e),d=q.join(a,`${e}.ino`);try{de.mkdirSync(a,{recursive:!0}),de.copyFileSync(t,d);for(let u of to(n)){let m=u.toLowerCase();if(m.endsWith(".h")||m.endsWith(".hpp")||m.endsWith(".cpp")||m.endsWith(".c")||m.endsWith(".cc")||m.endsWith(".cxx"))try{de.copyFileSync(q.join(n,u),q.join(a,u))}catch{}}}catch{return null}return{sketchDir:a,mainIno:d,isTemp:!0,originalIno:t,originalDir:n}}function oo(){let t=Ie.window.activeTextEditor;if(t){let r=t.document.uri.fsPath;if(r.toLowerCase().endsWith(".ino"))return q.dirname(r)}for(let r of Ie.window.visibleTextEditors){let o=r.document.uri.fsPath;if(o.toLowerCase().endsWith(".ino"))return q.dirname(o)}let n=Ie.workspace.workspaceFolders?.[0]?.uri.fsPath;if(!n)return null;if(Qr(n))return n;try{let r=de.readdirSync(n,{withFileTypes:!0});for(let o of r){if(!o.isDirectory())continue;let s=q.join(n,o.name);if(Qr(s))return s}}catch{}return n}var so="http://127.0.0.1:3333",io="";function ao(t){io=t}function lo(t){let e=String(t||"").trim();e&&(so=e.replace(/\/$/,""))}function ct(t=io){return{"x-grease-auth":t,"x-grease-client":"editor"}}async function ce(t,e){let n=await fetch(`${so}${t}`,{method:"POST",headers:{"content-type":"application/json",...ct()},body:JSON.stringify(e??{})});if(!n.ok)throw new Error(`HTTP ${n.status} ${t}`);return await n.json()}async function xe(t,e={}){try{await ce("/target",{port:t?.port??null,fqbn:t?.fqbn??null,auto:!!e.auto,chipFix:!!e.chipFix})}catch{}}async function co(t){try{let e=await ce("/chip",t);return e?.skipped?null:e}catch{return null}}async function Jt(t){return ce("/serial/open",t)}async function pt(){return ce("/serial/close",{})}async function po(t){return ce("/serial/write",t)}async function Un(){return ce("/serial/read",{})}async function qn(t,e=!1){return ce("/plot",{since:t,clear:e})}async function uo(t){return ce("/compile",{sketchPath:t})}async function fo(t){return ce("/upload",{sketchPath:t})}async function Xt(){await ce("/thrust",{})}async function Yt(){await ce("/idle",{})}var ho=require("node:child_process"),ut=C(require("node:path")),zn=C(require("node:os")),se=C(require("node:fs")),go=require("node:crypto");var Fs="1794626889648",Vn=ut.join(zn.homedir(),".grease","extension"),Us=ut.join(Vn,"mcp-auth.json"),We=ut.join(Vn,"ai-tether.log");function bo(t){return new Promise(e=>setTimeout(e,t))}async function Hn(t){try{let n=await(await fetch(`http://127.0.0.1:${t}/health`,{signal:AbortSignal.timeout(1e3)})).json();return n?.name==="grease-ai-tether"?n:null}catch{return null}}function Qt(){try{let t=JSON.parse(se.readFileSync(Us,"utf8"));return{key:typeof t.key=="string"?t.key:null,port:Number(t.port)||null}}catch{return{key:null,port:null}}}async function mo(){let{port:t}=Qt();return t?Hn(t):null}function Wn(t,e,n){return{port:t,authKey:e,stop:async()=>{let r=await Hn(t);if(r){try{await fetch(`http://127.0.0.1:${t}/shutdown`,{method:"POST",headers:ct(e),signal:AbortSignal.timeout(2e3)})}catch{}for(let o=0;o<16;o++){if((await Hn(t))?.pid!==r.pid)return;await bo(250)}n.appendLine(`AI Tether (pid ${r.pid}) did not stop in 4 s.`)}}}}async function vo(t,e,n=3333,r="Visual Studio Code"){let o=t.asAbsolutePath(ut.join("dist","server.mjs")),s=await mo();if(s){let k=Qt().key;if(k&&(Number(s.build)>=Number(Fs)||s.busy))return e.appendLine(`Using AI Tether ${s.version} on port ${s.port} (pid ${s.pid}), shared by this computer.`),Wn(s.port,k,e);k&&(e.appendLine(`Replacing AI Tether ${s.version} (older build, pid ${s.pid}) with this one.`),await Wn(s.port,k,e).stop())}let a=process.env.ARDUINO_MCP_NODE_PATH||process.execPath,d=Qt().key??(0,go.randomBytes)(24).toString("hex");se.mkdirSync(Vn,{recursive:!0});try{se.statSync(We).size>1e6&&se.renameSync(We,`${We}.old`)}catch{}let u=se.openSync(We,"a"),m=(0,ho.spawn)(a,[o],{cwd:zn.homedir(),env:{...process.env,MCP_PORT:String(n),MCP_HOST:"127.0.0.1",MCP_AUTH_KEY:d,MCP_IDE_NAME:r,ELECTRON_RUN_AS_NODE:"1",GREASE_STARTED_BY:"editor"},detached:!0,stdio:["ignore",u,u],shell:!1,windowsHide:!0});m.unref(),se.closeSync(u);let S=null;m.once("exit",k=>S=k);for(let k=0;k<40;k++){await bo(250);let $=await mo(),w=Qt().key;if($&&w&&$.pid!==s?.pid)return e.appendLine(`AI Tether ${$.version} on port ${$.port} (pid ${$.pid}) \xB7 log ${We}`),Wn($.port,w,e);if(S!==null&&S!==0)break}throw new Error(`AI Tether did not start${S!==null?` (exit code ${S})`:""}. See ${We}.`)}var ue=C(require("vscode")),pe=C(require("node:fs")),yo=C(require("node:os")),ie=C(require("node:path"));function ft(t){let e=Array.isArray(t?.examples)?t.examples:[],n=[];for(let r of e){let o=String(r?.library?.name??"Unknown"),s=Array.isArray(r?.examples)?r.examples:[];for(let a of s){let d=String(a??"");d&&n.push({library:o,example:ie.basename(d),fullPath:d})}}return n}function Kn(t){let e=ie.join(t,"snippets");if(!pe.existsSync(e))return[];let n=[];try{let r=pe.readdirSync(e,{withFileTypes:!0}).filter(o=>o.isDirectory());for(let o of r){let s=ie.join(e,o.name),a=pe.readdirSync(s).filter(d=>d.toLowerCase().endsWith(".ino"));for(let d of a)n.push({library:`Grease: ${o.name}`,example:d.replace(/\.ino$/i,""),fullPath:ie.join(s,d)})}}catch{}return n}function Jn(t){let e=new Set,n=[];for(let r of t){let o=`${r.library}::${r.fullPath}`;e.has(o)||(e.add(o),n.push(r))}return n.sort((r,o)=>{let s=r.library.localeCompare(o.library);return s!==0?s:r.example.localeCompare(o.example)})}var Zt=class{constructor(e,n){this.context=e;this.output=n}context;output;panel=null;show(e){this.panel?(this.panel.title="X-mpls",this.panel.reveal(ue.ViewColumn.Beside)):(this.panel=ue.window.createWebviewPanel("arduinoMcp.examples","X-mpls",ue.ViewColumn.Beside,{enableScripts:!0}),this.panel.onDidDispose(()=>this.panel=null),this.panel.webview.onDidReceiveMessage(n=>{this.onMessage(n,e)})),this.panel.webview.html=this.html(),this.onMessage({type:"list",library:""},e)}async listExamples(e){let n=["lib","examples","--json"];this.output.appendLine(`$ arduino-cli ${n.join(" ")}`);let r=await Y(n);if(!r.success)throw new Error(r.stderr||r.stdout||"Failed to list examples");let o=ft(JSON.parse(r.stdout));if(e){let a=["lib","examples","--fqbn",e,"--json"];this.output.appendLine(`$ arduino-cli ${a.join(" ")}`);let d=await Y(a);d.success&&(o=o.concat(ft(JSON.parse(d.stdout))))}let s=ie.join(yo.homedir(),"Documents","Arduino","libraries","AdvancedAnalog");if(pe.existsSync(s)){let a=ie.join(s,"examples");if(pe.existsSync(a))try{let d=pe.readdirSync(a,{withFileTypes:!0}).filter(u=>u.isDirectory());for(let u of d)o.push({library:"Filtered analog",example:u.name,fullPath:ie.join(a,u.name)})}catch{}}return o=o.concat(Kn(this.context.extensionPath)),Jn(o)}async openExampleAsTab(e){let n=String(e||"").trim();if(!n)return;let r=null;if(n.toLowerCase().endsWith(".ino")&&pe.existsSync(n))r=n;else{let s=ie.basename(n),a=ie.join(n,`${s}.ino`);if(pe.existsSync(a))r=a;else try{let d=pe.readdirSync(n).filter(u=>u.toLowerCase().endsWith(".ino"));d.length>0&&(r=ie.join(n,d[0]))}catch{}}if(!r){ue.window.showWarningMessage("Grease: No .ino file found in this example.");return}let o=await ue.workspace.openTextDocument(ue.Uri.file(r));await ue.window.showTextDocument(o,{preview:!1})}async onMessage(e,n){if(!(!this.panel||!e?.type))if(e.type==="list")try{let r=await this.listExamples(n);this.panel.webview.postMessage({type:"examples",rows:r})}catch(r){this.panel.webview.postMessage({type:"error",error:r instanceof Error?r.message:String(r)})}else e.type==="openExample"&&await this.openExampleAsTab(String(e.path??""))}html(){return`<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <style>
      :root {
        --bg: #0b0f0b;
        --panel: #121912;
        --ink: #d5ffd5;
        --soft: #bce6bc;
        --muted: #7ca57c;
        --stroke: #335233;
      }
      body { margin: 0; padding: 12px; background: var(--bg); color: var(--ink); font-family: Consolas, Menlo, Monaco, "Courier New", monospace; font-size:11px; }
      .card { border: 1px solid var(--stroke); background: var(--panel); padding: 9px; border-radius: 8px; margin-bottom: 9px; }
      input {
        background: #0d130d;
        color: var(--ink);
        border: 1px solid var(--stroke);
        border-radius: 6px;
        padding: 6px 7px;
        font-size: 11px;
        width: 168px;
      }
      .muted { color: var(--muted); font-size: 10px; }
      .list { display:flex; flex-direction:column; gap:7px; }
      .item { border:1px solid var(--stroke); border-radius:8px; background:#0d130d; padding:7px; cursor:pointer; }
      .item:hover { border-color:#58aa58; }
      .title { font-weight:700; font-size:11px; overflow-wrap:anywhere; color:var(--soft); }
      .meta { font-size:10px; color:var(--muted); margin-top:2px; overflow-wrap:anywhere; }
      .row { display:flex; gap:8px; align-items:flex-end; }
    </style>
  </head>
  <body>
    <div class="card">
      <div class="row">
        <div>
          <div class="muted">Search Text</div>
          <input id="query" placeholder="blink, imu, wifi" />
        </div>
        <div>
          <div class="muted">Library name</div>
          <input id="library" placeholder="wire, serv" />
        </div>
      </div>
    </div>

    <div class="card">
      <div id="error" style="color:#ffb4b4; white-space:pre-wrap"></div>
      <div id="count" class="muted" style="margin-bottom:8px"></div>
      <div id="out" class="list"></div>
    </div>

    <script>
      const vscode = acquireVsCodeApi();
      const $ = (id) => document.getElementById(id);
      let rows = [];

      function esc(s) {
        return String(s).replaceAll("&","&amp;").replaceAll("<","&lt;").replaceAll(">","&gt;").replaceAll('"',"&quot;").replaceAll("'","&#39;");
      }

      function render() {
        const q = String($("query").value || "").trim().toLowerCase();
        const lib = String($("library").value || "").trim().toLowerCase();
        const filtered = rows.filter((r) => {
          const full = (r.library + " " + r.example + " " + r.fullPath).toLowerCase();
          const libMatch = !lib || r.library.toLowerCase().includes(lib);
          const textMatch = !q || full.includes(q);
          return libMatch && textMatch;
        });

        $("count").textContent = String(filtered.length) + " example sketches";
        if (filtered.length === 0) {
          $("out").innerHTML = "<div class='muted'>No example sketches found.</div>";
          return;
        }

        $("out").innerHTML = filtered.map((r) => {
          return "<div class='item' data-path='" + esc(r.fullPath) + "'><div class='title'>" + esc(r.example) + "</div><div class='meta'>" + esc(r.library) + "</div><div class='meta'>" + esc(r.fullPath) + "</div></div>";
        }).join("");

        document.querySelectorAll(".item").forEach((el) => {
          el.addEventListener("dblclick", () => {
            vscode.postMessage({ type: "openExample", path: el.getAttribute("data-path") || "" });
          });
        });
      }

      function maybeSearchOnEnter(ev) {
        if (ev.key === "Enter") {
          ev.preventDefault();
          render();
        }
      }

      $("query").addEventListener("keydown", maybeSearchOnEnter);
      $("library").addEventListener("keydown", maybeSearchOnEnter);
      $("query").addEventListener("input", render);
      $("library").addEventListener("input", render);

      window.addEventListener("message", (event) => {
        const msg = event.data;
        if (msg.type === "error") {
          $("error").textContent = msg.error || "Unknown error";
        } else if (msg.type === "examples") {
          rows = Array.isArray(msg.rows) ? msg.rows : [];
          render();
        }
      });
    </script>
  </body>
</html>`}};var mt=C(require("vscode")),wo=C(require("node:os"));var en=class{constructor(e,n){this.output=e;this.onLine=n}output;onLine;pollTimer=null;connectedPort=null;baudRate=9600;isConnected=!1;async start(e,n=9600){if(this.baudRate=n,!e){this.output.appendLine("Serial: no port selected.");return}await this.connect(e,this.baudRate),this.startPolling()}async stop(){this.isConnected&&(await pt(),this.isConnected=!1,this.output.appendLine("Serial disconnected"))}async handleConsoleCommand(e){let n=String(e??"").trim();if(!n)return;if(/^baud\s*=\s*\d+$/i.test(n)){let s=Number(n.split("=")[1]);if(!Number.isFinite(s)||s<=0){this.output.appendLine("Serial: invalid baud value.");return}this.baudRate=s,this.output.appendLine(`Serial baud set to ${s}`),this.connectedPort&&await this.connect(this.connectedPort,this.baudRate);return}if(/^clear$/i.test(n)){let a=(await Un()).lines?.length??0;this.output.appendLine(`Serial buffer cleared (${a} lines dropped).`);return}if(/^disconnect$/i.test(n)){await this.stop();return}if(/^connect$/i.test(n)){if(!this.connectedPort){this.output.appendLine("Serial: no known port to connect.");return}await this.connect(this.connectedPort,this.baudRate);return}let r=n.match(/^send\s*=\s*"([\s\S]*)"$/i),o=n.match(/^send\s*=\s*(.+)$/i);if(r||o){let s=r?r[1]:o?.[1]??"";if(!this.isConnected){this.output.appendLine("Serial: not connected.");return}await po({data:s}),this.output.appendLine(`[serial:tx] ${s}`);return}this.output.appendLine(`Serial: unknown command "${n}".`)}async connect(e,n){let s="";for(let d=1;d<=3;d++)try{await Jt({path:e,baudRate:n}),this.connectedPort=e,this.baudRate=n,this.isConnected=!0,this.output.appendLine(`Serial connected: ${e} @ ${n}`);return}catch(u){if(s=u instanceof Error?u.message:String(u),s.includes("Permission denied")||s.includes("EACCES")||s.includes("EPERM"))break;d<3&&(this.output.appendLine(`Serial open failed (attempt ${d}/3): ${s} \u2014 retrying in 600ms`),await new Promise(S=>setTimeout(S,600)))}(s.includes("Permission denied")||s.includes("EACCES")||s.includes("EPERM"))&&wo.platform()==="linux"?mt.window.showErrorMessage(`Serial: Permission denied on ${e}. On Linux, run: sudo usermod -a -G dialout $USER \u2014 then log out and back in.`,"Copy Command").then(d=>{d==="Copy Command"&&mt.env.clipboard.writeText("sudo usermod -a -G dialout $USER")}):mt.window.showErrorMessage(`Serial: Failed to open ${e}: ${s}`),this.output.appendLine(`Serial open failed: ${s}`)}startPolling(){this.pollTimer||(this.pollTimer=setInterval(()=>{this.isConnected&&(async()=>{try{let e=await Un();for(let r of e.lines??[])this.output.appendLine(`[serial] ${r}`);let n=e.lines?.[e.lines.length-1];n&&this.onLine?.(n)}catch{}})()},250))}};var re=C(require("vscode")),tn=C(require("node:os")),So=require("node:crypto");var nn=class{constructor(e,n){this.output=e;this.extensionUri=n}output;extensionUri;panel=null;pollTimer=null;isConnected=!1;portOpen=!1;pendingPort=null;seq=0;show(e,n=!0){this.panel||(this.panel=re.window.createWebviewPanel("arduinoMcp.serialPlotter","Grease: Plttr",re.ViewColumn.Beside,{enableScripts:!0,localResourceRoots:[re.Uri.joinPath(this.extensionUri,"dist")]}),this.panel.onDidDispose(()=>this.dispose()),this.panel.onDidChangeViewState(r=>{r.webviewPanel.visible&&(this.seq=0)}),this.panel.webview.onDidReceiveMessage(r=>{this.onMessage(r)})),this.panel.title="Grease: Plttr",this.panel.reveal(re.ViewColumn.Beside),this.seq=0,this.panel.webview.html=this.html(this.panel.webview,e),this.startPolling(),this.postStatus(),this.pendingPort=n&&e&&!this.isConnected?e:null}dispose(){this.panel=null,this.pollTimer&&clearInterval(this.pollTimer),this.pollTimer=null,this.isConnected&&(pt(),this.isConnected=!1)}async connect(e,n){let s="";for(let d=1;d<=3;d++)try{await Jt({path:e,baudRate:n}),this.output.appendLine(`Serial connected (plotter): ${e} @ ${n}`),this.isConnected=!0,this.postStatus();return}catch(u){if(s=u instanceof Error?u.message:String(u),s.includes("Permission denied")||s.includes("EACCES")||s.includes("EPERM"))break;d<3&&(this.output.appendLine(`Serial open failed (plotter, attempt ${d}/3): ${s} \u2014 retrying in 600ms`),await new Promise(S=>setTimeout(S,600)))}(s.includes("Permission denied")||s.includes("EACCES")||s.includes("EPERM"))&&tn.platform()==="linux"?re.window.showErrorMessage(`Serial: Permission denied on ${e}. On Linux, run: sudo usermod -a -G dialout $USER \u2014 then log out and back in.`,"Copy Command").then(d=>{d==="Copy Command"&&re.env.clipboard.writeText("sudo usermod -a -G dialout $USER")}):re.window.showErrorMessage(`Serial: Failed to open ${e}: ${s}`),this.output.appendLine(`Serial open failed (plotter): ${s}`)}startPolling(){if(this.pollTimer)return;let e=!1;this.pollTimer=setInterval(()=>{!this.panel||e||(e=!0,(async()=>{try{let n=await qn(this.seq);(n.cleared||n.rows.length||this.seq===0)&&this.panel?.webview.postMessage({type:"plot",rows:n.rows,labels:n.labels,cleared:n.cleared||this.seq===0}),this.seq=n.seq,this.portOpen=n.serial.isOpen,n.serial.isOpen||(this.isConnected=!1);let r=this.pendingPort;this.pendingPort=null,r&&!n.serial.isOpen&&await this.connect(r,9600),this.panel?.webview.postMessage({type:"status",status:n.serial})}catch{this.panel?.webview.postMessage({type:"status",status:{isOpen:!1}})}finally{e=!1}})())},150)}postStatus(){this.panel&&this.panel.webview.postMessage({type:"status",status:{isOpen:this.isConnected}})}async onMessage(e){if(e?.type)try{if(e.type==="toggle"){let n=String(e.path||""),r=Number(e.baudRate||9600);this.isConnected||this.portOpen?(await pt(),this.output.appendLine("Serial disconnected (plotter)"),this.isConnected=!1):await this.connect(n,r),this.postStatus()}else e.type==="clear"&&await qn(this.seq,!0)}catch(n){let r=n instanceof Error?n.message:String(n);this.output.appendLine(`Serial plotter error: ${r}`),re.window.showErrorMessage(`Grease Plotter: ${r}`),this.postStatus()}}html(e,n){let r=n?n.replaceAll("&","&amp;").replaceAll('"',"&quot;").replaceAll("<","&lt;"):"",o=tn.platform()==="win32"?"COM1":tn.platform()==="darwin"?"/dev/cu.usbmodem...":"/dev/ttyUSB0",s=e.asWebviewUri(re.Uri.joinPath(this.extensionUri,"dist","plotter.js")),a=(0,So.randomBytes)(16).toString("hex");return`<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8" />
    <meta http-equiv="Content-Security-Policy" content="${["default-src 'none'",`style-src ${e.cspSource} 'unsafe-inline'`,`script-src 'nonce-${a}'`].join("; ")}">
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <style>
      :root { --panel: #0d1611; --ink: #b5ffc8; --muted: #76bf8b; --stroke: #2d4d3a; --mono: Consolas, Menlo, Monaco, "Courier New", monospace; }
      * { box-sizing: border-box; }
      html, body { height: 100%; }
      body { margin: 0; padding: 12px; display: flex; flex-direction: column; background: radial-gradient(circle at 20% 0%, #0c120f, #050806 65%); color: var(--ink); font-family: var(--mono); font-size: 11px; }
      .card { border: 1px solid var(--stroke); background: var(--panel); padding: 8px; border-radius: 8px; margin-bottom: 8px; }
      .row { display: flex; gap: 8px; align-items: flex-end; flex-wrap: wrap; }
      label { font-size: 10px; color: var(--muted); }
      input { background: #07100b; color: var(--ink); border: 1px solid var(--stroke); border-radius: 6px; padding: 6px 7px; font: 11px var(--mono); min-width: 180px; }
      #baud { min-width: 90px; width: 90px; }
      .status { font-size: 10px; color: var(--muted); margin-left: auto; padding-bottom: 6px; }
      .cmdbtn { color: #d8ffd8; cursor: pointer; position: relative; padding-bottom: 6px; user-select: none; }
      .cmdbtn::after { content: attr(data-tip); position: absolute; left: 0; bottom: 120%; background: #0a110a; color: #e5ffe5; border: 1px solid #335233; border-radius: 4px; padding: 2px 4px; font-size: 10px; opacity: 0; pointer-events: none; transition: opacity .12s ease; white-space: nowrap; }
      .cmdbtn:hover::after { opacity: 1; }
      #tabs { display: flex; gap: 2px; }
      #tabs button { background: #07100b; color: #4c7a5a; border: 1px solid var(--stroke); border-bottom: none; border-radius: 6px 6px 0 0; padding: 5px 14px; font: 11px var(--mono); cursor: pointer; }
      #tabs button.lit { color: var(--ink); }
      #tabs button.lit::before { content: "\\25CF  "; color: #7dff9e; }
      #tabs button.on { background: var(--panel); color: #e5ffe5; border-color: #49c06b; }
      canvas { flex: 1; min-height: 160px; width: 100%; display: block; border: 1px solid var(--stroke); border-radius: 0 8px 8px 8px; }
      #legend { display: flex; gap: 10px; flex-wrap: wrap; align-items: center; padding: 5px 8px; margin-top: 4px; border: 1px solid var(--stroke); border-radius: 6px; background: var(--panel); min-height: 26px; font-size: 10px; }
      .leg-item { cursor: pointer; user-select: none; }
      .leg-item:hover { text-decoration: underline; }
      .leg-item.off { opacity: .3; }
      .leg-bg { cursor: pointer; color: var(--muted); border: 1px solid var(--stroke); padding: 1px 6px; border-radius: 3px; margin-left: auto; }
      .leg-bg:hover { color: var(--ink); }
    </style>
  </head>
  <body>
    <div class="card">
      <div class="row">
        <div><label>Port</label><br/><input id="port" placeholder="${o}" value="${r}"/></div>
        <div><label>Baud</label><br/><input id="baud" value="9600"/></div>
        <span class="cmdbtn" data-tip="start/stop" id="toggle">||S||</span>
        <span class="cmdbtn" data-tip="clear the Plotter (agents too)" id="clear">||C||</span>
        <div class="status" id="status"></div>
      </div>
    </div>
    <div id="tabs"><button data-m="time">Time</button><button data-m="xy" title="name_x + name_y on one line">XY</button><button data-m="radar" title="name_deg + name_cm on one line">Radar</button></div>
    <canvas id="canvas"></canvas>
    <div id="legend"></div>
    <script nonce="${a}" src="${s}"></script>
  </body>
</html>`}};var R=C(require("vscode")),gt=C(require("node:fs")),bt=C(require("node:path"));function Xn(t){let e=t.jobBy==="agent";return t.uploading?e?"agentUpload":"upload":t.compiling?e?"agentCompile":"compile":t.agentActive||t.agentRecent?"agent":"idle"}var ko=require("node:child_process"),ht=C(require("node:fs"),1),Co=C(require("node:os"),1),Yn=C(require("node:path"),1),qs=["/etc/udev/rules.d","/usr/lib/udev/rules.d","/lib/udev/rules.d"],Ws=/2e8a/i,Hs=`echo 'SUBSYSTEM=="usb", ATTRS{idVendor}=="2e8a", MODE="0666"' | sudo tee /etc/udev/rules.d/60-picotool.rules && sudo udevadm control --reload-rules`;function xo(t,e){return new Promise(n=>{(0,ko.execFile)(t,e,{timeout:3e3},(r,o)=>n(`${o??""}`.trim()))})}function zs(t=qs){for(let e of t){let n=[];try{n=ht.default.readdirSync(e)}catch{continue}for(let r of n)try{if(r.endsWith(".rules")&&Ws.test(ht.default.readFileSync(Yn.default.join(e,r),"utf8")))return!0}catch{}}return!1}var Vs=()=>({platform:process.platform,isFlatpak:!!process.env.FLATPAK_ID||ht.default.existsSync("/.flatpak-info"),modemManagerState:()=>xo("systemctl",["is-active","ModemManager"]),brlttyUnitState:()=>xo("systemctl",["is-enabled","brltty-udev.service"]),rpCoreInstalled:ht.default.existsSync(Yn.default.join(Co.default.homedir(),".arduino15","packages","rp2040")),hasRpRule:zs});async function Ao(t=Vs()){if(t.platform!=="linux")return[];let e=[];t.isFlatpak&&e.push({id:"flatpak",blocking:!0,title:"Flatpak editor",why:"A Flatpak editor cannot see USB ports or programs installed on this computer.",note:"Install the editor from its .deb, .rpm or .tar.gz instead."});let[n,r]=await Promise.all([t.modemManagerState(),t.brlttyUnitState()]);return n==="active"&&e.push({id:"modemmanager",blocking:!1,title:"ModemManager running",why:"It probes new USB ports for about 10 s, which can break uploads to 32U4 boards such as Leonardo.",fix:"sudo systemctl disable --now ModemManager",note:"Skip this if you use a USB cellular modem."}),r&&r!=="masked"&&r!=="not-found"&&e.push({id:"brltty",blocking:!1,title:"brltty installed",why:"It takes CH340 USB serial ports, so the board's port disappears.",fix:"sudo systemctl mask --now brltty-udev.service",note:"Skip this if you use a Braille display."}),t.rpCoreInstalled&&!t.hasRpRule()&&e.push({id:"udev-rp",blocking:!1,title:"No USB rule for RP2040/RP2350",why:"Without a udev rule, picotool cannot reach the board to upload.",fix:Hs}),e}function rn(t){let e=JSON.parse(t),n=Array.isArray(e?.platforms)?e.platforms:[],r=[];for(let o of n){let s=String(o?.id??""),a=String(o?.installed_version??""),u=(o?.releases??{})?.[a]??null,m=Array.isArray(u?.boards)?u.boards:[];for(let S of m){let k=String(S?.name??"").trim(),$=String(S?.fqbn??"").trim();!k||!$||r.push({name:k,fqbn:$,platform:s,version:a})}}return r.sort((o,s)=>o.name.localeCompare(s.name)),r}function Qn(t){let e=JSON.parse(t),r=(Array.isArray(e?.installed_libraries)?e.installed_libraries:Array.isArray(e?.libraries)?e.libraries:[]).map(o=>({name:String(o?.library?.name??o?.name??"").trim(),version:String(o?.library?.version??o?.version??"").trim()||void 0,author:String(o?.library?.author??o?.author??"").trim()||void 0,sentence:String(o?.library?.sentence??o?.sentence??"").trim()||void 0})).filter(o=>o.name.length>0);return r.sort((o,s)=>o.name.localeCompare(s.name)),r}var He=class{constructor(e,n,r){this.context=e;this.output=n;this.actions=r}context;output;actions;static viewType="arduinoMcp.toolbar";view=null;state={port:null,fqbn:null,connectedPorts:[],serverRunning:!1,serverHealthy:!1,lastScanAtMs:null,serialActive:!1};lastCli=null;lastNode=null;lastLinux=[];job=null;pendingTab=null;bubble="idle";setState(e){this.state=e,this.postState()}setBubble(e){e!==this.bubble&&(this.bubble=e,this.view?.webview.postMessage({type:"bubble",state:e}))}postSerialLine(e){this.view?.webview.postMessage({type:"serialLine",line:e.slice(0,48)})}postMotion(){let e=R.workspace.getConfiguration("arduinoMcp").get("motion",!0);this.view?.webview.postMessage({type:"motion",on:e})}showTab(e){this.view?this.view.webview.postMessage({type:"switchTab",tab:e}):this.pendingTab=e}resolveWebviewView(e){this.view=e,e.title="",e.webview.options={enableScripts:!0,localResourceRoots:[R.Uri.joinPath(this.context.extensionUri,"resources")]},e.webview.html=this.html(e.webview),e.webview.onDidReceiveMessage(r=>{this.onMessage(r)});let n=R.workspace.onDidChangeConfiguration(r=>{r.affectsConfiguration("arduinoMcp.motion")&&this.postMotion()});e.onDidDispose(()=>n.dispose()),this.postState(),this.postMotion(),e.webview.postMessage({type:"bubble",state:this.bubble})}postState(){this.view?.webview.postMessage({type:"state",state:this.state})}postGrease(){this.view?.webview.postMessage({type:"greaseStatus",status:{cli:this.lastCli,job:this.job,node:this.lastNode,linux:this.lastLinux,agents:On(R.env.appName),tetherPort:this.state.serverRunning?this.state.tetherPort??null:null}})}setJob(e){this.job=e,this.postGrease()}refreshCli(){this.checkCli().then(()=>this.postGrease())}async checkCli(){this.lastCli=await jr(),this.actions.onSetupChecked?.(this.lastCli.state!=="missing")}async onGreaseMessage(e){switch(e.type){case"greaseRefresh":return this.postGrease(),(e.checks||!this.lastCli||!this.lastNode)&&(await Promise.all([this.checkCli(),Fr(process.execPath).then(n=>this.lastNode=n),Ao().then(n=>this.lastLinux=n)]),this.postGrease()),!0;case"gPrepareCli":return await this.actions.prepareCli?.(),await this.checkCli(),this.postGrease(),!0;case"gUseCli":{let n=String(e.path??"");return(this.lastCli?.copies??[]).includes(n)&&(lr({arduinoCliPath:n}),this.output.appendLine(`[Setup] Using arduino-cli at ${n}.`),await this.checkCli(),this.postGrease()),!0}case"gCopy":return await R.env.clipboard.writeText(String(e.text??"").slice(0,2e3)),R.window.setStatusBarMessage("Grease: copied",2e3),!0;case"gAddTools":{let n=String(e.id??"");if(!Gr.includes(n))return!0;let r=await Ft(n,{editorNode:process.execPath});return this.output.appendLine(`[Grease] Add tools (${n}): ${r.message}${r.manual?` ${r.manual}`:""}`),r.ok?R.window.showInformationMessage(`Grease: ${r.message}`):r.manual?await R.window.showWarningMessage(`Grease: ${r.message} ${r.manual}`,"Copy command")&&await R.env.clipboard.writeText(r.manual):R.window.showWarningMessage(`Grease: ${r.message}`),this.postGrease(),!0}case"gReinstall":return await R.commands.executeCommand("arduinoMcp.reinstallSkill"),this.postGrease(),!0;case"gRemove":return await R.commands.executeCommand("arduinoMcp.removeSkill"),this.postGrease(),!0;case"gShow":return await R.commands.executeCommand("arduinoMcp.showSkill"),!0;case"gOpen":{let n=On(R.env.appName).find(r=>r.id===String(e.id??""));return n&&await R.env.openExternal(R.Uri.parse(n.download)),!0}}return!1}async onMessage(e){if(!e?.type)return;try{if(await this.onGreaseMessage(e))return}catch(r){let o=r instanceof Error?r.message:String(r);this.output.appendLine(`[Grease] Error: ${o}`),this.view?.webview.postMessage({type:"greaseError",error:o});return}if(e.type==="cmd"){let r=String(e.command??"");if(!/^arduinoMcp\.[A-Za-z]+$/.test(r))return;await R.commands.executeCommand(r);return}if(e.type==="list"){try{let o=await Y(["lib","examples","--json"]);if(!o.success)throw new Error(o.stderr||o.stdout||"Failed to list examples");let s=ft(JSON.parse(o.stdout)),a=this.state.fqbn;if(a){let d=await Y(["lib","examples","--fqbn",a,"--json"]);d.success&&(s=s.concat(ft(JSON.parse(d.stdout))))}s=s.concat(Kn(this.context.extensionPath)),this.view?.webview.postMessage({type:"examples",rows:Jn(s)})}catch(r){this.view?.webview.postMessage({type:"exError",error:r instanceof Error?r.message:String(r)})}return}if(e.type==="openExample"){let r=String(e.path??"").trim();if(!r)return;let o=null;if(r.toLowerCase().endsWith(".ino")&&gt.existsSync(r))o=r;else{let a=bt.basename(r),d=bt.join(r,a+".ino");if(gt.existsSync(d))o=d;else try{let u=gt.readdirSync(r).filter(m=>m.toLowerCase().endsWith(".ino"));u.length>0&&(o=bt.join(r,u[0]))}catch{}}if(!o){R.window.showWarningMessage("Grease: No .ino file found in this example.");return}let s=await R.workspace.openTextDocument(R.Uri.file(o));await R.window.showTextDocument(s,{preview:!1}),await R.commands.executeCommand("workbench.action.files.setActiveEditorReadonlyInSession"),R.window.showInformationMessage("This is a read-only example. Save a copy to edit it.","Save As New Sketch").then(a=>{a==="Save As New Sketch"&&R.commands.executeCommand("workbench.action.files.saveAs")});return}let n=r=>{this.view?.webview.postMessage(r)};try{if(e.type==="updateIndexes"||e.type==="boardsInstalled"){if(e.type==="updateIndexes"){this.output.appendLine("[Mngrs] Updating board and library indexes (online)...");let o=await yr({onData:s=>this.output.append(s)});if(!o.success){let s=le(o.core.success?o.lib:o.core);this.output.appendLine(s.stderr),n({type:"mgrError",error:s.stderr.split(`
`).filter(Boolean).slice(-2).join(" ")})}}let r=await Y(["core","list","--json"]);if(r.success){let o=rn(r.stdout);n({type:"boards",rows:o}),this.output.appendLine(`[Mngrs] Loaded ${o.length} installed boards.`)}else n({type:"mgrError",error:"arduino-cli core list failed. See Output \u203A Grease VS."})}else if(e.type==="boardSearch"){let r=await Y(["core","list","--json"]);if(!r.success){n({type:"mgrError",error:"arduino-cli core list failed. See Output \u203A Grease VS."});return}n({type:"boards",rows:rn(r.stdout),query:String(e.query??"")})}else if(e.type==="chooseTarget"){let r=String(e.fqbn??"").trim();if(!r){n({type:"mgrError",error:"Select a board first."});return}this.output.appendLine(`[Mngrs] Choosing target ${r}...`),await this.actions.chooseTarget(r)}else if(e.type==="uploadFirmwareToTarget"){let r=String(e.fqbn??"").trim();if(!r){n({type:"mgrError",error:"Select a board first."});return}this.output.appendLine(`[Mngrs] Upload firmware to target ${r}...`),await this.actions.uploadFirmwareToTarget(r)}else if(e.type==="recoveryUpload")await this.actions.recoveryUpload();else if(e.type==="libList"){let r=await Y(["lib","list","--json"]);if(!r.success){n({type:"mgrError",error:r.stderr||r.stdout});return}n({type:"libraries",rows:Qn(r.stdout)})}else if(e.type==="libSearch"){let r=String(e.query??"").trim(),o=le(await Y(["lib","search",...r?[r]:[],"--json"],void 0,_.index));if(!o.success){n({type:"mgrError",error:o.stderr||o.stdout});return}n({type:"libraries",rows:Qn(o.stdout)})}else if(e.type==="libInstallSelected"){let r=String(e.name??"").trim();if(!r){n({type:"mgrError",error:"Select a library first."});return}this.output.appendLine(`[Mngrs] Installing library ${r} (online)...`);let o=le(await wr(r,null,{onData:s=>this.output.append(s)}));o.success?(R.window.showInformationMessage(`Grease: Library ${r} installed.`),this.actions.onLibraryInstalled&&await this.actions.onLibraryInstalled()):(this.output.appendLine(o.stderr),n({type:"mgrError",error:o.stderr||o.stdout}))}else if(e.type==="toggleSerial")await R.commands.executeCommand("arduinoMcp.toggleSerial");else if(e.type==="libInstallGit"){let r=String(e.input??"").trim();if(!r)return;let o=$t(r);if("error"in o){n({type:"mgrError",error:o.error});return}if(await R.window.showWarningMessage(`Install the library at ${o.url}?`,{modal:!0,detail:"Grease downloads this repository from GitHub and compiles its code into your sketches. Install only code you trust."},"Install")!=="Install")return;this.output.appendLine(`[Mngrs] Installing library from GitHub (online): ${o.url}`);let a=le(await Sr(r,{onData:d=>this.output.append(d)}));a.success?(n({type:"gitInstalled"}),R.window.showInformationMessage(`Grease: Library ${o.owner}/${o.repo} installed.`),this.actions.onLibraryInstalled&&await this.actions.onLibraryInstalled()):(this.output.appendLine(a.stderr),n({type:"mgrError",error:a.stderr||a.stdout}))}else if(e.type==="boardCatalogInstall"){let r=ye.find(s=>s.id===String(e.id??""));if(!r){n({type:"mgrError",error:"Pick a board from the list."});return}this.output.appendLine(`[Mngrs] Installing ${r.name} (${r.id}, online)...`);let o=le(await It(r.id,{onData:s=>this.output.append(s)}));if(!o.success)this.output.appendLine(o.stderr),n({type:"mgrError",error:o.stderr||o.stdout});else{R.window.showInformationMessage(`Grease: ${r.name} installed.`);let s=await Y(["core","list","--json"]);s.success&&n({type:"boards",rows:rn(s.stdout)}),this.actions.onBoardInstalled&&await this.actions.onBoardInstalled()}}else if(e.type==="serialOff")await this.actions.serialOff();else if(e.type==="setMotion")await R.workspace.getConfiguration("arduinoMcp").update("motion",e.on!==!1,R.ConfigurationTarget.Global);else if(e.type==="cycleAccent"){let r=String(e.color??"#007ACC");await this.actions.cycleAccent(r)}}catch(r){let o=r instanceof Error?r.message:String(r);this.output.appendLine(`[Mngrs] Error: ${o}`),n({type:"mgrError",error:o})}}html(e){let n=e.asWebviewUri(R.Uri.joinPath(this.context.extensionUri,"resources","icon.png")),r=JSON.stringify(ye.map(({id:a,name:d,vendor:u,boards:m})=>({id:a,name:d,vendor:u,boards:m}))).replace(/</g,"\\u003c"),o=JSON.stringify(this.pendingTab??"board");return this.pendingTab=null,`<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta http-equiv="Content-Security-Policy" content="${["default-src 'none'",`img-src ${e.cspSource} data:`,`style-src ${e.cspSource} 'unsafe-inline'`,`font-src ${e.cspSource}`,"script-src 'unsafe-inline'"].join("; ")}">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <style>
  * { box-sizing: border-box; margin: 0; padding: 0; }
  :root { --muted: #4a9960; --ink: #c8ffd8; --bg: #080e0a; --panel: #0b120d; --stroke: #1a3322; --mono: Consolas, Menlo, Monaco, 'Courier New', monospace; }
  body { background: var(--bg); color: var(--ink); font-family: var(--mono); font-size: 11px; height: 100vh; overflow: hidden; }
  .panel-area { width: 100%; height: 100vh; display: flex; flex-direction: column; background: var(--bg); overflow: hidden; position: relative; }
  canvas.bubble { position: absolute; top: 0; left: 0; width: 100%; height: 100%; pointer-events: none; z-index: 0; }
  .panel-content { position: relative; z-index: 1; padding: 10px 10px 4px; overflow-y: auto; height: 100%; }
  .tab-panel { display: none; }
  .mgr-err { color: #ff8a7a; font-size: 10px; white-space: pre-wrap; margin: 4px 0; }
  .mgr-err:empty { display: none; }
  .tab-panel.active { display: block; }
  a { font-family: var(--mono); font-size: 11px; text-decoration: none; cursor: pointer; background: none; border: none; padding: 0; margin: 0; display: inline; }
  a:hover { text-decoration: underline; }
  a.dim { color: var(--muted); } a.dim:hover { color: var(--ink); }
  a.c-green { color: #55efc4; } a.c-amber { color: #f6c542; } a.c-teal { color: #4ecdc4; }
  a.c-sky { color: #81ecec; } a.c-purple { color: #a29bfe; } a.c-blue { color: #74b9ff; }
  a.c-coral { color: #ff6b6b; } a.c-pink { color: #fd79a8; } a.c-std { color: var(--ink); }
  .act-row a { transition: color .2s, text-shadow .25s; }
  a.hot { color: #fff !important; text-shadow: 0 0 8px currentColor; }
  a.hot.pulse { text-shadow: 0 0 16px #fff, 0 0 4px #fff; transition: text-shadow 0s; }
  .serverStatus.agent-on { color: #ffb142 !important; }
  .ascii-line { font-family: var(--mono); font-size: 11px; color: var(--ink); white-space: pre; line-height: 1.6; display: block; overflow: hidden; text-overflow: ellipsis; }
  .dim { color: var(--muted); }
  .actions-wrap { margin-top: 6px; }
  .act-row { padding: 1px 0 1px 2px; font-family: var(--mono); font-size: 10px; line-height: 1.8; white-space: pre; }
  .server-section { margin-top: 12px; font-family: var(--mono); font-size: 11px; color: var(--ink); }
  .server-line { display: flex; align-items: center; gap: 6px; margin-top: 3px; }
  .sblock { font-size: 16px; line-height: 1; cursor: pointer; }
  /* Eye: stops the moving glyphs in one click (the old Grease chip). Off = struck through, solid dark green. */
  .eye { display: inline-flex; align-items: center; padding: 2px 4px; background: rgba(0,0,0,0.35); border: 1px solid var(--stroke); border-radius: 3px; cursor: pointer; opacity: 0.55; transition: opacity 140ms, background 140ms, border-color 140ms; }
  .eye:hover, .eye:focus-visible { opacity: 1; }
  .eye svg { display: block; }
  .eye .o { fill: none; stroke: var(--ink); stroke-width: 1.4; }
  .eye .p { fill: var(--ink); }
  .eye .s { stroke: var(--ink); stroke-width: 1.6; display: none; }
  .eye.off { opacity: 1; background: #10402a; border-color: #1d6b45; }
  .eye.off .o { stroke: #4fbf87; } .eye.off .p { fill: #4fbf87; } .eye.off .s { display: inline; stroke: #4fbf87; }
  .logo-area { margin-top: 12px; }
  .logo-img { display: block; width: 132px; opacity: 0.18; filter: brightness(2) saturate(0.3); }
  .logo-caption { font-family: var(--mono); font-size: 10px; color: #5bc8f5; margin-top: 3px; letter-spacing: 0.04em; }
  .logo-ascii { font-family: var(--mono); font-size: 9px; color: var(--muted); white-space: pre; line-height: 1.4; margin-top: 6px; opacity: 0.55; }

  .mgr-header { display: flex; align-items: baseline; justify-content: space-between; margin-bottom: 10px; }
  .mgr-label { color: var(--muted); font-size: 10px; }
  .mgr-section { margin-bottom: 10px; }
  .mgr-section-title { color: var(--muted); font-size: 10px; text-transform: uppercase; letter-spacing: 0.05em; margin-bottom: 4px; }
  .mgr-card { border: 1px solid var(--stroke); background: var(--panel); border-radius: 6px; padding: 8px; }
  .mgr-card input { background: #050d07; color: var(--ink); border: 1px solid var(--stroke); border-radius: 4px; padding: 4px 6px; font-size: 11px; font-family: var(--mono); width: 100%; margin-bottom: 6px; }
  .mgr-card input:focus { outline: none; border-color: var(--muted); }
  .mgr-top-row { margin-bottom: 7px; font-family: var(--mono); font-size: 10px; display: flex; gap: 14px; flex-wrap: wrap; }
  .mgr-list { max-height: 160px; overflow-y: auto; }
  .mgr-item { padding: 5px 6px; border: 1px solid var(--stroke); border-radius: 4px; margin-bottom: 3px; cursor: pointer; font-size: 10px; background: #050d07; }
  .mgr-item:hover { border-color: #58aa58; }
  .mgr-item .name { color: var(--ink); font-weight: bold; }
  .mgr-item .meta { color: var(--muted); }
  .setup-needed { display: none; margin-top: 4px; font-size: 10px; }
  .g-row { display: flex; gap: 6px; align-items: baseline; flex-wrap: wrap; margin-bottom: 3px; }
  .g-ok { color: #55efc4; } .g-warn { color: #f6c542; } .g-bad { color: #ff8a7a; }
  .g-path { color: var(--muted); word-break: break-all; }
  .g-code { display: block; background: #050d07; border: 1px solid var(--stroke); border-radius: 4px; padding: 4px 6px; margin: 3px 0; color: var(--ink); white-space: pre-wrap; word-break: break-all; user-select: all; }
  .g-note { color: var(--muted); margin-top: 3px; }
  .g-verdict { font-size: 13px; margin-bottom: 6px; }
  .g-hint { font-size: 11px; margin-top: 2px; }
  .g-line { justify-content: space-between; flex-wrap: nowrap; }
  .g-toggle { display: inline-block; margin-top: 6px; }
  #gDetails { margin-top: 8px; }
  #gAgents { margin-top: 8px; }
  .g-foot { margin-top: 8px; margin-bottom: 0; }
</style>
</head>
<body>
<div class="panel-area" id="panelArea">
    <canvas class="bubble" id="bubbleCanvas"></canvas>
    <div class="panel-content">

      <div class="tab-panel active" id="panel-board">
        <span class="ascii-line"><span class="dim">+------------ -  -   +</span></span>
        <span class="ascii-line"><span class="dim">| Port:     </span><a class="dim" href="#" onclick="cmd('arduinoMcp.refreshPortsBoards');return false">||R||</a></span>
        <span class="ascii-line"><span class="dim">| </span><span id="portVal">?</span></span>
        <span class="ascii-line"><span class="dim">| Board:</span></span>
        <span class="ascii-line"><span class="dim">| </span><span id="fqbnVal">?</span></span>
        <span class="ascii-line"><span class="dim">+   -   -  - --------+</span></span>
        <div id="setupNeeded" class="setup-needed"><a class="c-coral" href="#" onclick="switchTab('grease');return false">||Setup needed: open Help||</a></div>
        <div class="actions-wrap">
          <div class="act-row"><span class="dim">+-------------- -  -   +</span></div>
          <div class="act-row"> <a class="c-teal"  href="#" onclick="cmd('arduinoMcp.startSketch');return false">||Start Sketch||</a></div>
          <div class="act-row"> <a id="uploadBtn" class="c-coral"  href="#" onclick="onUploadClick();return false">||Upload||</a></div>
          <div class="act-row"> <a id="compileBtn" class="c-teal"   href="#" onclick="cmd('arduinoMcp.verify');return false">||Compile||</a></div>
          <div class="act-row"> <a id="serialBtn" class="c-blue" href="#" onclick="toggleSerial();return false">||Serial||</a></div>
          <div class="act-row"> <a class="c-teal" href="#" onclick="cmd('arduinoMcp.openSerialPlotter');return false">||Plotter||</a></div>
          <div class="act-row"> <a class="c-blue"   href="#" onclick="switchTab('examples');return false">||Examples||</a></div>
          <div class="act-row"> <a class="c-teal"  href="#" onclick="switchTab('managers');return false">||Managers||</a></div>
          <div class="act-row"> <a class="c-purple"   href="#" onclick="switchTab('grease');return false">||Help||</a></div>
          <div class="act-row"><span class="dim">+   -   -  - ----------+</span></div>
        </div>
        <div class="server-section">
          AI Tether <a class="dim" href="#" onclick="cmd('arduinoMcp.refreshServer');return false">||R||</a>
          <div class="server-line">
            <span class="sblock serverBlock" style="color:#49c06b" onclick="cmd('arduinoMcp.toggleServer')">&#x2588;</span>
            <span style="color:var(--muted);font-size:10px" class="serverStatus">running</span>
          </div>
          <div class="server-line" style="margin-top:6px">
            <span class="sblock" id="accentBlock" style="color:#007ACC;cursor:pointer;font-size:16px;line-height:1" onclick="cycleAccent()" title="Click to cycle accent color">&#x2588;</span>
            <span class="eye" id="eyeBtn" role="button" tabindex="0" aria-pressed="false" aria-label="Moving glyphs" title="Hide the moving glyphs" onclick="flipMotion()" onkeydown="if(event.key==='Enter'||event.key===' '){event.preventDefault();flipMotion();}">
              <svg viewBox="0 0 24 16" width="18" height="12" aria-hidden="true"><path class="o" d="M1 8 C5 2, 19 2, 23 8 C19 14, 5 14, 1 8 Z"/><circle class="p" cx="12" cy="8" r="3"/><line class="s" x1="3" y1="14" x2="21" y2="2"/></svg>
            </span>
          </div>
        </div>
        <div class="logo-area">
          <img class="logo-img" id="logoImg" src="${n}" draggable="false" />
        </div>
      </div>

      <div class="tab-panel" id="panel-managers">
        <div class="mgr-header">
          <span class="mgr-label"><a class="c-coral" href="#" onclick="switchTab('board');return false" style="margin-right:4px">||R||</a>Mngrs</span>
          <a class="c-std" id="updateIdx" href="#" title="Online: downloads the board and library indexes" onclick="event.preventDefault()">||Update indexes &#8599;||</a>
        </div>
        <div id="mgrErr" class="mgr-err" role="alert"></div>
        <div class="mgr-section">
          <div class="mgr-section-title">Board</div>
          <div class="mgr-card">
            <div class="mgr-top-row">
              <a class="c-std" id="chooseTargetBtn" href="#" onclick="event.preventDefault()">||Choose as target||</a>
              <a class="c-coral" id="recoveryUploadBtn" href="#" onclick="event.preventDefault()">||Recovery Upload||</a>
            </div>
            <input id="boardQuery" class="boardQuery" placeholder="arduino, esp32, rp2040..." />
            <div class="mgr-list" id="boardsList"></div>
            <a class="c-amber" id="installBoardBtn" href="#" title="Online: downloads the board core" onclick="event.preventDefault()" style="font-size:10px;margin-top:6px;display:block">||Install new board &#8599;||</a>
          </div>
        </div>
        <div class="mgr-section">
          <div class="mgr-section-title">Library</div>
          <div class="mgr-card">
            <div class="mgr-top-row">
              <a class="c-std" id="libListBtn" href="#" onclick="event.preventDefault()">||List installed||</a>
              <a class="c-green" id="libInstallBtn" href="#" title="Online: downloads the library" onclick="event.preventDefault()">||Install library &#8599;||</a>
            </div>
            <input id="libQuery" class="libQuery" placeholder="search online: servo, imu..." />
            <div class="mgr-list" id="libsList"></div>
          </div>
        </div>
        <div class="mgr-card" style="margin-top: 9px;">
          <div class="muted" style="font-size: 10px; margin-bottom: 4px;">
            Install library from GitHub &#8599; (owner/repo, a github.com link, or owner/repo#tag)
          </div>
          <input id="libGitInput" placeholder="pololu/romi32u4-arduino-library" onkeydown="if(event.key==='Enter') { vscode.postMessage({type:'libInstallGit', input:this.value}); }" />
        </div>
      </div>

      <div class="tab-panel" id="panel-examples">
        <div class="mgr-header">
          <span class="mgr-label"><a class="c-coral" href="#" onclick="switchTab('board');return false" style="margin-right:4px">||R||</a>X-mpls</span>
          <a class="c-std" href="#" onclick="vscode.postMessage({type:'list',library:''});return false">&#8635;</a>
        </div>
        <div class="mgr-card" style="margin-bottom:6px">
          <div style="display:flex;align-items:flex-end;gap:6px;flex-wrap:wrap">
            <div style="flex:1;min-width:70px">
              <div class="muted" style="font-size:9px;margin-bottom:2px">Search Text</div>
              <input id="exQuery" placeholder="blink, imu, wifi" oninput="renderEx()" onkeydown="if(event.key==='Enter')renderEx()" />
            </div>
            <div style="flex:1;min-width:70px">
              <div class="muted" style="font-size:9px;margin-bottom:2px">Library name</div>
              <input id="exLibrary" placeholder="wire, servo" oninput="renderEx()" onkeydown="if(event.key==='Enter')renderEx()" />
            </div>
          </div>
        </div>
        <div id="exError" style="color:#ffb4b4;white-space:pre-wrap;font-size:10px;margin-bottom:4px"></div>
        <div id="exCount" class="muted" style="font-size:9px;margin-bottom:4px">loading...</div>
        <div id="exOut" class="mgr-list" style="max-height:416px"></div>
      </div>

      <div class="tab-panel" id="panel-grease">
        <div class="mgr-header">
          <span class="mgr-label"><a class="c-coral" href="#" onclick="switchTab('board');return false" style="margin-right:4px">||R||</a>Help</span>
          <a class="c-std" href="#" title="Check everything again" onclick="greaseRefresh(true);return false">&#8635;</a>
        </div>
        <div id="gErr" class="mgr-err" role="alert"></div>

        <div class="g-verdict" id="gVerdict"><span class="dim">checking...</span></div>
        <div id="gFix"></div>
        <a class="c-std g-toggle" href="#" id="gToggle" data-toggle>&#9662; Details</a>

        <div id="gDetails">
          <div id="gSetup"></div>
          <div class="g-row g-line">
            <span><span class="sblock serverBlock" style="color:#49c06b" onclick="cmd('arduinoMcp.toggleServer')">&#x2588;</span> AI Tether <span style="color:var(--muted);font-size:10px" class="serverStatus">running</span></span>
            <span class="g-path" id="gPort"></span>
          </div>
          <div id="gAgents"></div>
          <div class="mgr-top-row g-foot">
          <a class="c-green" href="#" onclick="vscode.postMessage({type:'gReinstall'});return false">||Reinstall skill||</a>
          <a class="c-coral" href="#" onclick="vscode.postMessage({type:'gRemove'});return false">||Remove skill||</a>
          <a class="c-std" href="#" onclick="vscode.postMessage({type:'gShow'});return false">||Show skill||</a>
          </div>
        </div>
      </div>

    </div>
</div>

<script>
const vscode = acquireVsCodeApi();

function cmd(command) { vscode.postMessage({ type: 'cmd', command: command }); }

function switchTab(panel) {
  document.querySelectorAll('.tab-panel').forEach(p => p.classList.remove('active'));
  const panelEl = document.getElementById('panel-' + panel);
  if (panelEl) panelEl.classList.add('active');
  queueGather();
  if (panel === 'managers') { vscode.postMessage({type:'serialOff'}); vscode.postMessage({type:'boardsInstalled'}); vscode.postMessage({type:'libList'}); }
  if (panel === 'examples') { vscode.postMessage({type:'serialOff'}); vscode.postMessage({type:'list',library:''}); }
  if (panel === 'grease') greaseRefresh(false);
}

const ACCENT_COLORS = ['#005FA0','#6B0000','#A34300','#8F6809','#6B004A','#520A85','#004D00'];
let accentIndex = 0;
function cycleAccent() {
  accentIndex = (accentIndex + 1) % ACCENT_COLORS.length;
  const color = ACCENT_COLORS[accentIndex];
  const block = document.getElementById('accentBlock');
  if (block) block.style.color = color;
  vscode.postMessage({ type: 'cycleAccent', color: color });
}

// \u2500\u2500 Bubble-up \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500
// Glyphs rise from the bottom of the panel. Exhaust while a compile or upload
// runs (amber when an agent runs it), serial lines rise as words, red glyphs
// fall on an error. Glyphs rebound off links and pass through Grease-green
// text at 8 px.
const bubbleCanvas = document.getElementById('bubbleCanvas');
const bctx = bubbleCanvas.getContext('2d');
const panelArea = document.getElementById('panelArea');
const panelContent = document.querySelector('.panel-content');
const CHARS = 'abcdefghijklmnopqrstuvwxyzNH01+=-./\\\\[]{}()?!<>:;'.split('');
const AMBER = ['#ffb142', '#ffa502', '#f6b93b', '#fdcb6e', '#e1a233', '#ffbe76'];
const RED = ['#ff4757', '#ff6b6b', '#d63031', '#ee5253'];
const SERIAL_GREEN = '#2f7d50';
const rnd = (a, b) => a + Math.random() * (b - a);
const pick = (arr) => arr[(Math.random() * arr.length) | 0];
const pickCh = () => pick(CHARS);
const CHARW = 7, LIFE = 40000;
// Devin's narrowest side bar. The exhaust column sits at its middle.
const MIN_W = 170;
// Compile is half of Upload: half the glyphs on screen, half the speed. Slow glyphs
// stay on screen about twice as long, so Compile emits 0.3 of the glyphs.
const EXHAUST = { compile: 0.9, upload: 3, agentCompile: 0.9, agentUpload: 3 };
const SPEED = { compile: 0.5, upload: 1, agentCompile: 0.5, agentUpload: 1 };
const FLOATER_MS = { idle: 2200, agent: 700 };
const RISE = 1.8, THRUST_RISE = 2.2;
const POP_MS = 2000, POP_R = 140, ERROR_MS = 5000;
// At most this many glyphs, the oldest dropped first; Upload with Serial on sits near it.
const MAX_GLYPHS = 900;
// Text rectangles read from the open tab; glyphs look them up by 16-px row band.
const MAX_SOLIDS = 600, BAND = 16;
// Glyphs shrink and dim while they pass through Grease-green text (as in Rust Grease's bubble.js).
const FONT = 'bold 12px Consolas, Menlo, monospace', FONT_FLOW = 'bold 8px Consolas, Menlo, monospace';
const AGENT = new Set(['agent', 'agentCompile', 'agentUpload']);

// state: what is drawn; server: the last state AI Tether reported (an error covers it for 5 s).
const B = { state: 'idle', server: 'idle', serial: false, motion: true };
let BW = 0, BH = 0, solids = [], bands = new Map(), textBottom = 0, parts = [], queued = null;
let lastLinePrint = 0, lastSyn = 0, exhaustAcc = 0, bubbleErrorTimer = null, gatherQueued = false;
const mouse = { x: -999, y: -999 };
panelArea.addEventListener('mousemove', (e) => { const r = panelArea.getBoundingClientRect(); mouse.x = e.clientX - r.left; mouse.y = e.clientY - r.top; });
panelArea.addEventListener('mouseleave', () => { mouse.x = mouse.y = -999; });

function glyphColor(p) {
  if (B.state === 'error') return p.red || (p.red = pick(RED));
  if (p.line) return SERIAL_GREEN;
  if (AGENT.has(B.state)) return p.amber || (p.amber = pick(AMBER));
  if (B.state === 'compile' || B.state === 'upload') return p.kind === 'syn' ? '#55efc4' : '#5fcf93';
  return p.kind === 'syn' ? '#7af0b5' : '#5fcf93';
}

/** Grease green = hue 90\u2013160\xB0 with some saturation; everything else rebounds. */
function isGreaseGreen(cssColor) {
  const m = cssColor.match(/\\d+(\\.\\d+)?/g); if (!m) return false;
  const [r, g, b] = m.slice(0, 3).map((x) => Number(x) / 255);
  const max = Math.max(r, g, b), min = Math.min(r, g, b), d = max - min;
  if (max === 0 || d / max < 0.15) return false; // white or grey
  let h = max === r ? ((g - b) / d) % 6 : max === g ? (b - r) / d + 2 : (r - g) / d + 4;
  h = (h * 60 + 360) % 360;
  return h >= 90 && h <= 160;
}

function resizeBubble() {
  const r = panelArea.getBoundingClientRect(), dpr = window.devicePixelRatio || 1;
  BW = r.width || 300; BH = r.height || 660;
  bubbleCanvas.width = Math.round(BW * dpr); bubbleCanvas.height = Math.round(BH * dpr);
  bctx.setTransform(dpr, 0, 0, dpr, 0, 0);
  gather();
}
function queueGather() {
  if (gatherQueued) return;
  gatherQueued = true;
  requestAnimationFrame(gather);
}
/** Rectangles of the visible text in the open tab: links rebound, Grease-green text lets glyphs through. */
function gather() {
  gatherQueued = false;
  solids = []; bands = new Map(); textBottom = 0;
  const panel = document.querySelector('.tab-panel.active');
  if (!panel) return;
  const r0 = panelArea.getBoundingClientRect(), outer = panelContent.getBoundingClientRect();
  const walker = document.createTreeWalker(panel, NodeFilter.SHOW_TEXT);
  const range = document.createRange(), bounceOf = new Map();
  for (let n = walker.nextNode(), seen = 0; n && seen < 4 * MAX_SOLIDS && solids.length < MAX_SOLIDS; n = walker.nextNode(), seen++) {
    const first = n.data.search(/\\S/); if (first < 0) continue;
    const el = n.parentElement; if (!el) continue;
    let bounce = bounceOf.get(el);
    if (bounce === undefined) { bounce = !isGreaseGreen(getComputedStyle(el).color); bounceOf.set(el, bounce); }
    const list = el.closest('.mgr-list'), lr = list ? list.getBoundingClientRect() : outer;
    const top = Math.max(outer.top, lr.top), bottom = Math.min(outer.bottom, lr.bottom);
    range.setStart(n, first); range.setEnd(n, n.data.replace(/\\s+$/, '').length);
    for (const r of range.getClientRects()) {
      if (r.width < 1 || r.bottom <= top || r.top >= bottom) continue;
      const s = { l: r.left - r0.left - 1.5, r: r.right - r0.left + 1.5, t: Math.max(r.top, top) - r0.top - 2, b: Math.min(r.bottom, bottom) - r0.top + 2, bounce };
      solids.push(s);
      textBottom = Math.max(textBottom, s.b);
      for (let k = Math.floor(s.t / BAND); k <= Math.floor(s.b / BAND); k++) {
        let a = bands.get(k); if (!a) bands.set(k, (a = [])); a.push(s);
      }
    }
  }
}
new ResizeObserver(resizeBubble).observe(panelArea);
panelContent.addEventListener('scroll', queueGather, { capture: true, passive: true });
new MutationObserver(queueGather).observe(panelContent, { subtree: true, childList: true, characterData: true, attributes: true, attributeFilter: ['class', 'style'] });

function spawn(p) { parts.push({ vx: 0, vy: 0, born: performance.now(), kind: 'board', ...p }); }

/** White glyphs out of a link; push also shoves the glyphs near it out and up. */
function burstFrom(el, { n = 70, v = [2, 5], life = 2600, push = 0 } = {}) {
  if (!el || !B.motion) return;
  const r = el.getBoundingClientRect(), r0 = panelArea.getBoundingClientRect();
  if (!r.width) return; // its tab is closed
  const cx = (r.left + r.right) / 2 - r0.left, cy = (r.top + r.bottom) / 2 - r0.top;
  if (push) {
    for (const p of parts) {
      const dx = p.x - cx, dy = p.y - cy, d = Math.hypot(dx, dy);
      if (d > POP_R || d < 0.1) continue;
      const k = push * (1 - d / POP_R);
      p.vx += (dx / d) * k; p.vy -= (Math.abs(dy) / d) * k * 0.6 + k * 0.4; // out and up, never down
      if (p.line) p.line.broken = true;
      p.word = false; p.post = true;
    }
  }
  for (let i = 0; i < n; i++) {
    const a = (i / n) * Math.PI * 2, sp = rnd(v[0], v[1]);
    spawn({ ch: pickCh(), x: cx, y: cy, vx: Math.cos(a) * sp, vy: Math.sin(a) * sp, kind: 'burst', drag: true, life });
  }
}
const uploadOk = () => burstFrom(document.getElementById('uploadBtn'));
function serialPop() {
  const b = document.getElementById('serialBtn'); if (!b) return;
  b.classList.add('pulse'); setTimeout(() => b.classList.remove('pulse'), 220);
  burstFrom(b, { n: 16, v: [1.2, 2.6], life: 1100, push: 3.2 });
}

function step(now) {
  requestAnimationFrame(step);
  if (!B.motion) return;
  bctx.save();
  bctx.globalCompositeOperation = 'destination-out';
  bctx.fillStyle = 'rgba(0,0,0,0.3)';
  bctx.fillRect(0, 0, BW, BH);
  bctx.restore();
  bctx.textBaseline = 'middle';
  let font = FONT, fill = ''; bctx.font = font;

  // Serial line: one per 150 ms, newest wins; it rises as a whole word.
  if (queued && now - lastLinePrint >= 150) {
    lastLinePrint = now;
    const text = queued; queued = null;
    const line = { glyphs: [], broken: false };
    const x0 = rnd(10, Math.max(12, BW - text.length * CHARW - 10));
    for (let i = 0; i < text.length; i++) {
      if (text[i] === ' ') continue;
      spawn({ ch: text[i], x: x0 + i * CHARW, y: BH - 22, vy: -1.6, line, word: true });
      line.glyphs.push(parts[parts.length - 1]);
    }
  }
  // Floaters: few when idle, more while the agent works.
  const gap = FLOATER_MS[B.state] ?? FLOATER_MS.idle;
  if (now - lastSyn > gap * rnd(0.5, 1.5)) {
    lastSyn = now;
    spawn({ ch: pickCh(), x: rnd(8, BW - 8), y: BH - 6, vy: -0.4, kind: 'syn' });
  }
  // Exhaust from the bottom, centered on the 170-px panel.
  const speed = SPEED[B.state] ?? 0;
  const cx = Math.min(BW, MIN_W) / 2, spread = MIN_W * 0.18;
  exhaustAcc += EXHAUST[B.state] ?? 0;
  while (exhaustAcc >= 1) {
    exhaustAcc -= 1;
    spawn({ ch: pickCh(), x: cx + rnd(-spread, spread), y: BH - 4, vx: rnd(-0.3, 0.3), vy: rnd(-6, -3.5) * speed, kind: 'syn', life: 4000 });
  }

  const rise = speed ? Math.max(RISE, THRUST_RISE * RISE * speed) : RISE;
  const falling = B.state === 'error';
  const alive = [];
  for (const p of parts) {
    const age = now - p.born, life = p.life ?? LIFE;
    if (age > life) continue;
    const whole = p.word && !p.line.broken;
    if (falling) { p.vy += 0.09; p.vx *= 0.98; p.word = false; }
    else if (p.drag) { p.vx *= 0.96; p.vy = p.vy * 0.96 - 0.03; }
    else if (whole) { p.vy = Math.max(-rise, p.vy - 0.01); p.vx = 0; }
    else {
      if (age > 1000) { p.vx += rnd(-0.005, 0.005); p.vy += rnd(-0.01, 0.01); }
      if (p.vy > -rise) p.vy = Math.max(-rise, p.vy - 0.01);
      else if (p.vy < -rise * 1.2) p.vy += 0.04;
      p.vx *= 0.99;
    }
    const mdx = p.x - mouse.x, mdy = p.y - mouse.y, md = Math.hypot(mdx, mdy);
    if (md < 34 && md > 0.1) { p.vx += (mdx / md) * 0.35; p.vy += (mdy / md) * 0.35; p.post = true; } // a pushed glyph collides
    p.x += p.vx; p.y += p.vy;

    let dim = 1; p.flow = false;
    for (const s of bands.get(Math.floor(p.y / BAND)) || []) {
      if (p.x <= s.l || p.x >= s.r || p.y <= s.t || p.y >= s.b) continue;
      const dir = (p.x - s.l) < (s.r - p.x) ? -1 : 1;
      if (!s.bounce) { p.vy *= 0.995; p.flow = true; dim = 0.5; continue; } // flow through green, no push
      if (p.word && !p.line.broken) {
        p.line.broken = true;
        for (const q of p.line.glyphs) { q.word = false; q.vx = rnd(-2.2, 2.2); q.vy = rnd(-1.5, 1); q.post = true; }
      }
      p.x = dir < 0 ? s.l - 0.5 : s.r + 0.5;
      p.vx += dir * rnd(0.4, 1.6); p.vy *= -rnd(0.2, 0.5);
      p.post = true;
    }
    if (p.y < textBottom) p.post = true;
    if (falling && p.y > BH - 6) { p.y = BH - 6; p.vy *= -0.3; }
    if (p.x < -10 || p.x > BW + 10 || p.y < -16 || p.y > BH + 20) continue;

    let a = 0.9 * dim;
    if (life - age < 6000) a *= (life - age) / 6000;
    if (p.kind === 'burst') a = 0.95 * (1 - age / life);
    else if (p.kind === 'syn' && B.state === 'idle') a *= 0.5;
    const want = p.flow ? FONT_FLOW : FONT;
    if (want !== font) { font = want; bctx.font = font; }
    const hex = p.kind === 'burst' ? '#ffffff' : glyphColor(p);
    if (hex !== fill) { fill = hex; bctx.fillStyle = hex; }
    bctx.globalAlpha = Math.max(0, Math.min(1, a));
    bctx.fillText(p.ch, p.x, p.y);
    alive.push(p);
  }
  bctx.globalAlpha = 1;
  parts = alive.length > MAX_GLYPHS ? alive.slice(alive.length - MAX_GLYPHS) : alive;

  // Glyph-to-glyph collisions. A pair is tested when one of the two rebounded, rose above the
  // words or was pushed by the mouse, so the rising column never collides with itself.
  const CELL = CHARW * 2, R = CHARW * 1.5, grid = new Map();
  const cellKey = (x, y) => ((x / CELL) | 0) * 4096 + ((y / CELL) | 0);
  for (const p of parts) {
    if (p.flow) continue;
    const k = cellKey(p.x, p.y);
    let b = grid.get(k); if (!b) grid.set(k, (b = [])); b.push(p);
  }
  for (const p of parts) {
    if (!p.post || p.flow) continue;
    const k0 = cellKey(p.x, p.y);
    for (let dx = -1; dx <= 1; dx++) for (let dy = -1; dy <= 1; dy++) {
      const b = grid.get(k0 + dx * 4096 + dy); if (!b) continue;
      for (const q of b) {
        if (q === p) continue;
        const ex = p.x - q.x, ey = p.y - q.y, d2 = ex * ex + ey * ey;
        if (d2 > 0.01 && d2 < R * R) {
          const d = Math.sqrt(d2), nx = ex / d, ny = ey / d;
          const dv = (p.vx - q.vx) * nx + (p.vy - q.vy) * ny;
          if (dv < 0) { p.vx -= dv * nx * 0.5; p.vy -= dv * ny * 0.5; q.vx += dv * nx * 0.5; q.vy += dv * ny * 0.5; }
          const o = (R - d) * 0.5;
          p.x += nx * o; p.y += ny * o; q.x -= nx * o; q.y -= ny * o;
        }
      }
    }
  }
}
requestAnimationFrame(step);
// While Serial is on, ||Serial|| pops every 2 s.
setInterval(() => { if (B.serial && B.motion) serialPop(); }, POP_MS);

let lastTether = {};
// Help panel: last status, AI Tether off when last drawn, Details open.
let gLast = null;
let gTetherOff = false;
let gOpen = true;
/** White hot links for the running job and Serial; AI Tether turns amber while an agent works. */
function hotButtons() {
  const st = B.state;
  document.getElementById('compileBtn')?.classList.toggle('hot', st === 'compile' || st === 'agentCompile');
  document.getElementById('uploadBtn')?.classList.toggle('hot', st === 'upload' || st === 'agentUpload');
  document.getElementById('serialBtn')?.classList.toggle('hot', B.serial);
  setServer(lastTether);
}
function setBubble(st) {
  B.state = st;
  hotButtons();
}
/** AI Tether's state (polled every 0.5 s); an error shown now stays for its 5 s. */
function bubbleFromServer(st) {
  B.server = st;
  if (!bubbleErrorTimer && st !== B.state) setBubble(st);
}
function bubbleError() {
  clearTimeout(bubbleErrorTimer);
  bubbleErrorTimer = setTimeout(() => { bubbleErrorTimer = null; setBubble(B.server); }, ERROR_MS);
  setBubble('error');
}
function setSerial(on) {
  if (on === B.serial) return;
  B.serial = on;
  if (!on) queued = null;
  hotButtons();
}
let motionSetting = true;
function setMotion(on) {
  motionSetting = on;
  B.motion = on && !matchMedia('(prefers-reduced-motion: reduce)').matches;
  if (!B.motion) { parts = []; queued = null; bctx.clearRect(0, 0, BW, BH); }
  const eye = document.getElementById('eyeBtn');
  if (eye) {
    eye.classList.toggle('off', !on);
    eye.setAttribute('aria-pressed', String(!on));
    eye.title = on ? 'Hide the moving glyphs' : 'Show the moving glyphs';
  }
}
/** Eye click: flip at once, then save to arduinoMcp.motion (which posts back the same value). */
function flipMotion() {
  setMotion(!motionSetting);
  vscode.postMessage({ type: 'setMotion', on: motionSetting });
}
resizeBubble();
setMotion(true);

function toggleSerial() { vscode.postMessage({ type: 'toggleSerial' }); }
function setServer(state) {
  lastTether = state;
  const agent = !!state.serverRunning && AGENT.has(B.state);
  const blocks = document.querySelectorAll('.serverBlock');
  const statuses = document.querySelectorAll('.serverStatus');
  blocks.forEach(block => {
    if (!state.serverRunning) block.style.color='#593b3bff';
    else if (state.serverHealthy) block.style.color='#49c06b';
    else block.style.color='#d2b046';
  });
  statuses.forEach(status => {
    status.classList.toggle('agent-on', agent);
    if (!state.serverRunning) status.textContent='stopped';
    else if (agent) status.textContent='agent working';
    else if (state.serverHealthy) status.textContent='running';
    else status.textContent='starting...';
  });
  if (gLast && gTetherOff === !!state.serverRunning) renderGrease(gLast);
}

let exRows = [];
function renderEx() {
  const q = (document.getElementById('exQuery')?.value || '').trim().toLowerCase();
  const lib = (document.getElementById('exLibrary')?.value || '').trim().toLowerCase();
  const filtered = exRows.filter(r => {
    const full = (r.library + ' ' + r.example + ' ' + r.fullPath).toLowerCase();
    return (!lib || r.library.toLowerCase().includes(lib)) && (!q || full.includes(q));
  });
  const countEl = document.getElementById('exCount');
  const outEl = document.getElementById('exOut');
  if (countEl) countEl.textContent = filtered.length + ' example sketches';
  if (!outEl) return;
  if (!filtered.length) { outEl.innerHTML = '<div class="muted" style="font-size:10px">No examples found.</div>'; return; }
  outEl.innerHTML = filtered.map(r =>
    '<div class="mgr-item" style="cursor:pointer" data-path="' + esc(r.fullPath) + '">' +
    '<div class="name">' + esc(r.example) + '</div>' +
    '<div class="meta">' + esc(r.library) + '</div></div>'
  ).join('');
  outEl.querySelectorAll('.mgr-item').forEach(el => {
    el.addEventListener('click', () => vscode.postMessage({type:'openExample', path: el.getAttribute('data-path') || ''}));
  });
}

window.addEventListener('message', event => {
  const msg = event.data;
  if (msg.type === 'state') {
    const s = msg.state || {};
    document.getElementById('portVal').textContent = s.port || '?';
    const fqbnVal = s.fqbn || '?';
    document.getElementById('fqbnVal').textContent = fqbnVal;
    if (fqbnVal === '?') {
      document.getElementById('fqbnVal').innerHTML += '<br><span style="font-size:8px;color:#ffb4b4;line-height:1.2">Unknown board. Access ||Managers||.</span>';
    }
    setServer(s);
    const sn = document.getElementById('setupNeeded');
    if (sn) sn.style.display = s.setupNeeded ? 'block' : 'none';
    setSerial(!!s.serialActive);
    if (s.accentColor) {
      const block = document.getElementById('accentBlock');
      if (block) block.style.color = s.accentColor;
      const idx = ACCENT_COLORS.indexOf(s.accentColor);
      if (idx !== -1) accentIndex = idx;
    }
  } else if (msg.type === 'accentColor') {
    const block = document.getElementById('accentBlock');
    if (block) block.style.color = msg.color;
    const idx = ACCENT_COLORS.indexOf(msg.color);
    if (idx !== -1) accentIndex = idx;
  } else if (msg.type === 'verifyResult') {
    if (!msg.success) bubbleError();
  } else if (msg.type === 'examples') {
    exRows = Array.isArray(msg.rows) ? msg.rows : [];
    const errEl = document.getElementById('exError');
    if (errEl) errEl.textContent = '';
    renderEx();
  } else if (msg.type === 'exError') {
    const errEl = document.getElementById('exError');
    if (errEl) errEl.textContent = msg.error || 'Unknown error';
  } else if (msg.type === 'uploadResult') {
    if (msg.success) uploadOk(); else bubbleError();
  } else if (msg.type === 'bubble') {
    bubbleFromServer(String(msg.state || 'idle'));
  } else if (msg.type === 'serialLine') {
    if (B.serial && B.motion) queued = String(msg.line || '').slice(0, 48);
  } else if (msg.type === 'motion') {
    setMotion(msg.on !== false);
  } else if (msg.type === 'boards') {
    boards = Array.isArray(msg.rows) ? msg.rows : [];
    if (!selectedFqbn && boards.length) selectedFqbn = boards[0].fqbn;
    if (typeof msg.query === "string") boardQuery = msg.query;
    boardMode = 'installed';
    selectedCatalogId = null;
    const iBtn = document.getElementById('installBoardBtn');
    if (iBtn) iBtn.textContent = '||Install new board \u2197||';
    renderBoards();
  } else if (msg.type === 'libraries') {
    libs = Array.isArray(msg.rows) ? msg.rows : [];
    if (!selectedLib && libs.length) selectedLib = libs[0].name;
    renderLibs();
  } else if (msg.type === 'mgrError') {
    const el = document.getElementById('mgrErr');
    // Double backslash below: this script sits in a TS template literal, where a
    // single one turns into a real line break and the whole script stops parsing.
    const lines = String(msg.error || '').split('\\n').map(l => l.trim()).filter(Boolean);
    const explained = lines.filter(l => /^(Why|What):/.test(l));
    if (el) el.textContent = (explained.length ? explained : lines.slice(-2)).join('\\n');
    const iBtn = document.getElementById('installBoardBtn');
    if (iBtn && iBtn.textContent === '||Installing...||') iBtn.textContent = '||Install new board \u2197||';
  } else if (msg.type === 'greaseStatus') {
    renderGrease(msg.status || {});
  } else if (msg.type === 'greaseError') {
    const el = document.getElementById('gErr');
    if (el) el.textContent = String(msg.error || '');
  } else if (msg.type === 'switchTab') {
    switchTab(msg.tab === 'grease' ? 'grease' : 'board');
  } else if (msg.type === 'gitInstalled') {
    const g = document.getElementById('libGitInput');
    if (g) g.value = '';
  }
});

let boards = [];
let libs = [];
let selectedFqbn = "";
let selectedLib = "";
let boardQuery = "";
let boardMode = 'installed';
let selectedCatalogId = null;
const BOARD_CATALOG = ${r};
const INITIAL_TAB = ${o};

function esc(s){return String(s).replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;').replace(/"/g,'&quot;').replace(/'/g,'&#39;');}

function renderBoards() {
  if (boardMode === 'catalog') { renderCatalogBoards(); return; }
  const filtered = boards.filter((b) => {
    const q = boardQuery.toLowerCase();
    if (!q) return true;
    return (b.name + " " + b.fqbn + " " + b.platform).toLowerCase().includes(q);
  });
  if (!filtered.length) {
    document.getElementById('boardsList').innerHTML = "<div class='muted' style='font-size:10px'>No installed boards found.</div>";
    return;
  }
  document.getElementById('boardsList').innerHTML = filtered.map((b) => {
    const border = selectedFqbn === b.fqbn ? "border-color: #58aa58; box-shadow: 0 0 0 1px #58aa58 inset;" : "";
    return "<div class='mgr-item' style='" + border + "' data-fqbn='" + esc(b.fqbn) + "'><div class='name'>" + esc(b.name) + "</div><div class='meta'>" + esc(b.fqbn) + " | " + esc(b.platform) + " @ " + esc(b.version) + "</div></div>";
  }).join("");
  document.querySelectorAll("#boardsList .mgr-item").forEach((el) => {
    el.addEventListener("click", () => {
      selectedFqbn = el.getAttribute("data-fqbn") || "";
      renderBoards();
    });
  });
}

function renderCatalogBoards() {
  const q = boardQuery.toLowerCase();
  const filtered = q ? BOARD_CATALOG.filter(b =>
    (b.name + ' ' + b.vendor + ' ' + b.id + ' ' + b.boards.join(' ')).toLowerCase().includes(q)
  ) : BOARD_CATALOG;
  if (!filtered.length) {
    document.getElementById('boardsList').innerHTML = "<div class='muted' style='font-size:10px'>No matching boards.</div>";
    return;
  }
  document.getElementById('boardsList').innerHTML = filtered.map((b) => {
    const border = selectedCatalogId === b.id ? "border-color: #f6c542; box-shadow: 0 0 0 1px #f6c542 inset;" : "";
    return "<div class='mgr-item' style='" + border + "' data-catalog-id='" + esc(b.id) + "'><div class='name'>" + esc(b.name) + "</div><div class='meta'>" + esc(b.vendor) + " | " + esc(b.boards.slice(0,3).join(', ')) + "</div></div>";
  }).join("");
  document.querySelectorAll("#boardsList [data-catalog-id]").forEach((el) => {
    el.addEventListener("click", () => {
      selectedCatalogId = el.getAttribute("data-catalog-id") || "";
      renderCatalogBoards();
    });
    el.addEventListener("dblclick", () => {
      selectedCatalogId = el.getAttribute("data-catalog-id") || "";
      installSelectedCatalogBoard();
    });
  });
}

function clearMgrErr() { const el = document.getElementById('mgrErr'); if (el) el.textContent = ''; }

function installSelectedCatalogBoard() {
  if (!selectedCatalogId) return;
  clearMgrErr();
  const entry = BOARD_CATALOG.find(b => b.id === selectedCatalogId);
  if (!entry) return;
  const btn = document.getElementById('installBoardBtn');
  if (btn) btn.textContent = '||Installing...||';
  vscode.postMessage({ type: 'boardCatalogInstall', id: entry.id });
}

function renderLibs() {
  if (!libs.length) {
    document.getElementById('libsList').innerHTML = "<div class='muted' style='font-size:10px'>No libraries loaded.</div>";
    return;
  }
  document.getElementById('libsList').innerHTML = libs.map((l) => {
    const right = l.version || l.author || l.sentence || "";
    const border = selectedLib === l.name ? "border-color: #58aa58; box-shadow: 0 0 0 1px #58aa58 inset;" : "";
    return "<div class='mgr-item' style='" + border + "' data-lib='" + esc(l.name) + "'><div class='name'>" + esc(l.name) + "</div><div class='meta'>" + esc(right) + "</div></div>";
  }).join("");

  document.querySelectorAll("#libsList .mgr-item").forEach((el) => {
    el.addEventListener("click", () => {
      selectedLib = el.getAttribute("data-lib") || "";
      renderLibs();
    });
  });
}

const $ = (id) => document.getElementById(id);
$("updateIdx")?.addEventListener("click", () => {
  boardMode = 'installed';
  selectedCatalogId = null;
  const btn = $("installBoardBtn");
  if (btn) btn.textContent = '||Install new board \u2197||';
  vscode.postMessage({ type: "updateIndexes" });
});
$("boardQuery")?.addEventListener("keydown", (e) => {
  if(e.key === 'Enter') {
    boardQuery = $("boardQuery").value || "";
    if (boardMode === 'catalog') { renderCatalogBoards(); }
    else { vscode.postMessage({ type: "boardSearch", query: boardQuery }); }
  }
});
$("boardQuery")?.addEventListener("input", () => {
  if (boardMode === 'catalog') { boardQuery = $("boardQuery").value || ""; renderCatalogBoards(); }
});
$("installBoardBtn")?.addEventListener("click", () => {
  if (boardMode === 'installed') {
    boardMode = 'catalog';
    selectedCatalogId = null;
    boardQuery = $("boardQuery")?.value || "";
    const btn = $("installBoardBtn");
    if (btn) btn.textContent = '||Confirm install||';
    renderCatalogBoards();
  } else {
    installSelectedCatalogBoard();
  }
});
$("chooseTargetBtn")?.addEventListener("click", () => vscode.postMessage({ type: "chooseTarget", fqbn: selectedFqbn }));
$("recoveryUploadBtn")?.addEventListener("click", () => vscode.postMessage({ type: "recoveryUpload" }));
$("libListBtn")?.addEventListener("click", () => vscode.postMessage({ type: "libList" }));
$("libQuery")?.addEventListener("keydown", (e) => {
  if(e.key === 'Enter') {
    vscode.postMessage({ type: "libSearch", query: $("libQuery").value });
  }
});
$("libInstallBtn")?.addEventListener("click", () => { clearMgrErr(); vscode.postMessage({ type: "libInstallSelected", name: selectedLib }); });

// \u2500\u2500 Help panel \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500
function greaseRefresh(checks) {
  const el = document.getElementById('gErr');
  if (el) el.textContent = '';
  vscode.postMessage({ type: 'greaseRefresh', checks: !!checks });
}
function gCode(text) {
  return '<span class="g-code">' + esc(text) + '</span><a class="c-std" href="#" data-copy="' + esc(text) + '">||Copy||</a>';
}
function gSkill(st) {
  if (st === 'current') return '<span class="g-ok">&#10003;</span>';
  if (st === 'outdated') return '<span class="g-warn">old</span>';
  if (st === 'edited') return '<span class="g-warn">edited by you</span>';
  if (st === 'foreign') return '<span class="g-warn">another grease skill</span>';
  if (st === 'n/a') return '<span class="dim">&ndash;</span>';
  return '<span class="g-bad">&#10007;</span>';
}
function gTools(r) {
  if (r.tools === 'added') return '<span class="g-ok">&#10003;</span>';
  if (r.tools === 'built-in') return '<span class="g-ok">&#10003; built in</span>';
  if (r.tools === 'unreadable') return '<span class="g-bad">config unreadable</span>';
  return '<a class="c-green" href="#" data-add="' + esc(r.id) + '">||' + (r.tools === 'old' ? 'Replace old entry' : 'Add tools') + '||</a>';
}
function gLine(mark, cls, text, right, tip) {
  return '<div class="g-row g-line"' + (tip ? ' title="' + esc(tip) + '"' : '') + '><span><span class="' + cls + '">' + mark + '</span> ' + text + '</span><span>' + (right || '') + '</span></div>';
}
const gSkillOk = (r) => ['current', 'edited', 'n/a'].includes(r.skill);
const gToolsOk = (r) => ['added', 'built-in', 'n/a'].includes(r.tools);
function gAgentLine(r) {
  const tip = 'skill: ' + r.skill + ', tools: ' + r.tools;
  if (gSkillOk(r) && gToolsOk(r)) return gLine('&#10003;', 'g-ok', esc(r.name), '<span class="dim">' + esc(r.invoke) + '</span>', tip);
  let right = gTools(r);
  if (gToolsOk(r)) right = r.skill === 'foreign' ? gSkill(r.skill) : '<a class="c-green" href="#" data-act="gReinstall">||Reinstall skill||</a>';
  return gLine('&#9675;', 'g-warn', esc(r.name), right, tip);
}
function renderGrease(s) {
  gLast = s;
  const port = document.getElementById('gPort');
  if (port) port.textContent = s.tetherPort ? ':' + s.tetherPort : '';
  const c = s.cli;
  const n = s.node;
  const j = s.job;
  const busy = j && !j.failed;
  // Problems go on top, above Details.
  let fix = '';
  let count = 0;
  let setup = '';
  let installing = false;
  if (c && c.state === 'ok') {
    setup += gLine('&#10003;', 'g-ok', 'arduino-cli ' + esc(c.version || ''), '', c.path);
  } else if (c && c.state === 'offpath') {
    setup += gLine('&#9888;', 'g-warn', 'arduino-cli ' + esc(c.version || '') + ', not on PATH', '', c.path);
    setup += '<div class="g-note">Grease uses this copy. For terminals and other agents, run once:</div>' + gCode(c.fix);
  } else if (c && !busy) {
    const line = gLine('&#10007;', 'g-bad', 'arduino-cli', '<a class="c-amber" href="#" data-act="gPrepareCli">||Install &#8599;||</a>');
    setup += line;
    fix += line;
    count++;
    installing = true;
    if (j && j.kind === 'cli' && j.failed) fix += '<div class="g-note">Last try: ' + esc(j.failed) + '</div>';
    fix += '<div class="g-note">Or install it yourself:</div>' + gCode(c.install);
    (c.copies || []).forEach((p) => {
      fix += '<div class="g-row"><span class="g-path">' + esc(p) + '</span><a class="c-amber" href="#" data-usecli="' + esc(p) + '">||Use this copy||</a></div>';
    });
  }
  if (c && c.state !== 'missing' && !busy) {
    const retry = '<a class="c-amber" href="#" data-act="gPrepareCli">||Retry &#8599;||</a>';
    if (j && j.kind === 'firstRun' && j.failed) { fix += gLine('&#10007;', 'g-bad', 'first run: ' + esc(j.failed), retry); count++; }
    else if (c.firstRun && c.firstRun.length) { fix += gLine('&#9888;', 'g-warn', 'first run not finished: ' + esc(c.firstRun.join(', ')), retry); count++; }
  }
  if (n && n.state === 'ok') {
    setup += gLine('&#10003;', 'g-ok', 'Node.js ' + esc(n.version || ''), '', n.path);
  } else if (n && n.state === 'editor') {
    setup += gLine('&#9888;', 'g-warn', 'Node.js not found') + '<div class="g-note">Outside agents use this editor&#39;s Node. Install Node.js so they keep working:</div>' + gCode(n.install);
  } else if (n) {
    const line = gLine('&#10007;', 'g-bad', 'Node.js');
    setup += line;
    fix += line + '<div class="g-note">Needed for agents outside the editor:</div>' + gCode(n.install);
    count++;
    installing = true;
  }
  (s.linux || []).forEach((w) => {
    const row = gLine(w.blocking ? '&#10007;' : '&#9888;', w.blocking ? 'g-bad' : 'g-warn', esc(w.title), '', '')
      + '<div class="g-note">' + esc(w.why) + '</div>'
      + (w.fix ? gCode(w.fix) : '')
      + (w.note ? '<div class="g-note">' + esc(w.note) + '</div>' : '');
    setup += row;
    if (w.blocking) { fix += row; count++; }
  });
  if (installing) fix += '<div class="g-note">Restart the editor after installing.</div>';
  gTetherOff = lastTether.serverRunning === false;
  if (gTetherOff) {
    fix += gLine('&#10007;', 'g-bad', 'AI Tether off', '<a class="c-amber" href="#" data-tether>||Turn on||</a>');
    count++;
  }

  const rows = Array.isArray(s.agents) ? s.agents : [];
  const found = rows.filter((r) => r.present);
  const absent = rows.filter((r) => !r.present);
  const absentLinks = absent.map((r) => '<a class="dim" href="#" data-open="' + esc(r.id) + '">' + esc(r.name) + '</a>').join(', ');
  found.filter((r) => !gSkillOk(r) || !gToolsOk(r)).forEach((r) => {
    fix += gAgentLine(r);
    count++;
  });
  if (rows.length && !found.length) {
    fix += '<div class="g-note">No agent found. Install one: ' + absentLinks + '</div>';
    count++;
  }

  let verdict;
  if (busy) {
    const f = typeof j.fraction === 'number' ? Math.max(0, Math.min(1, j.fraction)) : null;
    const bar = f === null ? '~~~~~~~~~~' : '#'.repeat(Math.round(f * 10)) + '.'.repeat(10 - Math.round(f * 10));
    verdict = '<div class="g-row"><span class="g-warn" style="white-space:pre">[' + bar + ']</span><span>' + (f === null ? '' : Math.round(f * 100) + '% ') + esc(j.label) + '</span></div>';
  } else if (!c || !n) {
    verdict = '<span class="dim">checking...</span>';
  } else if (count) {
    verdict = '<span class="g-bad">&#10007; ' + count + ' to fix</span>';
  } else {
    verdict = '<span class="g-ok">&#10003; Ready</span><div class="g-hint">Type <b>/grease</b>, then ask.</div>';
  }
  document.getElementById('gVerdict').innerHTML = verdict;
  document.getElementById('gFix').innerHTML = fix;
  document.getElementById('gSetup').innerHTML = setup;
  let a = found.map(gAgentLine).join('');
  if (found.length && absent.length) a += '<div class="g-note">Not found: ' + absentLinks + '</div>';
  document.getElementById('gAgents').innerHTML = a;
}
function gToggleDetails() {
  gOpen = !gOpen;
  document.getElementById('gDetails').hidden = !gOpen;
  document.getElementById('gToggle').innerHTML = (gOpen ? '&#9662;' : '&#9656;') + ' Details';
}
document.getElementById('panel-grease')?.addEventListener('click', (e) => {
  const t = e.target.closest('[data-copy],[data-usecli],[data-act],[data-add],[data-open],[data-toggle],[data-tether]');
  if (!t) return;
  e.preventDefault();
  if (t.hasAttribute('data-toggle')) gToggleDetails();
  else if (t.hasAttribute('data-tether')) cmd('arduinoMcp.toggleServer');
  else if (t.hasAttribute('data-copy')) vscode.postMessage({ type: 'gCopy', text: t.getAttribute('data-copy') });
  else if (t.hasAttribute('data-usecli')) vscode.postMessage({ type: 'gUseCli', path: t.getAttribute('data-usecli') });
  else if (t.hasAttribute('data-act')) vscode.postMessage({ type: t.getAttribute('data-act') });
  else if (t.hasAttribute('data-add')) { t.textContent = '||Adding...||'; vscode.postMessage({ type: 'gAddTools', id: t.getAttribute('data-add') }); }
  else if (t.hasAttribute('data-open')) vscode.postMessage({ type: 'gOpen', id: t.getAttribute('data-open') });
});
if (INITIAL_TAB !== 'board') switchTab(INITIAL_TAB);

function onUploadClick(){cmd('arduinoMcp.upload');}
    </script>
  </body>
</html>`}};var Js="Grease VS",Zn="grease.skillRemoved",Eo="grease.toolsOffered";function ze(t){return new Promise(e=>setTimeout(e,t))}async function Xs(t){let e=i.window.createOutputChannel(Js);t.subscriptions.push(e);let n=j.join(on.homedir(),"Grease");try{vt.mkdirSync(n,{recursive:!0})}catch{}let r=t.asAbsolutePath(j.join("resources","arduino_docs.h")),o=Xr(r);t.subscriptions.push(Yr(o,e));let s=i.workspace.createFileSystemWatcher("**/*.ino");s.onDidCreate(l=>{e.appendLine(`[Grease] New sketch detected: ${l.fsPath}`),i.workspace.openTextDocument(l).then(c=>i.window.showTextDocument(c,{preview:!1}),()=>{})}),t.subscriptions.push(s),i.workspace.getConfiguration().update("output.smartScroll.enabled",!1,i.ConfigurationTarget.Global).then(void 0,()=>{});let a="arduinoMcp.firstInstallDone_1_0_6";if(!t.globalState.get(a)){try{let l=i.workspace.getConfiguration("workbench");await l.update("colorTheme","Grease",i.ConfigurationTarget.Global),await l.update("activityBar.location","top",i.ConfigurationTarget.Global),e.appendLine("First install: Applied Grease theme and moved Activity Bar to top.")}catch(l){e.appendLine("First install theme setup failed: "+(l instanceof Error?l.message:String(l)))}await t.globalState.update(a,!0)}let u=i.languages.createDiagnosticCollection("arduino");t.subscriptions.push(u);let m=null,S=!1,k=!1,$=null,w=!1,I=Promise.resolve(),J=-1,Q=new i.EventEmitter;t.subscriptions.push(Q),/^visual studio code/i.test(i.env.appName)&&typeof i.lm?.registerMcpServerDefinitionProvider=="function"&&t.subscriptions.push(i.lm.registerMcpServerDefinitionProvider("grease.aiTether",{onDidChangeMcpServerDefinitions:Q.event,provideMcpServerDefinitions:()=>m?[new i.McpHttpServerDefinition("Grease",i.Uri.parse(`http://127.0.0.1:${m.port}/mcp`),{"x-grease-auth":m.authKey},String(t.extension.packageJSON.version))]:[]}));let ee=async()=>{if(m)return!0;try{return m=await vo(t,e,3333,i.env.appName),lo(`http://127.0.0.1:${m.port}`),ao(m.authKey),Q.fire(),!0}catch(l){return e.appendLine(`Failed to start AI Tether: ${l instanceof Error?l.message:String(l)}`),!1}},be=async()=>{m&&(await m.stop(),m=null,S=!1,k=!1,Q.fire(),e.appendLine("AI Tether stopped on this computer."))};await ee();let F=i.window.createStatusBarItem(i.StatusBarAlignment.Left,100);F.name="Grease Target",F.command="arduinoMcp.refreshPortsBoards",F.show(),t.subscriptions.push(F);let te=new en(e,l=>v.postSerialLine(l)),sn=new nn(e,t.extensionUri),yt=new Zt(t,e),O=[],h=null,v=new He(t,e,{chooseTarget:async l=>{let c=l.split(":").slice(0,2).join(":"),p=O.find(b=>b.fqbn?.startsWith(c+":")&&typeof b.port=="string")??O.find(b=>b.port===h?.port)??O.find(b=>typeof b.port=="string")??null,f=p?.port??h?.port??null,y=p??(f?O.find(b=>b.port===f):null),A=Vt(y??null);if(A.length>0){let b=await zt(t);for(let x of A)b[x]={fqbn:l};await jn(t,b),e.appendLine(`[BoardMemory] Saved (${A.join(", ")}) \u2192 ${l}`)}else f&&e.appendLine(`[BoardMemory] Warning: no identifiers found for ${f} \u2014 memory not saved`);h={port:f,fqbn:l,userChosen:!0},await ge(t,h),await xe(h),f?_e(h):(ln(`${l} \u2014 no port`),i.window.showInformationMessage(`Grease: Board type set to ${l}. Connect the board and use Recovery Upload.`)),G()},recoveryUpload:async()=>{await i.commands.executeCommand("arduinoMcp.recoveryUpload")},uploadFirmwareToTarget:async l=>{let c=h?.port??O[0]?.port??null;if(!c){i.window.showWarningMessage("Grease: No port detected. Connect a board first.");return}h={port:c,fqbn:l,userChosen:!0},await ge(t,h),await xe(h),_e(h),G(),await Bo(l,c)},serialOff:async()=>{te.isConnected&&(await te.stop(),e.appendLine("Serial disconnected (panel switch)."),G())},cycleAccent:async l=>{let c=i.workspace.getConfiguration("workbench"),f={...c.get("colorCustomizations")||{},"statusBar.background":l,"statusBar.noFolderBackground":l,"statusBar.debuggingBackground":l,"statusBarItem.remoteBackground":l,focusBorder:l,"activityBarBadge.background":l,"panelTitle.activeBorder":l},y=i.workspace.workspaceFolders&&i.workspace.workspaceFolders.length>0?i.ConfigurationTarget.Workspace:i.ConfigurationTarget.Global;await c.update("colorCustomizations",f,y),e.appendLine(`Accent color set to ${l} (Target: ${y===i.ConfigurationTarget.Workspace?"Workspace":"Global"})`),G()},onBoardInstalled:async()=>{await me()},onSetupChecked:l=>{w=!l,G()},prepareCli:async()=>{let l=await pn(!0);return l&&await me(),l}});t.subscriptions.push(i.window.registerWebviewViewProvider(He.viewType,v));let E=async l=>{let c=jt({hostApp:i.env.appName}).filter(y=>y.present&&(y.tools==="missing"||y.tools==="old"));if(!c.length){l&&i.window.showInformationMessage("Grease: every agent found here already has the Grease tools.");return}if(!l&&(await t.globalState.update(Eo,!0),await i.window.showInformationMessage(`Grease: add Grease tools to ${c.map(A=>A.name).join(", ")}?`,"Choose","Not now")!=="Choose"))return;let p=await i.window.showQuickPick(c.map(y=>({label:y.name,description:y.tools==="old"?"replaces the old Arduino Grease entry":"",detail:y.configFile??void 0,picked:!0,id:y.id})),{canPickMany:!0,title:"Add the Grease tools to these agents (a backup of each config is kept)"});if(!p?.length)return;let f=[];for(let y of p){let A=await Ft(y.id,{editorNode:process.execPath});e.appendLine(`[Grease] Add tools (${y.label}): ${A.message}${A.manual?` ${A.manual}`:""}`),A.ok&&f.push(y.label),!A.ok&&A.manual?await i.window.showWarningMessage(`Grease: ${y.label}: ${A.message} ${A.manual}`,"Copy command")&&await i.env.clipboard.writeText(A.manual):A.ok||i.window.showWarningMessage(`Grease: ${y.label}: ${A.message}`)}f.length&&i.window.showInformationMessage(`Grease: tools added to ${f.join(", ")}. Restart them.`),v.postGrease()};{let l=!t.globalState.get(Zn,!1),{skills:c}=Ur(t,e,{skill:l}),p=c.filter(f=>f.action==="installed"||f.action==="replaced");if(p.length){for(let f of p)e.appendLine(`[Grease] Skill for ${f.readers}: ${f.file}`);i.window.showInformationMessage("Grease: /grease skill installed for your agents.","Show","Remove").then(f=>{f==="Show"?i.commands.executeCommand("arduinoMcp.showSkill"):f==="Remove"&&i.commands.executeCommand("arduinoMcp.removeSkill")})}if(!t.globalState.get(Eo,!1)){let f=setTimeout(()=>{E(!1)},p.length?4e3:1500);t.subscriptions.push({dispose:()=>clearTimeout(f)})}}let U=new Set,Z=null,Ve=0,z=new Ut,X=null;t.subscriptions.push({dispose:()=>{X&&clearTimeout(X)}});let er=0,an=!1;h=null;let G=()=>{let c=i.workspace.getConfiguration("workbench").get("colorCustomizations")||{};v.setState({port:h?.port??null,fqbn:h?.fqbn??null,connectedPorts:Array.from(U),serverRunning:m!==null,serverHealthy:S,lastScanAtMs:$,serialActive:te.isConnected,uploading:!!Z?.uploading,accentColor:c["statusBar.background"]||"#007ACC",setupNeeded:w,tetherPort:m?.port??null})},ln=l=>{F.text=`$(warning) ${l}`,F.backgroundColor=new i.ThemeColor("statusBarItem.warningBackground"),F.tooltip="Grease needs a board/port selection"},_e=l=>{l.fqbn&&l.port?(F.text=`$(circuit-board) ${l.fqbn} @ ${l.port}`,F.backgroundColor=void 0,F.tooltip="Grease target"):l.fqbn&&!l.port?(F.text=`$(circuit-board) ${l.fqbn} \u2014 Recovery mode`,F.backgroundColor=new i.ThemeColor("statusBarItem.warningBackground"),F.tooltip="Board type set, no port detected. Use Recovery Upload."):(F.text=`$(plug) ${l.port} (board unknown)`,F.backgroundColor=new i.ThemeColor("statusBarItem.warningBackground"),F.tooltip="Port detected but board not resolved. Use 'arduino-cli board list' and 'arduino-cli core install <package>' to install the correct driver.")},fe=i.window.createStatusBarItem(i.StatusBarAlignment.Left,100);fe.name="Grease Install",t.subscriptions.push(fe);let Ke=(l,c={})=>{fe.text=l,fe.tooltip=c.tooltip,fe.command=c.command??"arduinoMcp.openGreasePanel",fe.backgroundColor=c.error?new i.ThemeColor("statusBarItem.errorBackground"):void 0,fe.show(),F.hide()},wt=()=>{fe.hide(),F.show()},St=14,Je=!1,dn=!1,cn=!1,tr=()=>{pn(!0).then(l=>l&&me())};Pt(l=>{if(l.phase==="start")Ke(`$(sync~spin) Grease: first run \xB7 ${St} MB`,{tooltip:"arduino-cli downloads its board list and USB tools once"}),v.setJob({kind:"firstRun",label:`first run \xB7 ${St} MB`}),e.appendLine("[Setup] arduino-cli first run: downloading its board list and USB tools (once)...");else if(l.phase==="done"){wt(),v.setJob(null),e.appendLine("[Setup] arduino-cli first run done.");let c=Je?`Grease: arduino-cli ${Be} installed and ready (${Math.round((it()?.bytes??17e6)/1e6)+St} MB, once).`:`Grease: arduino-cli ready (${St} MB, once).`;Je=!1,dn=!1,cn=!1,i.window.showInformationMessage(c),v.refreshCli()}else{if(Ke("$(warning) Grease: first run failed",{tooltip:"No internet, or blocked. Grease retries every minute. Click for Help.",error:!0}),v.setJob({kind:"firstRun",label:"first run",failed:"no internet, or blocked"}),cn||e.appendLine(`[Setup] ${l.message??"arduino-cli first run failed."}`),cn=!0,dn)return;dn=!0,i.window.showErrorMessage("Grease: arduino-cli first run failed (no internet, or blocked).","Retry").then(c=>c==="Retry"&&tr())}});let Xe=null,pn=(l=!1)=>Xe||(Xe=(async()=>{let c=ve({refresh:!0});if(c)c.source==="grease"?e.appendLine(`[Setup] arduino-cli: ${c.path} (installed by Grease)`):c.onPath?e.appendLine(`[Setup] arduino-cli: ${c.path}`):(e.appendLine(`[Setup] arduino-cli: ${c.path} (found in a ${c.source==="saved"?"folder you picked":"standard install folder"}, NOT on PATH).`),e.appendLine(`[Setup] Grease uses it directly; terminals and other agents will not find it until you run: ${Mt(j.dirname(c.path))}`));else{let f=Math.round((it()?.bytes??17e6)/1e6),y=`arduino-cli ${Be}`;Ke(`$(sync~spin) Grease: ${y} \xB7 0% of ${f} MB`,{tooltip:"Grease installs arduino-cli into ~/.grease/bin"}),v.setJob({kind:"cli",label:y,fraction:0}),e.appendLine(`[Setup] arduino-cli not found: installing ${y} (${f} MB) into ~/.grease/bin...`);let A=-1,b=await vr({onProgress:x=>{let T=Math.floor(x*100);T!==A&&(A=T,fe.text=`$(sync~spin) Grease: ${y} \xB7 ${T}% of ${f} MB`,T%5===0&&v.setJob({kind:"cli",label:y,fraction:x}))}});if(!b.success){let x="error"in b?b.error:"unknown error";return w=!0,G(),e.appendLine(`[Setup] arduino-cli did not install: ${x}`),Ke("$(warning) Grease: arduino-cli not installed",{tooltip:x,error:!0}),v.setJob({kind:"cli",label:y,failed:x}),i.window.showErrorMessage(`Grease: arduino-cli did not install (${x}).`,"Retry","Install by hand").then(T=>{T==="Retry"?tr():T==="Install by hand"&&i.commands.executeCommand("arduinoMcp.openGreasePanel")}),!1}Je=!0,w=!1,wt(),v.setJob(null),G(),e.appendLine(`[Setup] Installed arduino-cli ${b.version}: ${b.path}`)}let p=await Cn({force:l});return p.success&&!p.ran&&Je&&(Je=!1,i.window.showInformationMessage(`Grease: arduino-cli ${Be} installed (${Math.round((it()?.bytes??17e6)/1e6)} MB).`)),p.success&&(wt(),v.setJob(null)),v.refreshCli(),p.success})().finally(()=>{Xe=null}),Xe),Pe=async(l=!1)=>{if(!m){S=!1,v.setBubble("idle"),G();return}try{let p=await(await fetch(`http://127.0.0.1:${m.port}/state`,{headers:ct(m.authKey)})).json(),f=!!p.target;S=f,v.setBubble(Xn(p)),Z=p,(p.uploading||p.compiling)&&(Ve=Date.now()),typeof p.portChangeVersion=="number"&&p.portChangeVersion!==er&&(er=p.portChangeVersion,p.uploading||me()),(p.targetBy==="agent"||p.targetBy==="developer")&&typeof p.targetVersion=="number"&&p.targetVersion!==J&&(J=p.targetVersion,p.target?.fqbn&&(h={port:p.target.port??null,fqbn:p.target.fqbn,userChosen:!0},await ge(t,h),_e(h),e.appendLine(`[Grease] ${p.targetBy==="agent"?"Agent":"Developer"} chose ${p.target.fqbn} @ ${p.target.port??"no port"}.`))),f?k&&(e.appendLine("AI Tether recovered."),k=!1):(k||e.appendLine("Problem with AI Tether."),k=!0),l&&e.appendLine(`Server status: ${f?"running and healthy":"running but unhealthy"}`)}catch(c){S=!1,v.setBubble("idle"),k||e.appendLine("Problem with AI Tether: it does not answer."),l&&e.appendLine(`Server health check failed: ${c instanceof Error?c.message:String(c)}`),k=!0}G()},Ye=new Set,xt=new Set,Qe=null,un=async l=>{let c=ot(l);if(!c||Qe)return!1;let p=c.name.split(" (")[0];e.appendLine(`[Grease] Installing ${c.name} (${c.id}, online)...`);try{await Xt()}catch{}Qe=new AbortController,Ke(`$(sync~spin) Grease: ${p} \xB7 starting (click to cancel)`,{command:"arduinoMcp.cancelInstall",tooltip:`Installing ${c.id}`}),v.setJob({kind:"core",label:p});let y;try{y=le(await It(c.id,{signal:Qe.signal,onData:A=>{e.append(A);let b=A.split(/[\r\n]+/).map(x=>x.trim()).filter(Boolean).pop();b&&(fe.text=`$(sync~spin) Grease: ${p} \xB7 ${b.slice(0,40)} (click to cancel)`)}}))}finally{Qe=null,wt(),v.setJob(null);try{await Yt()}catch{}}if(y.cancelled)return e.appendLine(`[Grease] ${c.name}: install cancelled.`),!1;if(!y.success){e.appendLine(y.stderr);let A=y.stderr.split(`
`).find(b=>b.startsWith("Why:"))?.slice(5)??"See Output \u203A Grease VS.";return i.window.showErrorMessage(`Grease: ${p} did not install. ${A}`,"Retry").then(b=>{b==="Retry"&&un(c.id).then(x=>x&&me())}),!1}return i.window.showInformationMessage(`Grease: ${p} installed.`),!0};t.subscriptions.push(i.commands.registerCommand("arduinoMcp.cancelInstall",()=>Qe?.abort()));let Mo=async(l,c)=>{let p=ot(l),f=`${c}|${l}`;!p||xt.has(f)||(xt.add(f),!await mr(l)&&await un(p.id)&&await me())},Ro=async()=>{await I,$=Date.now(),O=(await dt(e)).candidates;let c=new Map;for(let g of new Set(O.map(L=>L.port))){let L=O.filter(Oe=>Oe.port===g),B=L[0],ae=B.vid&&B.pid?`${String(B.vid).replace(/^0x/i,"")}:${String(B.pid).replace(/^0x/i,"")}`.toLowerCase():"";if(!xr(ae)||L.length===1&&B.fqbn)continue;let he=await co({port:g,usbId:ae,serialNumber:B.serialNumber??null});if(!he?.chip)continue;c.set(g,{chip:he.chip,mcu:he.mcu});let hn={...B,name:`${he.chip} (chip read over USB)`,fqbn:he.fqbn??void 0};O=O.filter(Oe=>Oe.port!==g).concat(hn)}let p=new Set(O.map(g=>g.port).filter(g=>typeof g=="string")),f=p.size!==U.size||Array.from(p).some(g=>!U.has(g))||Array.from(U).some(g=>!p.has(g));U=p,f&&Ye.clear();for(let g of Array.from(xt))p.has(g.split("|")[0])||xt.delete(g);X&&(clearTimeout(X),X=null),h?.port&&z.isGone(p.has(h.port),Ve)?(e.appendLine(`Port ${h.port} disconnected. Clearing selected target.`),z.reset(),h=null,await ge(t,null),await xe(null)):h?.port&&!p.has(h.port)&&(X=setTimeout(()=>{X=null,Z?.uploading||me()},4250));let y=await zt(t),A=[];for(let g of O)A.push(await(async()=>{let L=c.get(g.port);for(let B of Vt(g))if(y[B]){if(L?.mcu){let he=await Rn(y[B].fqbn);if(he&&he!==L.mcu)continue}let ae=`${g.port}:${B}:${y[B].fqbn}`;return Ye.has(ae)||(e.appendLine(`[BoardMemory] Restored ${y[B].fqbn} for ${g.port} via ${B}`),Ye.add(ae)),{...g,fqbn:y[B].fqbn,fromMemory:!0}}return g})());let b=new Map;for(let g of A){if(typeof g.port!="string")continue;let L=b.get(g.port);(!L||!L.fromMemory&&g.fromMemory)&&b.set(g.port,g)}let x=Array.from(b.values()),T=x.filter(g=>typeof g.fqbn=="string"&&typeof g.port=="string"),ne=x.filter(g=>!g.fqbn&&typeof g.port=="string"&&!!g.vid),W=h?.port?c.get(h.port):void 0;if(h?.port&&h.fqbn&&W?.mcu){let g=await Rn(h.fqbn);if(g&&g!==W.mcu){let L=h.fqbn,B=x.find(ae=>ae.port===h.port)?.fqbn??null;h={port:h.port,fqbn:B,userChosen:!1},e.appendLine(`[Grease] ${h.port} holds an ${W.chip}, not ${g}: target ${L} \u2192 ${B??"none"}`),i.window.showInformationMessage(`Grease: ${h.port} holds an ${W.chip}. Board set to ${B??"none (pick one)"}.`),await ge(t,h),await xe(h,{chipFix:!0})}}if(!h?.userChosen&&T.length===1){let g=T[0],L=g.fromMemory;if(h={port:g.port,fqbn:g.fqbn,userChosen:!1},L){let B=`auto:${g.fqbn}:${g.port}`;Ye.has(B)||(e.appendLine(`[BoardMemory] Restored ${g.fqbn} for ${Fn(g.vid,g.pid)??g.port}`),Ye.add(B))}await ge(t,h),await xe(h,{auto:!0})}else!h?.userChosen&&T.length===0&&ne.length===1&&(h={port:ne[0].port,fqbn:null},await ge(t,h),await xe(h,{auto:!0}));if(!h)x.some(g=>!!g.vid)?ln("Select board/port"):(ln("No board connected"),f&&e.appendLine("No Arduino port detected."));else{_e(h);let g=!!h.port&&p.has(h.port);if(f&&g){e.appendLine(`[Grease] ${h.port} \xB7 ${h.fqbn??"unknown board"}`);let L=x.find(ae=>ae.port===h?.port),B=fr(h.fqbn)??L?.neededCore??null;B&&Mo(B,h.port)}}G()},Ze=null,fn=!1,me=async()=>Ze?(fn=!0,Ze):(Ze=(async()=>{try{do fn=!1,await Ro();while(fn)}finally{Ze=null}})(),Ze),nr=async()=>{if(!an){an=!0,e.show(!0);try{O=(await dt(e)).candidates;let c=await Kr(t,O,h);c&&(h={...c,userChosen:!0},await xe(h),_e(h),G())}finally{an=!1}}},Po=async()=>{let l=oo();return l?Kt(l)?l:(e.appendLine(`Invalid sketch folder (main .ino missing): ${l}`),await i.window.showWarningMessage("Grease: Current folder is not a valid sketch (missing main .ino).","Start Sketch","Cancel")==="Start Sketch"&&await i.commands.executeCommand("arduinoMcp.startSketch"),null):null},rr=async()=>{let l=no();if(l){let f=ro(l);if(f)return f;e.appendLine(`[compile] Could not stage active sketch: ${l}`)}let c=await Po();if(!c)return null;let p=Kt(c);return p?{sketchDir:c,mainIno:p,isTemp:!1}:null},To=(l,c)=>{if(!c.isTemp||!c.originalDir)return l;let p=j.normalize(l),f=j.normalize(c.sketchDir);if(!p.startsWith(f))return l;let y=j.relative(f,p);return c.originalIno&&j.basename(y).toLowerCase()===j.basename(c.mainIno).toLowerCase()&&j.dirname(y)==="."?c.originalIno:j.join(c.originalDir,y)},or=async l=>{e.show(!0);let c=await rr();if(!c)return i.window.showErrorMessage("Grease: No .ino file open or valid sketch folder found."),{ok:!1};let p=c.sketchDir,f=l??h?.fqbn??null;if(!f)return await i.window.showWarningMessage("Grease: Board unknown. Install/select a core so an FQBN is available.","Install core...","Select board/port...")==="Install core..."?await i.commands.executeCommand("arduinoMcp.installCore"):await i.commands.executeCommand("arduinoMcp.refreshPortsBoards"),{ok:!1};let y=c.isTemp?c.originalIno:p;e.appendLine(`Compiling "${y}"...`),c.isTemp&&e.appendLine(`  (staged into temp sketch folder: ${p} \u2014 original folder name '${j.basename(c.originalDir)}' doesn't match .ino basename '${j.basename(c.mainIno,".ino")}')`);let A=i.window.setStatusBarMessage("$(sync~spin) Compiling..."),b;try{b=await uo(p)}catch{return A.dispose(),i.window.showErrorMessage("Grease: Compile failed \u2014 server unreachable."),{ok:!1}}if(A.dispose(),e.appendLine(b.stdout??""),e.appendLine(b.stderr??""),!b.ok){let x=new Map,T=/^(.+):([0-9]+):([0-9]+):\s*(error|warning):\s*(.+)$/gm,ne,W=b.stdout+`
`+b.stderr;for(;(ne=T.exec(W))!==null;){let g=To(ne[1],c),L=Math.max(0,parseInt(ne[2],10)-1),B=Math.max(0,parseInt(ne[3],10)-1),ae=ne[4]==="error"?i.DiagnosticSeverity.Error:i.DiagnosticSeverity.Warning,he=ne[5],hn=new i.Range(L,B,L,B+1),Oe=new i.Diagnostic(hn,he,ae);Oe.source="Grease";let gn=i.Uri.file(g).toString();x.has(gn)||x.set(gn,[]),x.get(gn).push(Oe)}u.clear();for(let[g,L]of x)u.set(i.Uri.parse(g),L);return i.window.showErrorMessage("Grease: Compile failed (see Output)."),{ok:!1,prepared:c}}return u.clear(),i.window.setStatusBarMessage("$(check) Compile OK",4e3),{ok:!0,sketchPath:p,fqbn:f,prepared:c}},Lo=async l=>{e.show(!0),await i.workspace.saveAll(!1);let c=null,p=l?.fqbnOverride??h?.fqbn??null;if(l?.verifyFirst!==!1){let T=await or(p??void 0);if(!T.ok)return!1;c=T.prepared??null,p=T.fqbn??p}else c=await rr();if(!c)return i.window.showErrorMessage("Grease: No .ino file to upload."),!1;let f=c.sketchDir;if(!(l?.portOverride??h?.port??null))return i.window.showWarningMessage("Grease: Select a port first."),await i.commands.executeCommand("arduinoMcp.refreshPortsBoards"),!1;if(!p)return i.window.showWarningMessage("Grease: Select a board first."),await i.commands.executeCommand("arduinoMcp.refreshPortsBoards"),!1;let A=c.isTemp?c.originalIno:f;e.appendLine(`Uploading "${A}"...`);let b=i.window.setStatusBarMessage("$(sync~spin) Uploading..."),x;try{x=await fo(f)}catch{return b.dispose(),i.window.showErrorMessage("Grease: Upload failed \u2014 server unreachable."),!1}if(b.dispose(),e.appendLine(x.stdout??""),e.appendLine(x.stderr??""),v.view?.webview.postMessage({type:"uploadResult",success:x.ok}),!x.ok){let T=(x.stdout??"")+(x.stderr??"");return T.includes("butterfly_recv")||T.includes("initialization failed")||T.includes("avrdude: stk500")?i.window.showErrorMessage("Upload failed: board not in bootloader mode. For Leonardo/32U4 boards: double-tap RESET, then upload within 8 s.","Recovery Upload").then(W=>{W==="Recovery Upload"&&i.commands.executeCommand("arduinoMcp.recoveryUpload")}):i.window.showErrorMessage("Grease: Upload failed (see Output)."),v.view?.webview.postMessage({type:"uploadResult",success:!1}),!1}return i.window.setStatusBarMessage("$(check) Upload OK",4e3),!0},Bo=async(l,c)=>{e.show(!0);let p=await i.window.showInputBox({title:"Programmer",prompt:"Enter programmer (e.g., avrispmkii, usbtinyisp) or leave empty for default",ignoreFocusOut:!1});if(p===void 0)return;e.appendLine(`[Mngrs] Burning bootloader for ${l} on ${c}...`);let f=["burn-bootloader","-b",l,"-p",c];p&&f.push("-P",p);try{await Xt()}catch{}let y=le(await Y(f,void 0,_.upload));try{await Yt()}catch{}if(e.appendLine(y.stdout),e.appendLine(y.stderr),!y.success){i.window.showErrorMessage("Grease: Burn bootloader failed (see Output).");return}i.window.showInformationMessage("Grease: Bootloader burned to target.")},mn=!1;I=pn(),(async()=>{await me(),await ze(3e3),await Pe(!0);let l=setInterval(()=>{mn||(mn=!0,Pe(!1).finally(()=>mn=!1))},500);t.subscriptions.push({dispose:()=>clearInterval(l)})})(),t.subscriptions.push(Hr({output:e,onChange:l=>{e.appendLine(`[Grease] ports: ${l} \u2014 rescanning boards.`),me()},isUploading:()=>!!Z?.uploading})),t.subscriptions.push(i.commands.registerCommand("arduinoMcp.serverStatus",async()=>{if(e.show(!0),e.appendLine("Checking MCP server status..."),!m){e.appendLine("Server status: stopped"),S=!1,G();return}await Pe(!0)}),i.commands.registerCommand("arduinoMcp.refreshServer",async()=>{if(e.show(!0),!m){await ee(),await ze(1e3),await Pe(!0);return}if(m&&!S){await be(),await ze(1e3),await ee(),await ze(1e3),await Pe(!0);return}m&&S&&(await be(),G())}),i.commands.registerCommand("arduinoMcp.startServer",async()=>{e.show(!0),await ee()&&(await Pe(!0),e.appendLine("Server start command completed.")),G()}),i.commands.registerCommand("arduinoMcp.stopServer",async()=>{e.show(!0),await be(),G()}),i.commands.registerCommand("arduinoMcp.toggleServer",async()=>{e.show(!0),m?await be():(await ee(),await Pe(!0)),G()}),i.commands.registerCommand("arduinoMcp.startSketch",async()=>{e.show(!0);let l=await i.window.showInputBox({title:"Start new sketch",prompt:"Sketch name",placeHolder:"BlinkNano33",ignoreFocusOut:!1,validateInput:x=>{let T=x.trim();return T?/^[A-Za-z0-9_\-]+$/.test(T)?null:"Use only letters, numbers, underscore, or dash.":"Sketch name is required."}});if(!l)return;let c=i.Uri.file(n),p=await i.window.showOpenDialog({canSelectFiles:!1,canSelectFolders:!0,canSelectMany:!1,defaultUri:c,openLabel:"Create Sketch Here",title:"Choose parent folder"});if(!p?.[0])return;let f=p[0].fsPath,y=j.join(f,l.trim());e.appendLine(`$ arduino-cli sketch new "${y}"`);let A=await Y(["sketch","new",y]);if(e.appendLine(A.stdout),e.appendLine(A.stderr),!A.success){i.window.showErrorMessage("Grease: Failed to create sketch. See output.");return}let b=j.join(y,`${l.trim()}.ino`);try{let x=await i.workspace.openTextDocument(i.Uri.file(b));await i.window.showTextDocument(x,{preview:!1})}catch(x){e.appendLine(`Could not open sketch file automatically: ${x instanceof Error?x.message:String(x)}`)}e.appendLine(`Sketch created: ${y}`),i.window.showInformationMessage(`Grease: Sketch created (${l.trim()}).`)}),i.commands.registerCommand("arduinoMcp.installCore",async()=>{let l=await i.window.showQuickPick(ye.map(c=>({label:c.name,description:c.id,detail:c.boards.join(" \xB7 "),id:c.id})),{title:"Install board core (online)",placeHolder:"Pick a Supported Core"});l&&await un(l.id)&&await me()}),i.commands.registerCommand("arduinoMcp.verify",async()=>{let l=await or();v.view?.webview.postMessage({type:"verifyResult",success:l.ok===!0})}),i.commands.registerCommand("arduinoMcp.upload",async()=>{let l=await Lo({verifyFirst:!0});v.view?.webview.postMessage({type:"uploadResult",success:l===!0})}),i.commands.registerCommand("arduinoMcp.refreshPortsBoards",async()=>{await nr()}),i.commands.registerCommand("arduinoMcp.selectTarget",async()=>{await nr()}),i.commands.registerCommand("arduinoMcp.toggleSerial",async()=>{te.isConnected?await te.stop():await i.commands.executeCommand("arduinoMcp.openSerialMonitor"),G()}),i.commands.registerCommand("arduinoMcp.openSerialMonitor",async()=>{e.show(!0);let l=h?.port??O[0]?.port??null;await te.start(l,9600),e.appendLine(`Type "baud=X" to set a new baud rate, or try any of the following commands: 'send="Hello World"', "clear", "disconnect", or "connect". Transmitting from ${l??"(unknown port)"} below:`)}),i.commands.registerCommand("arduinoMcp.serialCommand",async()=>{e.show(!0);let l=await i.window.showInputBox({title:"Serial command",prompt:"Enter serial command",placeHolder:'baud=9600 | send="Hello World" | clear | disconnect | connect',ignoreFocusOut:!1});l&&await te.handleConsoleCommand(l)}),i.commands.registerCommand("arduinoMcp.openSerialPlotter",async()=>{e.show(!0);let l=h?.port??O[0]?.port??null;te.isConnected&&(await te.stop(),e.appendLine("Serial disconnected (Plotter opened).")),sn.show(l,!0),setTimeout(()=>{i.commands.executeCommand("workbench.action.moveEditorToNewWindow")},500),G()}),i.commands.registerCommand("arduinoMcp.openExamples",async()=>{e.show(!0),te.isConnected&&(await te.stop(),e.appendLine("Serial disconnected (Examples panel opened)."),G()),yt.show(h?.fqbn??null)}),i.commands.registerCommand("arduinoMcp.openGreasePanel",async()=>{v.showTab("grease"),await i.commands.executeCommand(`${He.viewType}.focus`)}),i.commands.registerCommand("arduinoMcp.reinstallSkill",async()=>{await t.globalState.update(Zn,!1);let l=Gt(Re,{force:!0});for(let f of l)e.appendLine(`[Grease] Skill ${f.action}: ${f.file}${f.backup?` (your copy: ${f.backup})`:""}`);let c=l.filter(f=>f.action==="error"),p=l.filter(f=>f.backup).map(f=>f.backup);c.length?i.window.showErrorMessage(`Grease: the skill was not written to ${c.map(f=>f.file).join(", ")}: ${c[0].error}`):i.window.showInformationMessage(`Grease: the Grease Skill is installed for ${l.map(f=>f.readers).join("; ")}.${p.length?` Your edited copy is saved as ${p.join(", ")}.`:""} Restart your agent if /grease does not show up.`),v.postGrease()}),i.commands.registerCommand("arduinoMcp.removeSkill",async()=>{if(await i.window.showWarningMessage("Remove the Grease Skill from your agents?",{modal:!0,detail:"Grease deletes only the skill files it wrote. /grease stops working until you press Reinstall skill."},"Remove")!=="Remove")return;await t.globalState.update(Zn,!0);let c=_r();for(let f of c)e.appendLine(`[Grease] Skill ${f.action}: ${f.file}`);let p=c.filter(f=>f.action==="kept-edited");i.window.showInformationMessage(p.length?`Grease: removed. Kept your edited copy: ${p.map(f=>f.file).join(", ")}.`:"Grease: the Grease Skill was removed. Reinstall skill in Help puts it back."),v.postGrease()}),i.commands.registerCommand("arduinoMcp.showSkill",async()=>{let l=Dt(Re).find(p=>p.state!=="missing"),c=l?await i.workspace.openTextDocument(i.Uri.file(l.file)):await i.workspace.openTextDocument({language:"markdown",content:Re});await i.window.showTextDocument(c,{preview:!0}),l&&await i.commands.executeCommand("workbench.action.files.setActiveEditorReadonlyInSession")}),i.commands.registerCommand("arduinoMcp.addAgentTools",async()=>{await E(!0)}),i.commands.registerCommand("arduinoMcp.cycleAccentColor",async()=>{let l=["#005FA0","#6B0000","#A34300","#8F6809","#6B004A","#520A85","#004D00"],c=i.workspace.getConfiguration("workbench"),p=c.get("colorCustomizations")||{},f=p["statusBar.background"]||"#007ACC",A=(l.indexOf(f)+1)%l.length,b=l[A],x={...p,"statusBar.background":b,"statusBar.noFolderBackground":b,"statusBar.debuggingBackground":b,"statusBarItem.remoteBackground":b,focusBorder:b,"activityBarBadge.background":b,"panelTitle.activeBorder":b},T=i.workspace.workspaceFolders&&i.workspace.workspaceFolders.length>0?i.ConfigurationTarget.Workspace:i.ConfigurationTarget.Global;await c.update("colorCustomizations",x,T),e.appendLine(`Accent color cycled to ${b} (Target: ${T===i.ConfigurationTarget.Workspace?"Workspace":"Global"})`),G(),v.view?.webview.postMessage({type:"accentColor",color:b})}),i.commands.registerCommand("arduinoMcp.recoveryUpload",async()=>{if(!h?.fqbn){i.window.showWarningMessage("Grease: Pick a board from the Managers panel first.");return}let l=h.fqbn,c=j.join(on.tmpdir(),"grease-recovery","Blink");vt.mkdirSync(c,{recursive:!0}),vt.writeFileSync(j.join(c,"Blink.ino"),`void setup() { pinMode(LED_BUILTIN, OUTPUT); }
void loop() {
  digitalWrite(LED_BUILTIN, HIGH); delay(500);
  digitalWrite(LED_BUILTIN, LOW);  delay(500);
}
`),i.window.showInformationMessage("Grease: Double-tap RESET on your board now. Watching 10 seconds for bootloader port..."),e.appendLine("[Recovery] Watching for bootloader port (10s)...");let p=new Set(O.map(W=>W.port).filter(Boolean)),f=1e4,y=500,A=Date.now()+f,b=null;for(;Date.now()<A;){await ze(y);let g=(await dt(e)).candidates.map(L=>L.port).find(L=>typeof L=="string"&&!p.has(L));if(g){b=g;break}}if(!b){i.window.showWarningMessage("Grease: No bootloader port appeared within 10 seconds."),e.appendLine("[Recovery] Timed out \u2014 no new port detected.");return}e.appendLine(`[Recovery] Bootloader port found: ${b}. Uploading blink sketch...`);try{await Xt()}catch{}let x=le(await Y(["upload","-b",l,"-p",b,c],void 0,_.upload));try{await Yt()}catch{}if(e.appendLine(x.stdout),e.appendLine(x.stderr),!x.success){i.window.showErrorMessage("Grease: Recovery upload failed (see Output).");return}h={port:b,fqbn:l,userChosen:!0},await ge(t,h),await xe(h),_e(h),G(),await ze(1500);let T=await dt(e);O=T.candidates;let ne=T.candidates.find(W=>W.port===b);if(ne){let W=Vt(ne);if(W.length>0){let g=await zt(t);for(let L of W)g[L]={fqbn:l};await jn(t,g),e.appendLine(`[Recovery] BoardMemory saved (${W.join(", ")}) \u2192 ${l}`)}}i.window.showInformationMessage(`Grease: Recovery successful. Board is live on ${b}.`)})),G()}function Ys(){}0&&(module.exports={activate,deactivate});
